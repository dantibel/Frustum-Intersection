// Copyright Epic Games, Inc. All Rights Reserved.

#include "Task1GameModeBase.h"
#include "IntersectionVisualizer.h"

ATask1GameModeBase::ATask1GameModeBase()
{
	DefaultPawnClass = AInputHandler::StaticClass();
}

void ATask1GameModeBase::BeginPlay()
{
   Super::BeginPlay();
   GetWorld()->SpawnActor(AIntersectionVisualizer::StaticClass());
}
