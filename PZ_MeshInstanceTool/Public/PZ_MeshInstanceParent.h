// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "PZ_MeshInstanceInfo.h"
#include "PZ_MeshInstanceParent.generated.h"


UCLASS(Blueprintable, Abstract)
class PZ_MESHINSTANCETOOL_API APZ_MeshInstanceParent : public AActor
{
	GENERATED_BODY()
	
//Constructor
public:	

	// Sets default values for this actor's properties
	APZ_MeshInstanceParent();

//c++ private methods
private: 
	
	void DeleteAllComponentsWithClass(TSubclassOf<UActorComponent> SceneComponent) const;

	void SetupInstance(UInstancedStaticMeshComponent* Component, int index);

	void CheckOverlapping(UStaticMeshComponent* Component);

	void SetupStaticMesh(UStaticMeshComponent* Component);

	void BakeStaticMeshComponent(UWorld* InWorld, UStaticMeshComponent* Component, FTransform ActorTransform);

//Blueprint private methods
protected:


	UFUNCTION(BlueprintCallable, Category = "MeshInstanceTool|Functions")
		void DestroyComponents();

	UFUNCTION(BlueprintCallable, Category = "MeshInstanceTool|Functions")
		void SetupInstances();

	UFUNCTION(BlueprintCallable, Category = "MeshInstanceTool|Functions")
		void SetCurrentInstance(int32& RandIndex);



