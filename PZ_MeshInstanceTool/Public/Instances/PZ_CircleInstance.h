// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceParent.h"
#include "PZ_CircleInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PZ_MESHINSTANCETOOL_API APZ_CircleInstance : public APZ_MeshInstanceParent
{

	GENERATED_BODY()

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UFUNCTION()
		void SpawnCircleInstance();

//Blueprint variables
public:

	/*
		Number of instances to spawn
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridInstance")
		int32 NumInstances = 10;
	/*
		Angle of circle
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridInstance")
		float Angle = 360.0f;
	/*
		Radius of circle
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridInstance")
		float Radius = 100.0f;

};
