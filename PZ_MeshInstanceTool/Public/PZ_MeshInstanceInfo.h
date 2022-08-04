// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PZ_MeshInstanceInfo.generated.h"

UENUM(BlueprintType)
enum class EMeshInstanceType : uint8
{
	StaticMesh UMETA(DisplayName = "Static Mesh"),
	InstancedStaticMesh UMETA(DisplayName = "Instanced Static Mesh"),
	HierarchicalInstancedStaticMesh UMETA(DisplayName = "Hierarchical Instanced Static Mesh")
};

UENUM(BlueprintType)
enum class EMeshInstanceRotationAxes : uint8
{
	XYZ UMETA(DisplayName = "XYZ"),
	XY UMETA(DisplayName = "XY"),
	Z UMETA(DisplayName = "Z")
};

UENUM(BlueprintType)
enum class EMeshInstanceSelfOverlap : uint8
{
	Ignore UMETA(DisplayName = "Ignore"),
	Destroy UMETA(DisplayName = "Destroy")
};

UENUM(BlueprintType)
enum class EMeshInstanceSpawnType : uint8
{
	Random UMETA(DisplayName = "Random"),
	InOrder UMETA(DisplayName = "InOrder")
};

USTRUCT(BlueprintType)
struct FMeshInstanceLocation
{
	GENERATED_BODY()

	/* 
		Location offset of a initial instance location.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeStructureDefaultValue = "0, 0, 0"), Category = "MeshInstanceTool")
		FVector LocationOffset = FVector(0, 0, 0);
	/*
		Min random location in range. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeStructureDefaultValue = "0, 0, 0"), Category = "MeshInstanceTool")
		FVector LocationRangeMin = FVector(0, 0, 0);
	/*
		Max random location in range. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeStructureDefaultValue = "0, 0, 0"), Category = "MeshInstanceTool")
		FVector LocationRangeMax = FVector(0, 0, 0);
};

USTRUCT(BlueprintType)
struct FMeshInstanceRotation
{
	GENERATED_BODY()

	/*
		Rotation offset of a initial instance rotation.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FRotator RotationOffset = FRotator(0, 0, 0);
	/*
		Min random rotation in range.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FRotator RotationRangeMin = FRotator(0, 0, 0);
	/*
		Max random rotation in range.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FRotator RotationRangeMax = FRotator(0, 0, 0);
	/*
		Rotate instance towards an user-defined Actor.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		class AActor* RotateToActor = nullptr;
	/*
		An axes by which instance will be rotated.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		EMeshInstanceRotationAxes RotationAxes = EMeshInstanceRotationAxes::XYZ;
};

USTRUCT(BlueprintType)
struct FMeshInstanceScale
{
	GENERATED_BODY()

	/*
		Enable Uniform Scale.	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		bool IsUniformScaling = true;
	/*
		Min random uniform scale.	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		float MinUniformScale = 1.0f;
	/*
		Max random uniform scale.	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		float MaxUniformScale = 1.0f;
	/*
		Min random scale.	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FVector MinScale = FVector(1, 1, 1);
	/*
		Max random scale.	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FVector MaxScale = FVector(1, 1, 1);
};

USTRUCT(BlueprintType)
struct FMeshInstanceSettings
{
	GENERATED_BODY()

	/*
		Location randomization settings. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FMeshInstanceLocation Location;
	/*
		Rotation randomization settings	
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FMeshInstanceRotation Rotation;
	/*
		Scale randomization
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FMeshInstanceScale Scale;
	/*
		Enable Custom Collision Profile.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		bool CustomColisionEnabled = false;
	/*
		Whether to cast shadows. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		bool ShadowsCasting = true;
	/*
		Custom Collision Profile Name
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool")
		FName CustomCollisionName;
	/*
		Chance to spawn this instance, fron 0 to 100
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"), Category = "MeshInstanceTool")
		int32 SpawnChance;
	/*
		Arbitrary instance physical length which can be used by ProInstance Types with splines. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = "MeshInstanceTool")
		float InstanceLength = 100.0f;
	/*
		Set the custom mobility for this instance(s). 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MeshInstanceTool")
		TEnumAsByte<EComponentMobility::Type> Mobility = EComponentMobility::Type::Static;
};

USTRUCT(BlueprintType)
struct FMeshInstance
{
	GENERATED_BODY()

	/*
		User-defined Static Mesh
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstance")
		UStaticMesh* Mesh = nullptr;
	/*
		Enables Overitting Materials
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle), Category = "MeshInstance")
		bool IsOverwritingMaterials = false;
	/*
		Can overwrite Static Mesh materials if enabled.
		Each array element represents material slot of selected Static Mesh.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsOverwritingMaterials"), Category = "MeshInstance")
		TArray<UMaterialInterface*> OverwriteMaterials;
	/*
		Enables Local Instance Settings
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle), Category = "MeshInstanceMainSettings | Instances")
		bool InstanceSettingsEnabled = true;
	/*
		Local instance transform and randomization settings for current instance.
		Can overwrite Instance Global Settings if enabled.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "InstanceSettingsEnabled"), Category = "MeshInstance")
		FMeshInstanceSettings InstanceSettings;
};