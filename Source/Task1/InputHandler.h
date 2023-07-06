// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "InputHandler.generated.h"

// Class handling input and allowing movement (ASpectatorPawn)
UCLASS()
class TASK1_API AInputHandler : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInputHandler();
 
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input function
	void ChangeHeight(float AxisValue);

	// Input function
	void TogglePause();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
