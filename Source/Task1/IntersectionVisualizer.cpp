// Fill out your copyright notice in the Description page of Project Settings.

#include "IntersectionVisualizer.h"
#include <cassert>
#include "GameFramework/PlayerInput.h"

enum RayDirection
{
	bottomLeft = 0,
	topLeft,
	topRight,
	bottomRight
};

// Find ray intersection with horizontal plane
FVector RayHorPlaneIntersection(const FVector& RayOrig, const FVector& RayDir, float HorPlaneHeight)
{
	const auto coef = HorPlaneHeight - RayOrig.Z;
	return { RayOrig.X + RayDir.X * coef / RayDir.Z, RayOrig.Y + RayDir.Y * coef / RayDir.Z, HorPlaneHeight };
}

// Sets default values
AIntersectionVisualizer::AIntersectionVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0;

	// create intersection quadangle mesh
	Quadangle = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Quadangle"));
	assert(Quadangle != nullptr && "Quadangle mesh is not created");
	Quadangle->SetupAttachment(RootComponent);

	// const TArray<FVector> vertices {{ 0., 0., 200. }, { 0., 100., 200. }, { 100., 100., 200. }, { 100., 0., 200. }};
	const TArray<FVector> vertices {{ 0., 0., 0. }, { 0., 0., 0. }, { 0., 0., 0. }, { 0., 0., 0. }};
	const TArray<int32> indices { 0, 2, 1, 2, 0, 3 };
	const TArray<FColor> colors { FColor::Black, FColor::Black, FColor::Black, FColor::Black };
	Quadangle->CreateMeshSection(0, vertices, indices, {}, QuadangleTexCoords, {}, {}, {}, colors, {}, false);
	Quadangle->SetMeshSectionVisible(0, true);
	Quadangle->SetRelativeLocation(FVector(0., 0., 0.));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> material_asset(TEXT("/Game/SolidColor_green"));
	// static ConstructorHelpers::FObjectFinder<UMaterial> material_asset(TEXT("/Game/PlaneMaterial"));
	if (material_asset.Succeeded())
	{
		IntersectionMaterial = UMaterialInstanceDynamic::Create(material_asset.Object, nullptr);
		Quadangle->SetMaterial(0, IntersectionMaterial);
	}
}

