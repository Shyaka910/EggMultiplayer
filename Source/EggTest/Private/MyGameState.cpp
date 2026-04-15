// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"
#include "MyPlayerState.h"

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, JoinedPlayers);
}

void AMyGameState::AddScoreToPlayer(AMyPlayerState* InPlayer, int32 ScoreCount)
{
	if (!HasAuthority() || InPlayer == nullptr) return;

	InPlayer->AddScore(ScoreCount);
}

void AMyGameState::OnRep_JoinedPlayers()
{
	OnPlayerJoined.Broadcast(JoinedPlayers.Last());
}

void AMyGameState::SetUserNameToPlayer(AMyPlayerState* InPlayer, FName InUserName)
{
	InPlayer->SetPlayerUserName(InUserName);
}
