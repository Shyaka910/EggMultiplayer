// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "CombatComponent.generated.h"

/**
* Delegates
*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdated, float, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EGGTEST_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Replicated, BlueprintReadOnly)
	float Health;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 50))
	float DamagePower = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* ImpactSound;

	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdated OnHealthUpdated;

	/**
	 * Event Data will be sent after sending fire
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "HandleFire"))
	void K2_HandleFire();

	UFUNCTION(Client, Reliable)
	void GASFireLocally();

	UFUNCTION(Server, Reliable)
	void OnServerFireSuccess(FVector_NetQuantize HitStart, FVector_NetQuantize HitEnd);

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFireEffect(FVector_NetQuantize FireStart, FVector_NetQuantize FireEnd);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireEffect();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PlayFireEffect();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayImpact(FVector_NetQuantize ImpactLoc, FVector ImpactDir);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
