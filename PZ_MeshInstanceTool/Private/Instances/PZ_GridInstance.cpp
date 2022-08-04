// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/PZ_GridInstance.h"

#include "Kismet/KismetMathLibrary.h"

APZ_GridInstance::APZ_GridInstance()
{
}

void APZ_GridInstance::OnConstruction(const FTransform& Transform)
{
	SetupInstances();

	SpawnGridInstance();
}

void APZ_GridInstance::SpawnGridInstance()
{
	int32 LIndex;
	FVector LUnitPlace;

	FVector LLocationOut;
	FRotator LRotatorOut;
	bool LCanSpawn;

	for( int x = 0; x < NumInstances.X; x++ )
	{
		LUnitPlace.X = Space.X * x;
		for( int y = 0; y < NumInstances.Y; y++)
		{
			LUnitPlace.Y = Space.Y * y;
			for(int z = 0; z < NumInstances.Z; z++)
			{
				SetCurrentInstance(LIndex);

				LUnitPlace.Z = Space.Z * z;

				PlacementLineTrace(
					GetRandomLocation(GetLocationXYOffset(LUnitPlace) ),
					LineTracingDistance, LLocationOut, LRotatorOut, LCanSpawn);

				if( LCanSpawn && GetRandomChance() )
				{
					FRotator LUnitRotation = GetLookAtActor( LLocationOut, LRotatorOut);
					LUnitRotation = GetRandomRotation(GetRotationOffset(LUnitRotation));
					FTransform LUnitTransform =
						UKismetMathLibrary::MakeTransform(GetLocationZOffset(LLocationOut), LUnitRotation, GetRandomScale());
					CreateInstance(LUnitTransform);
				}
			}
		}
	}
}
