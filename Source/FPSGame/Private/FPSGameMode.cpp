// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "FPSGameState.h"
#include "Engine/World.h"
#include "FPSPlayerController.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

    GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		if (SpectatingViewpointClass)
		{
			TArray<AActor*> ReturnedActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

			if (ReturnedActors.Num() > 0)
			{
				AActor* NewViewTarget = ReturnedActors[0];
                for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
                {
                    AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
                    if (PC)
                    {
                        PC->SetViewTargetWithBlend(NewViewTarget, 1.5f, VTBlend_Cubic);
                    }
                }
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpoint class is nullptr. please update GameMode class with valid subclass."));
		}
	}

    AFPSGameState* GS = GetGameState<AFPSGameState>();
    if (GS)
    {
        GS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);
    }

	OnCompleteCompleted(InstigatorPawn, bMissionSuccess);
}
