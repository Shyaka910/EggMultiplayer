// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyPlayerHUD.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerHUD::BeginPlay()
{
	Super::BeginPlay();
	AddPlayerHUD();
}

void AMyPlayerHUD::AddPlayerHUD()
{
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (OwningPlayerController && PlayerHUD)
	{
		PlayerHudWidget = CreateWidget<UUserWidget>(OwningPlayerController, PlayerHUD);
		PlayerHudWidget->AddToViewport(0);
	}

	// create player health HUD

	if (OwningPlayerController && PlayerHealthHUD)
	{
		PlayerHealthWidget = CreateWidget<UUserWidget>(OwningPlayerController, PlayerHealthHUD);
		PlayerHealthWidget->AddToViewport(0);
	}
}
