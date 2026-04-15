// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyGameState.h"
#include "MyPlayerState.h"
#include "MyPlayerCharacter.h"

void AMyGameMode::RespawnPlayer(APlayerController* PlayerController)
{
	FTimerHandle SpawnTimer;

	GetWorldTimerManager().SetTimer(SpawnTimer, [this, PlayerController]()
		{
			if (PlayerController == nullptr) return;

			RestartPlayer(PlayerController);

			if (AMyGameState* GS = GetGameState<AMyGameState>())
			{
				if (AMyPlayerState* PS = PlayerController->GetPlayerState<AMyPlayerState>())
				{
					GS->AddScoreToPlayer(PS, GS->LoseScoreFactor * -1);
				}
			}
		},
		SpawnDelay,
		false
		);
}

void AMyGameMode::PostLogin(APlayerController* Newplayer)
{
	Super::PostLogin(Newplayer);

	// Register Player and Assign there names
	if (AMyGameState* GS = GetGameState<AMyGameState>())
	{
		if (AMyPlayerState* PS = Newplayer->GetPlayerState<AMyPlayerState>())
		{
			FString Name = FString::Printf(TEXT("Player_%d"), ++PlayerJoinCount);
			GS->SetUserNameToPlayer(PS,*Name);
			GS->JoinedPlayers.Add(PS);
			GS->OnPlayerJoined.Broadcast(PS);
		}
	}
}
