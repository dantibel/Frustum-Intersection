// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InputHandler.h"
#include "Task1GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TASK1_API ATask1GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ATask1GameModeBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
