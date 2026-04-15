// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	float SpawnDelay = 3.f;

	UFUNCTION(BlueprintCallable)
	void RespawnPlayer(APlayerController* PlayerController);

protected:

	virtual void PostLogin(APlayerController* Newplayer) override;
	
private:
	int32 PlayerJoinCount = 0;
};
