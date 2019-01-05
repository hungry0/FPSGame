// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AI/Navigation/NavigationSystem.h"
#include "UnrealNetwork.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnSeenPawn);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnHearNoise);

	GuardState = EAIState::Idle;
}

void AFPSAIGuard::MoveToNextPoint()
{
	if (CurrentPoint == nullptr || CurrentPoint == SecondPatrolPoint)
	{
		CurrentPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPoint = SecondPatrolPoint;
	}

	UE_LOG(LogTemp, Log, TEXT("%s"), *(CurrentPoint->GetName()));

	UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPoint);
}

void AFPSAIGuard::OnRep_GuardState()
{
    OnGuardStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewGuardState)
{
	if (GuardState == NewGuardState)
	{
		return;
	}

    GuardState = NewGuardState;
    OnRep_GuardState();

	OnGuardStateChanged(NewGuardState);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotator = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPoint();
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotator);

	SetGuardState(EAIState::Idle);

	if (bPatrol)
	{
		MoveToNextPoint();
	}
}

void AFPSAIGuard::OnSeenPawn(APawn* SeenPawn)
{
	UE_LOG(LogTemp, Log, TEXT("On Seen Pawn"));

	if (SeenPawn == nullptr)
	{
		return;
	}

	SetGuardState(EAIState::Alerted);

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Blue, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}

	SetActorRotation(NewLookAt);
	SetGuardState(EAIState::Suspicious);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation,this, &AFPSAIGuard::ResetOrientation, 3.0f, false);
}

void AFPSAIGuard::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentPoint)
	{

		FVector Delta = GetActorLocation() - CurrentPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();
		if (DistanceToGoal < 100)
		{
			MoveToNextPoint();
		}
	}
}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFPSAIGuard, GuardState);
}

