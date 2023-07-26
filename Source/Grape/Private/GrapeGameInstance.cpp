// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapeGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include <Kismet\GameplayStatics.h>

UGrapeGameInstance::UGrapeGameInstance() {

	isLoggedIn = false;
}

void UGrapeGameInstance::Init() {
	Super::Init();

	if (this->IsDedicatedServerInstance()) {
		CreateSession();
	}
}

void UGrapeGameInstance::Login()
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());

	if (OnlineSubsystem) {
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {

			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UGrapeGameInstance::OnLoginComplete);
			Identity->Login(0, Credentials);
		}
	}
}

void UGrapeGameInstance::CreateSession()
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();

	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bIsDedicated = true;
	SessionSettings->bShouldAdvertise = true; 
	SessionSettings->bUsesPresence = false; 
	SessionSettings->bAllowJoinViaPresence = false;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	SessionSettings->Settings.Add(
		FName(TEXT("GrapeMatchmaking")),
		FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));

	// Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
	if (!Session->CreateSession(0, FName(TEXT("GrapeSession")), *SessionSettings))
	{
		// Call didn't start, return error.
	}

	Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
		this,
		&UGrapeGameInstance::HandleCreateSessionComplete));
}

void UGrapeGameInstance::FindAndJoinSession()
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());

	if (OnlineSubsystem) {

		IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();

		if (Session) {

			SessionSearch = MakeShareable(new FOnlineSessionSearch);
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->QuerySettings.SearchParams.Empty();
			Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UGrapeGameInstance::OnFindSessionCompleted);
			Session->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}
}

void UGrapeGameInstance::OnFindSessionCompleted(
	bool bWasSuccessful)
{
	if (bWasSuccessful) {
		OnlineSubsystem = Online::GetSubsystem(this->GetWorld());

		if (OnlineSubsystem) {

			IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();

			if (Session) {

				if (SessionSearch->SearchResults.Num() > 0) {

					Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UGrapeGameInstance::OnJoinSessionCompleted);
					Session->JoinSession(0, FName("GrapeSession"), SessionSearch->SearchResults[0]);
				}
			}
		}
	}
}

void UGrapeGameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult)
{
	if (JoinResult == EOnJoinSessionCompleteResult::Success) {

		UE_LOG(LogTemp, Warning, TEXT("Session Joined Success"));

		if (APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
			FString JoinAddress;

			OnlineSubsystem = Online::GetSubsystem(this->GetWorld());

			if (OnlineSubsystem) {

				IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();

				if (Session) {

					Session->GetResolvedConnectString(FName("GrapeSession"), JoinAddress);
					UE_LOG(LogTemp, Warning, TEXT("Join Address %s"), *JoinAddress);

					if (!JoinAddress.IsEmpty()) {

						Controller->ClientTravel(JoinAddress, TRAVEL_Absolute);
					}
				}
			}
		}
	}
}

void UGrapeGameInstance::DestroySession()
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	
	if (OnlineSubsystem) {

		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface) {
			
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGrapeGameInstance::HandleDestroySessionComplete);
			SessionInterface->DestroySession(FName("GrapeSession"));
		}
	}
}

void UGrapeGameInstance::HandleDestroySessionComplete(
	FName SessionName,
	bool bWasSuccessful)
{
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Session Destroyed: true"));
	}
}

void UGrapeGameInstance::HandleCreateSessionComplete(
	FName SessionName,
	bool bWasSuccessful)
{
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Warning, TEXT("Session Created: true"));
	}
}

void UGrapeGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error) {

	UE_LOG(LogTemp, Warning, TEXT("loggedin: %d"), bWasSuccessful);

	UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error);

	isLoggedIn = bWasSuccessful;

	if (OnlineSubsystem) {
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {

			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void UGrapeGameInstance::UpdateStats(FString StatName, int32 NewScore)
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());

	if (isLoggedIn && OnlineSubsystem) {

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("It Has OnlineSubsystem"));

		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface()) {

			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("It Has IdentityInterface"));

			FOnlineStatsUserUpdatedStats Stat = FOnlineStatsUserUpdatedStats(Identity->GetUniquePlayerId(0).ToSharedRef());

			Stat.Stats.Add(StatName, FOnlineStatUpdate(NewScore, FOnlineStatUpdate::EOnlineStatModificationType::Sum));

			TArray<FOnlineStatsUserUpdatedStats> Stats;
			Stats.Add(Stat);

			if (IOnlineStatsPtr StatsInterface = OnlineSubsystem->GetStatsInterface()) {

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("It Has StatsInterface"));

				StatsInterface->UpdateStats(
					Identity->GetUniquePlayerId(0).ToSharedRef(),
					Stats,
					FOnlineStatsUpdateStatsComplete::CreateLambda([](
						const FOnlineError& ResultState)
						{
							UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *ResultState.ToLogString());
							UE_LOG(LogTemp, Warning, TEXT("Points sent: %d"), ResultState.bSucceeded);
						}));
			}
		}
	}
}

void UGrapeGameInstance::GetStats(TArray<FString> StatNames)
{
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
	IOnlineStatsPtr StatsInterface = OnlineSubsystem->GetStatsInterface();

	TArray<TSharedRef<const FUniqueNetId>> Users;
	Users.Add(Identity->GetUniquePlayerId(0).ToSharedRef());

	StatsInterface->QueryStats(
		Identity->GetUniquePlayerId(0).ToSharedRef(),
		Users,
		StatNames,
		FOnlineStatsQueryUsersStatsComplete::CreateLambda([](
		const FOnlineError& ResultState,
		const TArray<TSharedRef<const FOnlineStatsUserStats>>& UsersStatsResult)
		{
			if (!ResultState.bSucceeded)
			{
				return;
			}

			for (auto QueriedStats : UsersStatsResult)
			{
				for (auto KV : QueriedStats->Stats)
				{
					int32 Value;
					KV.Value.GetValue(Value);
					UE_LOG(LogTemp, Warning, TEXT("Points: %s"), *FString::FromInt(Value));
				}
			}
		}));
}
