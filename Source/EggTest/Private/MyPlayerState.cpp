// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerSessionSubsystem.h"

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, PlayerScore);
	DOREPLIFETIME(AMyPlayerState, PlayerUserName);
}

int32 AMyPlayerState::GetPlayerScore()
{
	return PlayerScore;
}

void AMyPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwningController() && GetOwningController()->IsLocalController())
	{
		SetPlayerUserName();
	}
}

void AMyPlayerState::SetPlayerUserName()
{
	UMultiplayerSessionSubsystem* MultiplayerSession = GetGameInstance()->GetSubsystem<UMultiplayerSessionSubsystem>();

	if (MultiplayerSession)
		ServerSetPlayerUserName(MultiplayerSession->GetOwningSessionPlayerName());

}

void AMyPlayerState::ServerSetPlayerUserName_Implementation(FName Name)
{
	PlayerUserName = Name;
}

void AMyPlayerState::AddScore(int32 score)
{
	PlayerScore += score;
	PlayerScore = PlayerScore > 0 ? PlayerScore : 0;
}

FName AMyPlayerState::GetPlayerUserName()
{
	return PlayerUserName;
}
