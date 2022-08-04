// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceParent.h"
#include "PZ_PathScatterInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PZ_MESHINSTANCETOOL_API APZ_PathScatterInstance : public APZ_MeshInstanceParent
{
	GENERATED_BODY()

//Constructor
public:

	APZ_PathScatterInstance();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

//c++ methods
private:
	UFUNCTION()
		void SpawnPathScatter();

//Blueprint values
public:

	UPROPERTY(BlueprintReadOnly, Category = "PathScatterInstance")
		class USplineComponent* SplineComponent = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathScatterInstance")
		int32 NumInstances = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathScatterInstance")
		int32 RandomScaleDistance = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathScatterInstance")
		bool UsingScale = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathScatterInstance")
		bool FollowedSplineRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathScatterInstance")
		bool SplineClosed = false;

	UPROPERTY(BlueprintReadWrite, Category = "PathScatterInstance")
		float CurrentDistance;
};
