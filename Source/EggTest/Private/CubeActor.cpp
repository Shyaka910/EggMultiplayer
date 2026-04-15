// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeActor.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"
#include "MyGameState.h"
#include "MyPlayerState.h"
// Sets default values
ACubeActor::ACubeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);

	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	CubeMesh->SetupAttachment(RootComponent);

	CubeMesh->SetGenerateOverlapEvents(true);
}

void ACubeActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && BoxComp)
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACubeActor::OnBeginOverlap);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &ACubeActor::OnEndOverlap);
	}
}

void ACubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACubeActor, RandomColor);
}

void ACubeActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// this function is running on the server

	if (AMyPlayerCharacter* MyCharacter = Cast<AMyPlayerCharacter>(OtherActor))
	{
		MyCharacter->EquippedCube = this;
		SetOwner(MyCharacter);
	}
}

void ACubeActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// this function is running on the server

	if (AMyPlayerCharacter* MyCharacter = Cast<AMyPlayerCharacter>(OtherActor))
	{
		MyCharacter->EquippedCube = nullptr;
		SetOwner(nullptr);
	}
}

void ACubeActor::OnRep_RandomColor(FLinearColor LastColor)
{
	//UpdateCubeColor();

	UMaterialInstanceDynamic* MatInst = GetMeshComp()->CreateDynamicMaterialInstance(0);

	if (MatInst)
	{
		MatInst->SetVectorParameterValue(FName("Color"), RandomColor);
	}
}

void ACubeActor::UpdateCubeColor()
{
	ServerUpdateCubColor(FLinearColor::MakeRandomColor());
}

void ACubeActor::ServerUpdateCubColor_Implementation(FLinearColor GeneratedColor)
{
	RandomColor = GeneratedColor;
	UMaterialInstanceDynamic* MatInst = GetMeshComp()->CreateDynamicMaterialInstance(0);

	if (MatInst)
	{
		MatInst->SetVectorParameterValue(FName("Color"), RandomColor);
	}

	// add score to player
	if (AMyPlayerCharacter* ScoredPlayer = Cast<AMyPlayerCharacter>(GetOwner()))
	{
		if (AMyPlayerState* ScoredPlayerState = Cast<AMyPlayerState>(ScoredPlayer->GetPlayerState()))
		{
			if (AMyGameState* GS = Cast<AMyGameState>(GetWorld()->GetGameState()))
			{
				GS->AddScoreToPlayer(ScoredPlayerState, ScorePoints);
			}
		}
	}
}
