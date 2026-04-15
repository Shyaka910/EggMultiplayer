// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, Value)\
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, Value)\
GAMEPLAYATTRIBUTE_VALUE_GETTER(Value)\
GAMEPLAYATTRIBUTE_VALUE_SETTER(Value)\
GAMEPLAYATTRIBUTE_VALUE_INITTER(Value)

/**
 * 
 */
UCLASS()
class EGGTEST_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UCharacterAttributeSet();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing= OnRep_Health, Category = "AttributeTest")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, Health)

	UFUNCTION()
	void OnRep_Health(FGameplayAttributeData& OldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing= OnRep_MaxHealth, Category = "AttributeTest")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_MaxHealth(FGameplayAttributeData& OldMaxHealth);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
