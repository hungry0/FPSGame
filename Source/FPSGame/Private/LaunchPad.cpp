// Fill out your copyright notice in the Description page of Project Settings.

#include "LaunchPad.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ALaunchPad::ALaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetBoxExtent(FVector(75.0f));
	RootComponent = OverlapComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(OverlapComp);
	MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	launchStrength = 1500.0f;
	launchPitchAngle = 35.0f;

}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaunchPad::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	UE_LOG(LogTemp, Log, TEXT("Touch the LaunchPad"));

	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += launchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * launchStrength;

	AFPSCharacter* OtherCharactor = Cast<AFPSCharacter>(OtherActor);
	if (OtherCharactor)
	{
		OtherCharactor->LaunchCharacter(LaunchVelocity, true, true);

		UGameplayStatics::SpawnEmitterAtLocation(this, ActivateLaunchPadEffect, GetActorLocation());
	}
}

