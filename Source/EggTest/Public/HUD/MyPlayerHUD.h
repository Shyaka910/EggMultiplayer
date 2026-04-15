// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyPlayerHUD.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class EGGTEST_API AMyPlayerHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerHUD;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PlayerHealthHUD;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* PlayerHudWidget;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* PlayerHealthWidget;

protected:

	virtual void BeginPlay() override;

	void AddPlayerHUD();
	
};
