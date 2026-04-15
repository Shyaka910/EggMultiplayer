// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MyGameState.generated.h"

class AMyPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoined, AMyPlayerState*, InPlayer);

/**
 * 
 */
UCLASS()
class EGGTEST_API AMyGameState : public AGameState
{
	GENERATED_BODY()

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UPROPERTY(ReplicatedUsing = OnRep_JoinedPlayers, BlueprintReadOnly)
	TArray<AMyPlayerState*> JoinedPlayers;

	UPROPERTY(EditDefaultsOnly)
	int32 LoseScoreFactor = 30;

	UFUNCTION()
	virtual void OnRep_JoinedPlayers();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoined OnPlayerJoined;

	void AddScoreToPlayer(AMyPlayerState* InPlayer, int32 ScoreCount);
	void SetUserNameToPlayer(AMyPlayerState* InPlayer, FName InUserName);

};