	UFUNCTION(BlueprintCallable, Category = "MeshInstanceTool|Functions")
		void CreateInstance(FTransform Transform);
	/* 
		Re - run the Construction Script
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MeshInstanceTool")
		void Update();
	/* 
		Set random Seed value and re - run the Construction Script
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MeshInstanceTool")
		void RandomizeSeed();
	/* 
		Bakes procedually placed Components to Actors
	*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MeshInstanceTool")
		void BakeInstances();

	UFUNCTION(BlueprintCallable, Category = "MeshInstanceContainer|Functions")
		void SetInstanceContainer(int ContainerNum, int32& RandomIndexOut);

	
//.......................................... Chidren methods ......................................

	FORCEINLINE FMeshInstanceSettings GetInstanceSettings()
	{
		if( CurrentInstance.InstanceSettingsEnabled )
		{
			return CurrentInstance.InstanceSettings;
		}
		else
		{
			return InstanceSettings;
		}
	};

	FORCEINLINE FVector GetRandomLocation(FVector LocationIn)
	{
		FVector LMin = GetInstanceSettings().Location.LocationRangeMin;
		FVector LMax = GetInstanceSettings().Location.LocationRangeMax;

		FVector LLocationOut;

		LLocationOut.X = RandomStream.FRandRange(LMin.X, LMax.X);
		LLocationOut.Y = RandomStream.FRandRange(LMin.Y, LMax.Y);
		LLocationOut.Z = RandomStream.FRandRange(LMin.Z, LMax.Z);

		LLocationOut += LocationIn;

		return LLocationOut;
	};

	FORCEINLINE FRotator GetRandomRotation(FRotator RotationIn)
	{
		FRotator LMin = GetInstanceSettings().Rotation.RotationRangeMin;
		FRotator LMax = GetInstanceSettings().Rotation.RotationRangeMax;

		FRotator LRotationOut;

		LRotationOut.Roll = RandomStream.FRandRange(LMin.Roll, LMax.Roll);
		LRotationOut.Pitch = RandomStream.FRandRange(LMin.Pitch, LMax.Pitch);
		LRotationOut.Yaw = RandomStream.FRandRange(LMin.Yaw, LMax.Yaw);

		LRotationOut = FRotator(FQuat(LRotationOut) * FQuat(RotationIn));

		return LRotationOut;
	};

	FORCEINLINE FVector GetRandomScale()
	{
		FVector LScaleOut;

		if( GetInstanceSettings().Scale.IsUniformScaling )
		{
			float LMin = GetInstanceSettings().Scale.MinUniformScale;
			float LMax = GetInstanceSettings().Scale.MaxUniformScale;

			float LRand = RandomStream.FRandRange(LMin, LMax);

			LScaleOut = FVector(LRand, LRand, LRand);
		}
		else
		{
			FVector LMin = GetInstanceSettings().Scale.MinScale;
			FVector LMax = GetInstanceSettings().Scale.MaxScale;

			LScaleOut.X = RandomStream.FRandRange(LMin.X, LMax.X);
			LScaleOut.Y = RandomStream.FRandRange(LMin.Y, LMax.Y);
			LScaleOut.Z = RandomStream.FRandRange(LMin.Z, LMax.Z);
		}

		return LScaleOut;
	};

	FORCEINLINE FVector GetRandomLocationScaled(FVector LocationIn, FVector ScaleIn)
	{
		FVector LLocationOut;

		LLocationOut.X = RandomStream.FRandRange(-ScaleIn.X, ScaleIn.X);
		LLocationOut.Y = RandomStream.FRandRange(-ScaleIn.Y, ScaleIn.Y);
		LLocationOut.Z = RandomStream.FRandRange(-ScaleIn.Z, ScaleIn.Z);

		return LLocationOut + LocationIn;
	};

	FORCEINLINE FVector GetLocationXYOffset(FVector LocationIn) { return FVector(GetInstanceSettings().Location.LocationOffset.X, GetInstanceSettings().Location.LocationOffset.Y, 0) + LocationIn; };

	FORCEINLINE FVector GetLocationZOffset(FVector LocationIn) { return FVector(0, 0, GetInstanceSettings().Location.LocationOffset.Z) + LocationIn; };

	FORCEINLINE FRotator GetRotationOffset(FRotator RotationIn) { return FRotator(FQuat(GetInstanceSettings().Rotation.RotationOffset) * FQuat(RotationIn)); };

	FORCEINLINE bool GetRandomChance() { return GetInstanceSettings().SpawnChance >= RandomStream.RandRange(0, 100); };

	FORCEINLINE FRotator GetLookAtActor(FVector LocationIn, FRotator RotationIn)
	{
		FMeshInstanceSettings LSettings = GetInstanceSettings();

		if( IsValid(LSettings.Rotation.RotateToActor) )
		{
			FVector LActorLocation = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), LSettings.Rotation.RotateToActor->GetActorLocation());
			FRotator LRotationToActor = UKismetMathLibrary::FindLookAtRotation(LocationIn, LActorLocation);

			switch( LSettings.Rotation.RotationAxes )
			{
			case EMeshInstanceRotationAxes::XYZ: return FRotator(FQuat(FRotator(LRotationToActor.Roll, LRotationToActor.Pitch, LRotationToActor.Yaw)) * FQuat(RotationIn)); break;

			case EMeshInstanceRotationAxes::XY: return FRotator(FQuat(FRotator(LRotationToActor.Roll, LRotationToActor.Pitch, 0)) * FQuat(RotationIn)); break;

			case EMeshInstanceRotationAxes::Z: return FRotator(FQuat(FRotator(0, 0, LRotationToActor.Yaw)) * FQuat(RotationIn)); break;
			}
		}
		return RotationIn;
	};

	FORCEINLINE void PlacementLineTrace(FVector StartTracing, float TracingDepth, FVector& LocationOut, FRotator& RotationOut, bool& CanSpawn)
	{

		FVector LTargetLocation = UKismetMathLibrary::GetUpVector(GetActorRotation()) + StartTracing + GetActorLocation();

		FHitResult LHitActor;

		EDrawDebugTrace::Type LTrace = EDrawDebugTrace::Type::None;

		if( DrawDebugLine )
		{
			LTrace = EDrawDebugTrace::Type::ForDuration;
		}

		bool LReturnValue = UKismetSystemLibrary::LineTraceSingleForObjects(this, FVector(LTargetLocation.X, LTargetLocation.Y, LTargetLocation.Z + TracingDepth),
			FVector(LTargetLocation.X, LTargetLocation.Y, LTargetLocation.Z - TracingDepth), ObjectTypes, false, ActorsWithCollisionIgnore, LTrace, LHitActor, IsSelfCollisionIgnored,
			FLinearColor::Red, FLinearColor::Green,DrawTime);

		if( IsSurfaceSnapping )
		{
			CanSpawn = LReturnValue;

			LocationOut = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), LHitActor.ImpactPoint);

			if( UseMeshAdjustment )
			{
				FBoxSphereBounds LBoxLocation = 0;

				switch( InstanceType )
				{
				case EMeshInstanceType::StaticMesh: LBoxLocation = StaticMeshToSpawn->GetBounds(); break;
				case EMeshInstanceType::InstancedStaticMesh: LBoxLocation = InstancedStaticMeshToSpawn->Bounds; break;
				case EMeshInstanceType::HierarchicalInstancedStaticMesh: LBoxLocation = HISMToSpawn->Bounds; break;
				}

				LocationOut.Z += LBoxLocation.GetBox().GetSize().Z;
			}
		}
		else
		{
			CanSpawn = true;

			LocationOut = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), FVector(LTargetLocation.X, LTargetLocation.Y, LTargetLocation.Z - 1));
		}

		LocationOut += SizeOffset;

		if( IsSurfaceAligning )
		{
			RotationOut =
				UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), UKismetMathLibrary::MakeRotFromZX(LHitActor.Normal, UKismetMathLibrary::GetForwardVector(GetActorRotation())));
		}
		else
		{
			RotationOut = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), GetActorRotation());
		}
	};

//Blueprint values
public:


	//......................... Containers ........................

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceMainSettings")
		FMeshInstance CurrentInstance;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceMainSettings")
		TArray<UObject*> MeshContainer;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceTool")
		UStaticMesh* StaticMeshToSpawn = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceTool")
		UInstancedStaticMeshComponent* InstancedStaticMeshToSpawn = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceTool")
		UHierarchicalInstancedStaticMeshComponent* HISMToSpawn = nullptr;


	//......................... Instances ........................
	/*
		Sets a component type for instances.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		EMeshInstanceSpawnType SpawnType = EMeshInstanceSpawnType::Random;
	/*
		Sets a component type for instances.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		EMeshInstanceType InstanceType = EMeshInstanceType::StaticMesh;
	/*
		User-defined array of instances.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		TArray<FMeshInstance> Instances;
	/*
		Global Instance transform and randomization settings
		Can be overwritten by Instance Local Settings
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		FMeshInstanceSettings InstanceSettings;
	/*
		Defines what to do when spawned instance is overlapping neighbor instance.
		Currently working only with Static Mesh instance type.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "InstanceType == EMeshInstanceType::StaticMesh"), Category = "MeshInstanceMainSettings|Instances")
		EMeshInstanceSelfOverlap InstanceSelfOverlap = EMeshInstanceSelfOverlap::Ignore;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		int32 Index;

	UPROPERTY(BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		FIntVector IndexVector;
	/*
		Whether to have an actor collision. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInsctanceMainSettings|Instances")
		bool IsCollisionEnabled = true;

	//.........................Optimization and Randomization ........................
	
	/*
		Start Cull Distance.
		Works with Instanced Static Mesh or Hierarchical Instance Static Mesh
	*/
	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "InstanceType == EMeshInstanceType::InstanceStaticMesh || InstanceType == EMeshInstanceType::HISM")
		,Category = "MeshInstanceMainSettings|Optimization")
		int32 StartCullDistance;
	/*
		End Cull Distance.
		Works with Instanced Static Mesh or Hierarchical Instance Static Mesh
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (EditCondition = "InstanceType == EMeshInstanceType::InstancedStaticMesh || InstanceType == EMeshInstanceType::HierarchicalInstancedStaticMesh"),
		Category = "MeshInstanceMainSettings|Optimization")
		int32 EndCullDistance;
	/*
		Randomization Seed
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Randomization")
		int32 Seed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "MeshInstanceMainSettings|Randomization")
		FRandomStream RandomStream;

	//.........................Snapping And Aligning ........................


	/*
		Distance to move Mesh
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceMainSettings|Instances")
		FVector SizeOffset = FVector(0, 0, 0);
	/*
		Whether to draw debug line traces. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool|SnappingAndAligning")
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	/*
		Snaps instance's location to a surface below. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool|SnappingAndAligning")
		bool IsSurfaceSnapping = false;
	/*
		Aligns instance's rotation to a surface angle below. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsSurfaceSnapping"), Category = "MeshInstanceTool|SnappingAndAligning")
		bool IsSurfaceAligning = false; 
	/*
		Moves mesh up on a half of it's own boarders
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsSurfaceSnapping"), Category = "MeshInstanceTool|SnappingAndAligning")
		bool UseMeshAdjustment = false;
	/*
		Aligns instance's rotation to a surface angle below. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsSurfaceSnapping"), Category = "MeshInstanceTool|SnappingAndAligning")
		bool IsSelfCollisionIgnored = false;
	/*
		Distance of line traces when casting them for Surface Snapping. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsSurfaceSnapping"), Category = "MeshInstanceTool|SnappingAndAligning")
		float LineTracingDistance = 500.0f;
	/*
		Ignore collision of user-defined actors when casting line traces for Surface Snapping.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsSurfaceSnapping"), Category = "MeshInstanceTool|SnappingAndAligning")
		TArray<AActor*> ActorsWithCollisionIgnore;


		//......................... Line Trace Debug ........................

	/*
		Whether to draw debug line traces. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshInstanceTool|Debug")
		bool DrawDebugLine = false;
	/* 
		Time to draw debug line traces. 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = "MeshInstanceTool|Debug")
		float DrawTime = 1.0f;
};
