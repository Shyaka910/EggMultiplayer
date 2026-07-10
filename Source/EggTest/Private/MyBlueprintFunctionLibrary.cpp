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

    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    const FVector ToTarget = WorldLocation - CameraLocation;
    const bool bBehindCamera =
        FVector::DotProduct(CameraRotation.Vector(), ToTarget) < 0.f;

    if (!PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
        return false;

    FVector2D Center = ViewportSize * 0.5f;

    if (bBehindCamera)
    {
        ScreenPosition = Center - (ScreenPosition - Center);
    }


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

    FVector2D Direction = ScreenPosition - Center;
    Direction.Normalize();

    // Avoid zero vector
    if (Direction.IsNearlyZero())
    {
        Direction = FVector2D(1.f, 0.f);
    }


    /*
        Find where the line from center to enemy
        intersects the screen rectangle
    */


    float ScaleX = (FMath::Abs(Direction.X) > 0.001f)
        ? Center.X / FMath::Abs(Direction.X)
        : FLT_MAX;

    float ScaleY = (FMath::Abs(Direction.Y) > 0.001f)
        ? Center.Y / FMath::Abs(Direction.Y)
        : FLT_MAX;


    // Padding from screen edge
    const float Padding = 100.f;

    Direction = ScreenPosition - Center;
    Direction.Normalize();

    float X = (ViewportSize.X * 0.5f - Padding) / FMath::Abs(Direction.X);
    float Y = (ViewportSize.Y * 0.5f - Padding) / FMath::Abs(Direction.Y);

    float Distance = FMath::Min(X, Y);

    FVector2D EdgePos = Center + Direction * Distance;

    EdgePos.X = FMath::Clamp(EdgePos.X, Padding, ViewportSize.X - Padding);
    EdgePos.Y = FMath::Clamp(EdgePos.Y, Padding, ViewportSize.Y - Padding);


    FVector2D EdgePosition =
        Center + Direction * Distance;

    Direction = ScreenPosition - Center;

    FVector2D EdgeDirection = Direction.GetSafeNormal();

    EdgePosition -= EdgeDirection * Padding;


    OutScreenPos = EdgePos;


    FVector2D ToEnemy = ScreenPosition - OutScreenPos;

    OutAngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));

    /*UE_LOG(LogTemp, Warning, TEXT("ScreenPos = %s"), *ScreenPosition.ToString());
    UE_LOG(LogTemp, Warning, TEXT("Direction = %s"), *Direction.ToString());
    UE_LOG(LogTemp, Warning, TEXT("ScaleX = %f  ScaleY = %f"), ScaleX, ScaleY);
    UE_LOG(LogTemp, Warning, TEXT("EdgePos = %s"), *EdgePosition.ToString());*/

    return true;
}
