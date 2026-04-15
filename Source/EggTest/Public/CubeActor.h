// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

UCLASS()
class EGGTEST_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACubeActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* CubeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RandomColor)
	FLinearColor RandomColor = FLinearColor::White;

	UPROPERTY(EditAnywhere)
	int32 ScorePoints = 14;

	UFUNCTION()
	virtual void OnRep_RandomColor(FLinearColor LastColor);

	void UpdateCubeColor();

	UFUNCTION(Server, Reliable)
	void ServerUpdateCubColor(FLinearColor GeneratedColor);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:

	FORCEINLINE UStaticMeshComponent* GetMeshComp() { return CubeMesh; }

};
