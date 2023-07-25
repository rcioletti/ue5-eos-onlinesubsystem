// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GrapeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GRAPE_API UGrapeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UGrapeGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void Login();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void CreateSession();

	void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void UpdateStats(FString StatName, int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void GetStats(TArray<FString> StatsName);

	virtual void Init() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool isLoggedIn;

	class IOnlineSubsystem* OnlineSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	int32 ScoreAPI;
};
