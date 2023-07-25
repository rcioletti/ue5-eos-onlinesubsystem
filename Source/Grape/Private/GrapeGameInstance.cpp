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

	// The list of users to fetch stats for.
	TArray<TSharedRef<const FUniqueNetId>> Users;
	Users.Add(Identity->GetUniquePlayerId(0).ToSharedRef());

	// The stats to retrieve.
	//TArray<FString> StatNames;
	//StatNames.Add(TEXT("SCORE"));

	// The first parameter is the user performing the querying.
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

			// Each entry in UsersStatsResult is for a user.
			for (auto QueriedStats : UsersStatsResult)
			{
				// Each entry in Stats is for a stat for that user.
				for (auto KV : QueriedStats->Stats)
				{
					// KV.Key is the stat name.
					int32 Value;
					KV.Value.GetValue(Value);
					UE_LOG(LogTemp, Warning, TEXT("Points: %s"), *FString::FromInt(Value));
				}
			}
		}));

}
