// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

UCharacterAbilitySystemComponent::UCharacterAbilitySystemComponent()
{
	
}

void UCharacterAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// make sure we're on the server before applying default effect
	if (IsOwnerActorAuthoritative())
	{
		FGameplayEffectContextHandle EffectHandle = MakeEffectContext();
		EffectHandle.AddSourceObject(GetOwner());

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DefaultEffectClass, 1.0f, EffectHandle);

		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		// Grant Abilities to Character
		GrantDefaultAbilities();
	}
}

void UCharacterAbilitySystemComponent::GrantDefaultAbilities()
{
	// loop through all default abilities and give them to the character. The -1 is the level of the ability, 
	// and the 0 is the input code for the ability (if you want to bind it to an input)
	for (TSubclassOf<UGameplayAbility>& InClass : DefaultAbilities)
	{
		if (InClass)
		{
			GiveAbility(FGameplayAbilitySpec(
				InClass,
				1,
				-1,
				this
			));
		}
	}
}
