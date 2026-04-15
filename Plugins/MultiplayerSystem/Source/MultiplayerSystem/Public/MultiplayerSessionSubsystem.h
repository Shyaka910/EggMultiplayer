// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSystem.h"

#include "MultiplayerSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSYSTEM_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UMultiplayerSessionSubsystem();

public:

	UFUNCTION(BlueprintCallable)
	void CreateGameSession(EMatchType MatchType);

	UFUNCTION(BlueprintCallable)
	void JoinGameSession(EMatchType MatchToJoin);

	UFUNCTION(BlueprintCallable)
	void StartSession();

	UFUNCTION(BlueprintCallable)
	void EndGameSession();

	UFUNCTION(BlueprintPure)
	bool GetIsSessionInProgress() const;

protected:

	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessul);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	class IOnlineSubsystem* OnlineSubsystem;
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnlineSessionSearchResult JoinedSessionResult;
	EMatchType ToJoinMatchType;
};
