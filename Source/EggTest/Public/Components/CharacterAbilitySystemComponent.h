// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CharacterAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	UCharacterAbilitySystemComponent();
	
public:

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class UGameplayEffect> DefaultEffectClass;

protected:

	virtual void BeginPlay() override;

private:

	void GrantDefaultAbilities();
};
