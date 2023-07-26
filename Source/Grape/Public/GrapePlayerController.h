// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GrapePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRAPE_API AGrapePlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void OnNetCleanup(UNetConnection* Connection) override;
	
};
