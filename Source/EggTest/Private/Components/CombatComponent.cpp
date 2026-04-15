// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UCombatComponent::K2_HandleFire()
{
	GASFireLocally();

}

void UCombatComponent::GASFireLocally_Implementation()
{
	APlayerController* Ctrl = nullptr;
	APawn* InOwner = Cast<APawn>(GetOwner());
	if (InOwner)
	{
		Ctrl = Cast<APlayerController>(InOwner->GetController());
	}

	if (!Ctrl) return;

	const UGameViewportClient* InLocalPlayerViewport = GEngine->GameViewport;
	FVector2D InViewportSize;
	InLocalPlayerViewport->GetViewportSize(InViewportSize);
	FVector2D CenterPoint(InViewportSize.X / 2.f, InViewportSize.Y / 2.f);

	FVector StartLocation;
	FVector StartDirection;

	UGameplayStatics::DeprojectScreenToWorld(Ctrl, CenterPoint, StartLocation, StartDirection);

	StartLocation = StartLocation + (StartDirection * 300.f);
	FVector InFireEnd = (StartLocation + (StartDirection * FireDistance));

	OnServerFireSuccess(StartLocation, InFireEnd);
}

void UCombatComponent::OnServerFireSuccess_Implementation(FVector_NetQuantize HitStart, FVector_NetQuantize HitEnd)
{
	FHitResult HitRes;
	FCollisionQueryParams QuerryParams;
	QuerryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitRes, HitStart, HitEnd, ECC_Visibility, QuerryParams))
	{
		if (AActor* HittedActor = HitRes.GetActor())
		{
			AController* Ctrl = nullptr;
			APawn* InOwner = Cast<APawn>(GetOwner());
			if (InOwner)
			{
				Ctrl = Cast<AController>(InOwner->GetController());

				if (Ctrl)
				{
					// Send GameplayEvent
					FGameplayEventData Eventdata;
					Eventdata.Instigator = GetOwner();
					Eventdata.Target = GetOwner();
					Eventdata.OptionalObject = HitRes.GetActor();

					if (UAbilitySystemComponent* AbilityComp = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
					{
						AbilityComp->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("Event.Fire.Hit"), &Eventdata);
					}
				}
			}
		}
	}
}

void UCombatComponent::Fire()
{
	// play fire effect locally
	//PlayFireEffect();

	APlayerController* Ctrl = nullptr;
	APawn* InOwner = Cast<APawn>(GetOwner());
	if (InOwner)
	{
		Ctrl = Cast<APlayerController>(InOwner->GetController());
	}

	if (!Ctrl) return;

	const UGameViewportClient* InLocalPlayerViewport = GEngine->GameViewport;
	FVector2D InViewportSize;
	InLocalPlayerViewport->GetViewportSize(InViewportSize);
	FVector2D CenterPoint(InViewportSize.X / 2.f, InViewportSize.Y / 2.f);

	FVector StartLocation;
	FVector StartDirection;

	UGameplayStatics::DeprojectScreenToWorld(Ctrl, CenterPoint, StartLocation, StartDirection);

	StartLocation = StartLocation + (StartDirection * 300.f);
	FVector InFireEnd = (StartLocation + (StartDirection * FireDistance));

	// Server play fire effect
	ServerFireEffect(StartLocation, InFireEnd);
}

void UCombatComponent::ServerFireEffect_Implementation(FVector_NetQuantize FireStart, FVector_NetQuantize FireEnd)
{
	//MulticastFireEffect();

	// TO-DO: Handle Fire Trace and Damage Logic

	
	FHitResult HitRes;
	FCollisionQueryParams QuerryParams;
	QuerryParams.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(HitRes, FireStart, FireEnd, ECC_Visibility, QuerryParams))
	{
		if (AActor* HittedActor = HitRes.GetActor())
		{
			AController* Ctrl = nullptr;
			APawn* InOwner = Cast<APawn>(GetOwner());
			if (InOwner)
			{
				Ctrl = Cast<AController>(InOwner->GetController());

				if (Ctrl)
				{
					// Send Damage
					UGameplayStatics::ApplyDamage(HittedActor, DamagePower, Ctrl, GetOwner(), UDamageType::StaticClass());
					MulticastPlayImpact(HitRes.ImpactPoint, HitRes.ImpactNormal);
				}
			}
		}
	}
}

void UCombatComponent::MulticastFireEffect_Implementation()
{
	APawn* InOwner = Cast<APawn>(GetOwner());
	if (InOwner && !InOwner->IsLocallyControlled())
		PlayFireEffect();
}

void UCombatComponent::PlayFireEffect()
{
	if (MuzzleFlash)
	{
		FVector MuzzleFlashLocation(GetOwner()->GetActorLocation().X,
			GetOwner()->GetActorLocation().Y,
			GetOwner()->GetActorLocation().Z + 100.f);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleFlashLocation, FRotator::ZeroRotator);
	}
}

void UCombatComponent::MulticastPlayImpact_Implementation(FVector_NetQuantize ImpactLoc, FVector ImpactDir)
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, ImpactLoc, ImpactDir.Rotation());
	}

	// Play impact sound effect
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, ImpactLoc);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);
}

void UCombatComponent::OnRep_Health()
{
	APawn* InOwner = Cast<APawn>(GetOwner());
	if (InOwner && InOwner->IsLocallyControlled())
	{
		OnHealthUpdated.Broadcast(Health);
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		Health = MaxHealth;
	}
	OnHealthUpdated.Broadcast(Health);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

