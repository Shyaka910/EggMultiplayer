// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()

	friend class AMyGameState;

public:

	UFUNCTION(BlueprintPure)
	FName GetPlayerUserName();

	UFUNCTION(BlueprintPure)
	int32 GetPlayerScore();

protected:

	void SetPlayerUserName(FName Name);
	void AddScore(int32 score);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UPROPERTY(Replicated)
	int32 PlayerScore = 0;

	UPROPERTY(Replicated)
	FName PlayerUserName;
	
};
