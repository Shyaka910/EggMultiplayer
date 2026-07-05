// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "../../../../../UE_5.7/Engine/Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"

bool UMyBlueprintFunctionLibrary::GetOffScreenIndicatorPosition(APlayerController* PC, FVector WorldLocation, FVector2D& OutScreenPos, float& OutAngleDegrees, bool& bIsOnScreen)
{
    if (!PC) return false;

    // --- Get DPI scale (THIS is what you're missing in most systems)
    const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(PC);

    int32 SizeX, SizeY;
    PC->GetViewportSize(SizeX, SizeY);

    // Convert to "Slate-style scaled space"
    const FVector2D ViewportSize(SizeX, SizeY);
    const FVector2D ScreenCenter = ViewportSize * 0.5f;

    FVector ScreenPos;

    // Project world → screen
    bool bProjected = PC->ProjectWorldLocationToScreenWithDistance(WorldLocation, ScreenPos);
    if (!bProjected) return false;

    // Apply DPI correction (IMPORTANT FIX)
    ScreenPos /= DPIScale;

    // Check if on screen (in DPI-corrected space)
    bIsOnScreen =
        ScreenPos.X >= 0.f && ScreenPos.X <= ViewportSize.X &&
        ScreenPos.Y >= 0.f && ScreenPos.Y <= ViewportSize.Y;

    if (bIsOnScreen)
    {
        OutScreenPos = FVector2D(ScreenPos.X, ScreenPos.Y);
        OutAngleDegrees = 0.f;
        return true;
    }

    // --- OFF SCREEN LOGIC (direction-based, NOT clamp-based)

    FVector2D Dir = (OutScreenPos - ScreenCenter).GetSafeNormal();

    if (Dir.IsNearlyZero())
    {
        Dir = FVector2D(1.f, 0.f);
    }

    // Edge padding in DPI space
    const float Padding = 80.f;
    const FVector2D HalfBounds(
        (ViewportSize.X * 0.5f) - Padding,
        (ViewportSize.Y * 0.5f) - Padding
    );

    // Push to screen edge in correct direction
    OutScreenPos = ScreenCenter + Dir * HalfBounds;

    // Rotation for arrow UI
    OutAngleDegrees = FMath::RadiansToDegrees(FMath::Atan2(Dir.Y, Dir.X));

    return true;
}
