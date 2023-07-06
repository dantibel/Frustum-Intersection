// Fill out your copyright notice in the Description page of Project Settings.

#include "InputHandler.h"
#include <cassert>
#include "Kismet/GameplayStatics.h"
#include "IntersectionVisualizer.h"

// Sets default values
AInputHandler::AInputHandler()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AInputHandler::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInputHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AInputHandler::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("ChangeHeight", this, &AInputHandler::ChangeHeight);
	PlayerInputComponent->BindAction("TogglePause", IE_Released, this, &AInputHandler::TogglePause);
}

// Input functions
void AInputHandler::ChangeHeight(float AxisValue)
{
	if (AxisValue == 0)
		return;

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIntersectionVisualizer::StaticClass(), actors);
	if (actors.GetData() != nullptr)
	{
		auto* actor = dynamic_cast<AIntersectionVisualizer*>(actors[0]);
		assert(actor != nullptr);
		actor->SetPlaneHeight(actor->GetPlaneHeight() + AxisValue);
	}
}

// Input function
void AInputHandler::TogglePause()
{
	// switch TestAcor flag
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AIntersectionVisualizer::StaticClass(), actors);
	if (actors.GetData() != nullptr)
	{
		auto* actor = dynamic_cast<AIntersectionVisualizer*>(actors[0]);
		assert(actor != nullptr);
		actor->SetPause(!actor->IsPause());
	}
};