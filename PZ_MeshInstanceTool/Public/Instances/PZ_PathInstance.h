// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceParent.h"
#include "PZ_PathInstance.generated.h"

UENUM(BlueprintType)
enum class EPathPlacementType : uint8
{
	Spacing UMETA(DisplayName = "Spacing"),
	SplinePoint UMETA(DisplayName = "SplinePoint")
};
/*

*/
UCLASS(Blueprintable)
class PZ_MESHINSTANCETOOL_API APZ_PathInstance : public APZ_MeshInstanceParent
{
	GENERATED_BODY()

//Constructor
public:

	APZ_PathInstance();

protected:

	virtual void OnConstruction(const FTransform& Transform) override;
	
//c++ methods
private:

	UFUNCTION()
		void SpawnPathBySpacing();

	UFUNCTION()
		void CreatePathInstance(int IndexIn);

	UFUNCTION()
		void SpawnPathBySpline();

	UFUNCTION()
		void SetupFirstMesh();

	UFUNCTION()
		void SetupLastMesh();

//Blueprint values
public: 

	UPROPERTY(BlueprintReadOnly, Category = "PathInstance")
		class USplineComponent* SplineComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathInstance")
		EPathPlacementType PlacementType;

	UPROPERTY(EditAnywhere, Category = "PathInstance")
		bool UseExtraMeshes;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "UseExtraMeshes == true"), Category = "PathInstance")
		UStaticMesh* FirstMesh = nullptr;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "UseExtraMeshes == true"), Category = "PathInstance")
		UStaticMesh* LastMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == Spacing"), Category = "PathInstance")
		float Spacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == Spacing"), Category = "PathInstance")
		int32 MaxInstance = 250;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "PlacementType == Spacing"), Category = "PathInstance")
		bool UsingInstanceLength = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathInstance")
		bool FollowedSplineRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathInstance")
		bool SplineClosed = false;

	UPROPERTY(BlueprintReadWrite, Category = "PathInstance")
		float CurrentDistance;

	UPROPERTY(BlueprintReadWrite, Category = "PathInstance")
		float CurrentInstanceLength;

};
