// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapePlayerController.h"
#include <GrapeGameInstance.h>
#include <Kismet\GameplayStatics.h>

void AGrapePlayerController::OnNetCleanup(UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);

	UGrapeGameInstance* GameInstance = Cast<UGrapeGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance) {
		
		GameInstance->DestroySession();
	}
}
