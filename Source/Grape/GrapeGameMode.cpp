// Copyright Epic Games, Inc. All Rights Reserved.

#include "GrapeGameMode.h"
#include "GrapeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGrapeGameMode::AGrapeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
