// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EGGTEST_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure)
    static bool GetOffScreenIndicatorPosition(
        APlayerController* PC,
        FVector WorldLocation,
        FVector2D& OutScreenPos,
        float& OutAngleDegrees,
        bool& bIsOnScreen
    );
	
};
