// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/PZ_ScatterInstance.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "Kismet/KismetMathLibrary.h"

APZ_ScatterInstance::APZ_ScatterInstance()
{
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
}

void APZ_ScatterInstance::OnConstruction(const FTransform& Transform)
{
	SetupInstances();

	DefinePlacementStyle();

	SpawnScatter();
}

void APZ_ScatterInstance::SpawnScatter()
{
	int32 LIndex;
	for( int i = 0; i < NumInstances; i++ )
	{

		SetCurrentInstance(LIndex);

		FVector LPosition;
		float LTraceDistance;

		FVector LLocationOut;
		FRotator LRotatorOut;
		bool LCanSpawn;

		switch(PlacementType)
		{
		case EScatterPlacementType::Box:

			LPosition = GetRandomPointInBox();
			LTraceDistance = BoxSize.Z;
			break;

		case EScatterPlacementType::Sphere:

			LPosition = GetRandomPointInSphere();
			LTraceDistance = LineTracingDistance;
			break;
		}

		PlacementLineTrace(
		GetRandomLocation(GetLocationXYOffset(LPosition)), LTraceDistance, LLocationOut, LRotatorOut, LCanSpawn);

		if( LCanSpawn && GetRandomChance() )
		{
			CreateInstance(UKismetMathLibrary::MakeTransform(

				GetLocationZOffset(LLocationOut),
				GetRandomRotation(GetRotationOffset(GetLookAtActor( LLocationOut, LRotatorOut)) ),GetRandomScale()));
		}
	}
}

void APZ_ScatterInstance::DefinePlacementStyle()
{
	switch(PlacementType)
	{
	case EScatterPlacementType::Box: 
		
		Box->SetVisibility(true,false);
		Sphere->SetVisibility(false, false);
		BoxExtent = BoxSize / 2;
		Box->SetBoxExtent(BoxExtent, true);

		if(BoxPivotPoint == EScatterBoxPivotPoint::AtCorner)
		{
			Box->SetRelativeLocation(BoxExtent, false, false);
		}
		break;

	case EScatterPlacementType::Sphere: 
		
		Box->SetVisibility(false, false);
		Sphere->SetVisibility(true, false);
		Sphere->SetSphereRadius(SphereRadius, true);
		break;
	}
}

FVector APZ_ScatterInstance::GetRandomPointInBox()
{
	FVector LLocation;

	switch(BoxPivotPoint)
	{
	case EScatterBoxPivotPoint::AtCenter: 
		
		LLocation = FVector(RandomStream.FRandRange(-BoxExtent.X, BoxExtent.X), RandomStream.FRandRange(-BoxExtent.Y, BoxExtent.Y), RandomStream.FRandRange(-BoxExtent.Z, BoxExtent.Z));
		break;

	case EScatterBoxPivotPoint::AtCorner: 
		
		LLocation = FVector(RandomStream.FRandRange(0, BoxSize.X), RandomStream.FRandRange(0, BoxSize.Y), RandomStream.FRandRange(0, BoxSize.Z));
		break;
	}

	if(IsSurfaceSnapping)
	{
		LLocation = FVector(LLocation.X, LLocation.Y, BoxExtent.Z);
	}

	return LLocation;
}

FVector APZ_ScatterInstance::GetRandomPointInSphere()
{
	return RandomStream.GetUnitVector() * RandomStream.FRandRange(0, SphereRadius);
}
