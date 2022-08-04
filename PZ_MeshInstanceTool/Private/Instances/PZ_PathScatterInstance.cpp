// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/PZ_PathScatterInstance.h"

#include "Kismet/KismetMathLibrary.h"

#include "Components/SplineComponent.h"


APZ_PathScatterInstance::APZ_PathScatterInstance()
{
	//Set SplineComponent
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

	if( SplineComponent )
	{
		SplineComponent->SetMobility(EComponentMobility::Type::Static);
		SplineComponent->SetupAttachment(RootComponent);
	}
}

void APZ_PathScatterInstance::OnConstruction(const FTransform& Transform)
{
	SetupInstances();

	SplineComponent->SetClosedLoop(SplineClosed, true);

	SpawnPathScatter();
}

void APZ_PathScatterInstance::SpawnPathScatter()
{
	int32 LIndex;
	for( int i = 0; i < NumInstances; i++ )
	{
		FVector LLocationOut;
		FRotator LRotationOut;
		bool LCanSpawn;

		FVector LPosition;

		CurrentDistance = RandomStream.FRandRange(0, SplineComponent->GetSplineLength());

		SetCurrentInstance(LIndex);

		if( UsingScale )
		{
			LPosition = GetRandomLocationScaled(
				 SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local),
				SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local).RotateVector(SplineComponent->GetScaleAtDistanceAlongSpline(CurrentDistance))
				* RandomScaleDistance);
		}
		else
		{
			LPosition = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local);
		}
		
		PlacementLineTrace(
			GetRandomLocation( GetLocationXYOffset(LPosition)), 
			LineTracingDistance, LLocationOut, LRotationOut, LCanSpawn);
	
		if( LCanSpawn && GetRandomChance() )
		{
			FRotator LRotation = GetLookAtActor( LLocationOut, LRotationOut);
			
			if(FollowedSplineRotation)
			{
				LRotation = FRotator(FQuat(SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local)) * FQuat(LRotation));
			}

			FTransform LTransform = UKismetMathLibrary::MakeTransform(
				GetLocationZOffset(LLocationOut), 
				GetRandomRotation( GetRotationOffset(LRotation)), 
				GetRandomScale());
			
			CreateInstance(LTransform);
		}
	}
}