// Called when the game starts or when spawned
void AIntersectionVisualizer::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AIntersectionVisualizer::Tick(float DeltaTime)
{
	int output_index = 0; // string index for screen debug messages

	if (!IsPause()) // real-time intersection update
	{
		Super::Tick(DeltaTime);

		// get camera parameters
		const auto cam_manager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		const auto cam_loc = cam_manager->GetCameraLocation();
		const auto cam_rot = cam_manager->GetCameraRotation();
		const auto cam_angle = cam_manager->GetFOVAngle();
		const auto forward = FRotationMatrix(cam_rot).GetScaledAxis(EAxis::X) * (180.0 - cam_angle);
		const auto right = FRotationMatrix(cam_rot).GetScaledAxis(EAxis::Y) * cam_angle;
		const auto up = FRotationMatrix(cam_rot).GetScaledAxis(EAxis::Z) * cam_angle;

		CamLocSaved = cam_loc;
		// calculate frustum rays
		const FVector3d rays_dirs[]{
			(forward - right - up).GetUnsafeNormal(),
			(forward - right + up).GetUnsafeNormal(),
			(forward + right + up).GetUnsafeNormal(),
			(forward + right - up).GetUnsafeNormal() };

		// find intersections
		if (cam_rot.Pitch - cam_angle * .5 >= 0 && cam_rot.Pitch - cam_angle * .5 <= 180 || PlaneHeight > cam_loc.Z) // No intersection
		{
			GEngine->AddOnScreenDebugMessage(++output_index, DeltaTime, FColor::Red, TEXT("NO frustum intersection"));
			return;
		}

		Intersects[bottomLeft] = RayHorPlaneIntersection(cam_loc, rays_dirs[bottomLeft], PlaneHeight);
		Intersects[bottomRight] = RayHorPlaneIntersection(cam_loc, rays_dirs[bottomRight], PlaneHeight);

		if (cam_rot.Pitch + cam_angle * .5 >= 360 || cam_rot.Pitch - cam_angle * .5 <= 180) // No intersection with frustum top plane 
		{
			GEngine->AddOnScreenDebugMessage(output_index, DeltaTime, FColor::Yellow, TEXT("NO frustum top plane intersection "));

			Intersects[topLeft] = Intersects[bottomLeft] + FVector::VectorPlaneProject(rays_dirs[topLeft] + rays_dirs[bottomLeft], { 0., 0., 1. }).GetSafeNormal() * ClipDistance;
			Intersects[topRight] = Intersects[bottomRight] + FVector::VectorPlaneProject(rays_dirs[topRight] + rays_dirs[bottomRight], { 0., 0., 1. }).GetSafeNormal() * ClipDistance;
		}
		else // has intersection with frustum top plane
		{
			GEngine->AddOnScreenDebugMessage(output_index, DeltaTime, FColor::Green, TEXT("HAS frustum top plane intersection "));

			Intersects[topLeft] = RayHorPlaneIntersection(cam_loc, rays_dirs[topLeft], PlaneHeight);
			Intersects[topRight] = RayHorPlaneIntersection(cam_loc, rays_dirs[topRight], PlaneHeight);

			// clip intersection by plane distance
			const auto left_line = Intersects[topLeft] - Intersects[bottomLeft];
			if (left_line.SquaredLength() > ClipDistance * ClipDistance)
				Intersects[topLeft] = Intersects[bottomLeft] + left_line.GetUnsafeNormal() * ClipDistance;
			const auto right_line = Intersects[topRight] - Intersects[bottomRight];
			if (right_line.SquaredLength() > ClipDistance * ClipDistance)
				Intersects[topRight] = Intersects[bottomRight] + right_line.GetUnsafeNormal() * ClipDistance;
		}

		for (size_t i = 0; i < 4; i++)
			GEngine->AddOnScreenDebugMessage(output_index++, 10000.f, FColor::Cyan, FString::Printf(TEXT("Intersection #%i location: %f, %f, %f"), i, Intersects[i].X, Intersects[i].Y, Intersects[i].Z));

		// update quadangle geometry
		const TArray<FVector> vertices(Intersects, 4);
		// const TArray<FVector> vertices {{ 0., 0., 0. }, { 0., 0., 0. }, { 0., 0., 0. }, { 0., 0., 0. }};
		IntersectionMaterial->SetVectorParameterValue("Position0", Intersects[bottomLeft]);
		IntersectionMaterial->SetVectorParameterValue("Position1", Intersects[topLeft]);
		IntersectionMaterial->SetVectorParameterValue("Position2", Intersects[topRight]);
		IntersectionMaterial->SetVectorParameterValue("Position3", Intersects[bottomRight]);
		// IntersectionMaterial->SetVectorParameterValue("Position0", FVector4(0., 0., 1., 1.));
		// IntersectionMaterial->SetVectorParameterValue("Position1", FVector4(0., 1., 0., 1.));
		// IntersectionMaterial->SetVectorParameterValue("Position2", FVector4(1., 0., 0., 1.));
		// IntersectionMaterial->SetVectorParameterValue("Position3", FVector4(1., 1., 0., 1.));

		Quadangle->SetRelativeLocation({ 0., 0., 0. });
		Quadangle->UpdateMeshSection(0, vertices, {}, QuadangleTexCoords, {}, {}, {}, {}, {});
	}
	else // no intersection update
		for (size_t i = 0; i < 4; i++)
		{
			DrawDebugLine(GetWorld(), CamLocSaved, Intersects[i], FColor{ 0, 0, 255, });
			DrawDebugSphere(GetWorld(), Intersects[i], 5, 10, FColor{ 0, 0, 255, });
		}
	GEngine->AddOnScreenDebugMessage(output_index++, DeltaTime, FColor::Cyan, FString::Printf(TEXT("Plane height: %f"), PlaneHeight));
}
