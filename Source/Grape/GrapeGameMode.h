// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GrapeGameMode.generated.h"

UCLASS(minimalapi)
class AGrapeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGrapeGameMode();

	void RegisterExistingPlayers();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	bool bAllExistingPlayersRegistered;
};



