// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/PZ_PathInstance.h"

#include "Kismet/KismetMathLibrary.h"

#include "Components/SplineComponent.h"

APZ_PathInstance::APZ_PathInstance()
{
	//Set SplineComponent
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

	if( SplineComponent )
	{
		SplineComponent->SetMobility(EComponentMobility::Type::Static);
		SplineComponent->SetupAttachment(RootComponent);
	}
}

void APZ_PathInstance::OnConstruction(const FTransform& Transform)
{
	SetupInstances();

	SplineComponent->SetClosedLoop(SplineClosed, true);

	switch(PlacementType)
	{
	case EPathPlacementType::Spacing: SpawnPathBySpacing(); break;
	case EPathPlacementType::SplinePoint: SpawnPathBySpline(); break;
	}
}

void APZ_PathInstance::SpawnPathBySpacing()
{

	for(int i = 0; i < MaxInstance; i++)
	{
		float LDistance = 0;

		if(UsingInstanceLength) 
		{ 
			LDistance = CurrentDistance + CurrentInstanceLength;
		}
		else
		{ 
			LDistance = CurrentDistance; 
		}

		if(LDistance <= SplineComponent->GetSplineLength())
		{
			CreatePathInstance(i);
			CurrentDistance += Spacing;
			if( UsingInstanceLength ) CurrentDistance += CurrentInstanceLength;

			CurrentInstanceLength = GetInstanceSettings().InstanceLength;
		}
		else
		{
			break;
		}
	}
}

void APZ_PathInstance::CreatePathInstance(int IndexIn)
{
	int32 LIndex;
	SetCurrentInstance(LIndex);

	FVector LLocationOut;
	FRotator LRotationOut;
	bool LCanSpawn;

	float LDistance = 0;

	if(UsingInstanceLength)
	{
		LDistance = CurrentDistance + CurrentInstanceLength;
	}
	else
	{
		LDistance = CurrentDistance;
	}

	FVector LPosition = 
	SplineComponent->GetLocationAtDistanceAlongSpline(LDistance, ESplineCoordinateSpace::Local)
		+
	SplineComponent->GetRotationAtDistanceAlongSpline(LDistance, ESplineCoordinateSpace::Local).RotateVector(GetInstanceSettings().Location.LocationOffset);

	if( UseExtraMeshes )
	{
		if( IsValid(FirstMesh) && CurrentDistance == 0 )
		{
			SetupFirstMesh();
		}

		if( IsValid(LastMesh) && (UsingInstanceLength ? LDistance + Spacing + CurrentInstanceLength : LDistance + Spacing) >= SplineComponent->GetSplineLength() || IndexIn == MaxInstance - 1 )
		{
			SetupLastMesh();
		}
	}


	LPosition = GetRandomLocation(GetLocationXYOffset(LPosition));

	PlacementLineTrace( LPosition, LineTracingDistance, LLocationOut, LRotationOut, LCanSpawn);

	if( LCanSpawn && GetRandomChance() )
	{

	LRotationOut = GetLookAtActor( LLocationOut, LRotationOut);

	if(FollowedSplineRotation)
	{
		LRotationOut = FRotator(FQuat(SplineComponent->GetRotationAtDistanceAlongSpline(LDistance, ESplineCoordinateSpace::Local)) * FQuat(LRotationOut));
	}

		FTransform LTransform = UKismetMathLibrary::MakeTransform(
			GetLocationZOffset(LLocationOut), 
			GetRandomRotation(GetRotationOffset(LRotationOut)),
			GetRandomScale());

		CreateInstance(LTransform);
	}
}

void APZ_PathInstance::SpawnPathBySpline()
{
	int32 LIndex;

	int LSplinePoints = SplineComponent->GetNumberOfSplinePoints();
	
	for(int i = 0; i < LSplinePoints; i++ )
	{
		FVector LLocationOut;
		FRotator LRotationOut;
		bool LCanSpawn;

		SetCurrentInstance(LIndex);

		if(UseExtraMeshes)
		{
			if( IsValid(FirstMesh) && i == 0)
			{
				SetupFirstMesh();
			}
			
			if( IsValid(LastMesh) && i == LSplinePoints-1 )
			{
				SetupLastMesh();
			}
		}

		PlacementLineTrace(
			GetRandomLocation(
				GetLocationXYOffset(SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local))), LineTracingDistance,
			LLocationOut, LRotationOut, LCanSpawn);

		if(FollowedSplineRotation)
		{
			LRotationOut = FRotator(FQuat(SplineComponent->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::Local)) * FQuat(LRotationOut));
		}

		if(LCanSpawn && GetRandomChance())
		{
			FTransform LTransform = UKismetMathLibrary::MakeTransform(
				GetLocationZOffset(LLocationOut),
				GetRandomRotation(GetRotationOffset(LRotationOut)), 
				GetRandomScale());

			CreateInstance(LTransform);
		}
	}
}

void APZ_PathInstance::SetupFirstMesh()
{
	switch( InstanceType )
	{
	case EMeshInstanceType::StaticMesh: StaticMeshToSpawn = FirstMesh; break;
	case EMeshInstanceType::InstancedStaticMesh: InstancedStaticMeshToSpawn->SetStaticMesh(FirstMesh); break;
	case EMeshInstanceType::HierarchicalInstancedStaticMesh: HISMToSpawn->SetStaticMesh(FirstMesh); break;
	}
}

void APZ_PathInstance::SetupLastMesh()
{
	switch( InstanceType )
	{
	case EMeshInstanceType::StaticMesh: StaticMeshToSpawn = LastMesh; break;
	case EMeshInstanceType::InstancedStaticMesh: InstancedStaticMeshToSpawn->SetStaticMesh(LastMesh); break;
	case EMeshInstanceType::HierarchicalInstancedStaticMesh: HISMToSpawn->SetStaticMesh(LastMesh); break;
	}
}
