// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)),
	FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionSubsystem::CreateGameSession(EMatchType MatchType)
{
	if (!SessionInterface.IsValid()) return;

	// check if we already have a session then destory it first

	if (auto Session = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}

	SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = (OnlineSubsystem->GetSubsystemName() == "NULL");
	SessionSettings.bAllowInvites = true;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.Set(FName("MatchType"), (int32)MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings);
}

void UMultiplayerSessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessul)
{
	if (!bWasSuccessul) return;
	
	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
	{
		GetWorld()->ServerTravel(FString("/Game/ThirdPerson/Maps/L_LobbyMap?listen"));
	}
}

void UMultiplayerSessionSubsystem::JoinGameSession(EMatchType MatchToJoin)
{
	if (!SessionInterface.IsValid()) return;

	ToJoinMatchType = MatchToJoin;
	SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	const ULocalPlayer* InLocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = OnlineSubsystem->GetSubsystemName() == "NULL";
	SessionSearch->MaxSearchResults = 5;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(FName("MatchType"), (int32)MatchToJoin, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(*InLocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void UMultiplayerSessionSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful || !SessionInterface) return;

	for (auto& InResult : SessionSearch->SearchResults)
	{
		int32 MatchTypeID = InResult.Session.SessionSettings.Get(FName("MatchType"), MatchTypeID) ?
			MatchTypeID : -1;
	
		bool bIsSameMatchType = MatchTypeID == (int32)ToJoinMatchType;


		if (InResult.IsValid() && bIsSameMatchType)
		{
			SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
			JoinedSessionResult = InResult;
			const ULocalPlayer* InPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			SessionInterface->JoinSession(*InPlayer->GetPreferredUniqueNetId(), NAME_GameSession, InResult);
			break;
		}
		else
			continue;
	}
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success || !SessionInterface) return;

	if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
	{
		if (JoinedSessionResult.IsValid())
		{
			FString HostAddress;
			SessionInterface->GetResolvedConnectString(JoinedSessionResult, NAME_GamePort, HostAddress);

			Controller->ClientTravel(HostAddress, ETravelType::TRAVEL_Absolute);
		}
	}

}

void UMultiplayerSessionSubsystem::StartSession()
{
	if (SessionInterface)
	{
		SessionInterface->StartSession(NAME_GameSession);
	}
}

void UMultiplayerSessionSubsystem::EndGameSession()
{
	if (SessionInterface)
	{
		SessionInterface->EndSession(NAME_GameSession);
	}
}

bool UMultiplayerSessionSubsystem::GetIsSessionInProgress() const
{
	return SessionInterface ? SessionInterface->GetSessionState(NAME_GameSession) == EOnlineSessionState::InProgress : false;
}
