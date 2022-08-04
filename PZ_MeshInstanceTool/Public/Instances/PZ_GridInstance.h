// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceParent.h"
#include "PZ_GridInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PZ_MESHINSTANCETOOL_API APZ_GridInstance : public APZ_MeshInstanceParent
{
	GENERATED_BODY()
	
//Constructor
public:

	APZ_GridInstance();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UFUNCTION()
		void SpawnGridInstance();

private:


//Blueprint variables
public:

	/*
		Number of instances to spawn
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridInstance")
		FIntVector NumInstances = FIntVector(2, 2, 1);
	/*
		Distance between instances
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GridInstance")
		FVector Space = FVector(100, 100, 100);

};
