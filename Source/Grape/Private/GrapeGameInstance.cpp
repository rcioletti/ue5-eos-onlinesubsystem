// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapeGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

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
		FName(TEXT("SessionSetting")),
		FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));

	// Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
	if (!Session->CreateSession(0, FName(TEXT("MyLocalSessionName")), *SessionSettings))
	{
		// Call didn't start, return error.
	}

	Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
		this,
		&UGrapeGameInstance::HandleCreateSessionComplete));
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
