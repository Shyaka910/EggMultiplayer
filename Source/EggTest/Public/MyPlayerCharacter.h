// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MyPlayerCharacter.generated.h"

class AEggActor;

UCLASS()
class EGGTEST_API AMyPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMyPlayerCharacter();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintReadOnly)
	class UCharacterAttributeSet* MyCharacterAttribute;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCharacterAbilitySystemComponent* PlayerAbilityComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCombatComponent* CombatComponent;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TravelToMap(FName MapName, bool bBacktoLobbyMap);

	// Add egg to trail
	void RegisterEgg(AEggActor* NewEgg);

	UFUNCTION(BlueprintCallable)
	void ChangePickedCube();

	// Launch first egg in your trail
	UFUNCTION(BlueprintCallable)
	void LaunchFirstEgg(float LaunchStrength, float UpwardStrength);

	// Launch all other players' eggs
	UFUNCTION(BlueprintCallable)
	void LaunchOtherPlayersEggs(float LaunchStrength, float UpwardStrength);

	// Multiplayer-safe versions
	UFUNCTION(Server, Reliable)
	void ServerLaunchFirstEgg(float LaunchStrength, float UpwardStrength);

	UFUNCTION(Server, Reliable)
	void ServerLaunchOtherPlayersEggs(float LaunchStrength, float UpwardStrength);

	// Multiplayer-safe versions
	UFUNCTION(Server, Reliable)
	void ServerSetOverlappedEgg(AActor* OtherActor);

	UFUNCTION(Server, Reliable)
	void ServerRemoveOverlappedEgg(AActor* OtherActor);

	// Trail array
	UPROPERTY(Replicated)
	TArray<AEggActor*> EggTrail;

	UPROPERTY(Replicated)
	AEggActor* OtherPlayerEggToThrow = nullptr;

	UPROPERTY(Replicated)
	class ACubeActor* EquippedCube;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnPlayerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnPlayerEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};

