// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "../../../../../UE_5.7/Engine/Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"

bool UMyBlueprintFunctionLibrary::GetOffScreenIndicatorPosition(APlayerController* PC, FVector WorldLocation, FVector2D& OutScreenPos, float& OutAngleDegrees, bool& bIsOnScreen)
{
    if (!PC)
        return false;

    int32 ViewportX;
    int32 ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);

    float DPIScale = UWidgetLayoutLibrary::GetViewportScale(PC);

    FVector2D ViewportSize(
        ViewportX / DPIScale,
        ViewportY / DPIScale
    );

    FVector2D ScreenPosition;

    if (!PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
        return false;


    // Convert projected position into UMG space
    ScreenPosition /= DPIScale;


    // Check if enemy is visible on screen
    bIsOnScreen =
        ScreenPosition.X >= 0 &&
        ScreenPosition.X <= ViewportSize.X &&
        ScreenPosition.Y >= 0 &&
        ScreenPosition.Y <= ViewportSize.Y;


    if (bIsOnScreen)
    {
        OutScreenPos = ScreenPosition;
        OutAngleDegrees = 0.f;
        return true;
    }


    // ---------- OFF SCREEN ----------

    FVector2D Center = ViewportSize * 0.5f;

    FVector2D Direction = ScreenPosition - Center;


    // Avoid zero vector
    if (Direction.IsNearlyZero())
    {
        Direction = FVector2D(1.f, 0.f);
    }


    /*
        Find where the line from center to enemy
        intersects the screen rectangle
    */

    float ScaleX = BIG_NUMBER;
    float ScaleY = BIG_NUMBER;


    if (!FMath::IsNearlyZero(Direction.X))
    {
        ScaleX = (ViewportSize.X * 0.5f) / FMath::Abs(Direction.X);
    }

    if (!FMath::IsNearlyZero(Direction.Y))
    {
        ScaleY = (ViewportSize.Y * 0.5f) / FMath::Abs(Direction.Y);
    }


    float Scale = FMath::Min(ScaleX, ScaleY);


    FVector2D EdgePosition =
        Center + Direction * Scale;


    // Padding from screen edge
    const float Padding = 100.f;

    FVector2D EdgeDirection = Direction.GetSafeNormal();

    EdgePosition -= EdgeDirection * Padding;


    OutScreenPos = EdgePosition;


    // Arrow rotation
    OutAngleDegrees = FMath::RadiansToDegrees(
        FMath::Atan2(Direction.Y, Direction.X)
    );


    return true;
}
