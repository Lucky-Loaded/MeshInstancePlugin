// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/PZ_CircleInstance.h"

#include "Kismet/KismetMathLibrary.h"

void APZ_CircleInstance::OnConstruction(const FTransform& Transform)
{
	SetupInstances();

	SpawnCircleInstance();
}

void APZ_CircleInstance::SpawnCircleInstance()
{
	int32 LIndex;
	for(int i = 0; i < NumInstances; i++)
	{
		SetCurrentInstance(LIndex);
		
		float LUnitAngle = Angle * 2 * PI / NumInstances / 360;
		float LAngle = LUnitAngle * i;
		float LSin = sinf(LAngle);
		float LCos = cosf(LAngle);

		FVector LUnitPlace = FVector(LSin, LCos, 0) * Radius;

		FVector LLocationOut;
		FRotator LRotatorOut;
		bool LCanSpawn;

		PlacementLineTrace(
		 GetRandomLocation(
				GetLocationXYOffset(LUnitPlace)), LineTracingDistance, LLocationOut, LRotatorOut, LCanSpawn);
	
		if(LCanSpawn && GetRandomChance())
		{
			CreateInstance(UKismetMathLibrary::MakeTransform(
				GetLocationZOffset(LLocationOut),
				GetRandomRotation(GetRotationOffset(GetLookAtActor( LLocationOut, LRotatorOut))),GetRandomScale()));
		}
	}
}
