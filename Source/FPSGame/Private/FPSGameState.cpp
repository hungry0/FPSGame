// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "Engine/World.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatePawn, bool bMissionSuccess)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
        if (PC && PC->IsLocalController())
        {
            PC->OnMissionCompleted(InstigatePawn, bMissionSuccess);

            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                Pawn->DisableInput(PC);
            }
        }
    }
}
