// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceParent.h"
#include "PZ_ScatterInstance.generated.h"

UENUM(BlueprintType)
enum class EScatterPlacementType : uint8
{
	Box UMETA(DisplayName = "Box"),
	Sphere UMETA(DisolayName = "Sphere")
};

UENUM(BlueprintType)
enum class EScatterBoxPivotPoint : uint8
{
	AtCenter UMETA(DisplayName = "At Center"),
	AtCorner UMETA(DisolayName = "At Corner")
};

/**
 * 
 */
UCLASS(Blueprintable)
class PZ_MESHINSTANCETOOL_API APZ_ScatterInstance : public APZ_MeshInstanceParent
{
	GENERATED_BODY()

public:

	APZ_ScatterInstance();


protected:
	
	virtual void OnConstruction(const FTransform& Transform) override;

//c++ methods
private:

	UFUNCTION()
		void SpawnScatter();

	UFUNCTION()
		void DefinePlacementStyle();

	UFUNCTION()
		FVector GetRandomPointInBox();

	UFUNCTION()
		FVector GetRandomPointInSphere();

//Bluprint values
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScatterInstance")
		int32 NumInstances = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScatterInstance")
		EScatterPlacementType PlacementType = EScatterPlacementType::Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == EScatterPlacementType::Box"), Category = "ScatterInstance")
		FVector BoxSize = FVector(200, 200, 200);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == EScatterPlacementType::Box"), Category = "ScatterInstance")
		EScatterBoxPivotPoint BoxPivotPoint;

	UPROPERTY(BlueprintReadWrite, Category = "ScatterInstance")
		FVector BoxExtent = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == EScatterPlacementType::Sphere"), Category = "ScatterInstance")
		float SphereRadius = 200.0f;

	UPROPERTY(BlueprintReadWrite, Category = "ScatterInstance")
		class UBoxComponent* Box = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "ScatterInstance")
		class USphereComponent* Sphere = nullptr;
};
