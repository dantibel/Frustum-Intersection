// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "IntersectionVisualizer.generated.h"

// Class visualizing camera frustum intersection with horizontal plane 
UCLASS()
class TASK1_API AIntersectionVisualizer : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AIntersectionVisualizer();

	// Set plane height
	void SetPlaneHeight(float height)
	{
		PlaneHeight = std::max(MinPlaneHeight, height);
	}

	// Get plane height
	float GetPlaneHeight()
	{
		return PlaneHeight;
	}

	// Set pause
	void SetPause(bool pause)
	{
		Pause = pause;
	}

	// Is Pause
	bool IsPause() const
	{
		return Pause;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	static constexpr float MinPlaneHeight = 1.f;
	inline static const TArray<FVector2D> QuadangleTexCoords {{ 0., 0.}, { 0., 1. }, { 1., 1. }, { 1., 0. }};

	UPROPERTY(EditAnywhere)
	float PlaneHeight{ 200.f }; // height of plane to intersect

	UPROPERTY(EditAnywhere)
	bool Pause{ false }; // is dynamic intersection update on flag

	UPROPERTY(EditAnywhere)
	float ClipDistance{ 10000.f }; // plane intersection clip distance

	UProceduralMeshComponent* Quadangle{ nullptr }; // primitive consist of intersection points

	UMaterialInstanceDynamic* IntersectionMaterial{ nullptr }; // intersection plane material

	FVector CamLocSaved; // saved camera location
	FVector Intersects[4]; // intersection points
};
