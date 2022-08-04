// Fill out your copyright notice in the Description page of Project Settings.


#include "PZ_MeshInstanceParent.h"

#include "Components/SceneComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

#include "Engine/StaticMeshActor.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APZ_MeshInstanceParent::APZ_MeshInstanceParent()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Type::Static);
	RootComponent = Root;
}



void APZ_MeshInstanceParent::DeleteAllComponentsWithClass(TSubclassOf<UActorComponent> SceneComponent) const
{
	TArray<UActorComponent*> Components;
	GetComponents(SceneComponent, Components);
	for( auto Component : Components )
	{
		Component->DestroyComponent();
	}
}

void APZ_MeshInstanceParent::SetupInstance(UInstancedStaticMeshComponent* Component, int index)
{
	Component->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	Component->SetupAttachment(RootComponent);
	Component->SetNetAddressable();
	Component->SetStaticMesh(Instances[index].Mesh);
	Component->SetCullDistances(StartCullDistance, EndCullDistance);

	if( Instances[index].InstanceSettingsEnabled )
	{
		Component->SetMobility(Instances[index].InstanceSettings.Mobility);
		Component->SetCastShadow(Instances[index].InstanceSettings.ShadowsCasting);

		if( Instances[index].InstanceSettings.CustomColisionEnabled )
		{
			Component->SetCollisionProfileName(Instances[index].InstanceSettings.CustomCollisionName);
		}
	}
	else
	{
		Component->SetMobility(InstanceSettings.Mobility);
		Component->SetCastShadow(InstanceSettings.ShadowsCasting);

		if( InstanceSettings.CustomColisionEnabled )
		{
			Component->SetCollisionProfileName(InstanceSettings.CustomCollisionName);
		}
	}

	if( Instances[index].IsOverwritingMaterials && Instances[index].OverwriteMaterials.Num() != 0 )
	{
		for( int i = 0; i < Instances[index].OverwriteMaterials.Num(); i++ )
		{
			if( Instances[index].OverwriteMaterials[i] )
			{
				Component->SetMaterial(i, Instances[index].OverwriteMaterials[i]);
			}
		}
	}

	FinishAndRegisterComponent(Component);
}

void APZ_MeshInstanceParent::CheckOverlapping(UStaticMeshComponent* Component)
{
	UPrimitiveComponent* LPrimitiveComponent;

	TArray<AActor*> LIgnoreActors;

	TArray<AActor*> LOutActors;
	TArray<UPrimitiveComponent*> LOutComponents;

	if( InstanceSelfOverlap == EMeshInstanceSelfOverlap::Destroy )
	{
		LPrimitiveComponent = Component;
		if( UKismetSystemLibrary::ComponentOverlapActors(LPrimitiveComponent, LPrimitiveComponent->GetComponentTransform(), ObjectTypes, APZ_MeshInstanceParent::StaticClass(), LIgnoreActors, LOutActors) )
		{
			for( int i = 0; i < LOutActors.Num(); i++ )
			{
				if( LOutActors[i] == this )
				{
					if( UKismetSystemLibrary::ComponentOverlapComponents(
							LPrimitiveComponent, LPrimitiveComponent->GetComponentTransform(), ObjectTypes, UStaticMeshComponent::StaticClass(), LIgnoreActors, LOutComponents) )
					{
						Component->DestroyComponent();
					}
				}
			}
		}
	}
}

void APZ_MeshInstanceParent::SetupStaticMesh(UStaticMeshComponent* Component)
{
	Component->SetStaticMesh(StaticMeshToSpawn);
	Component->SetupAttachment(RootComponent);
	Component->SetNetAddressable();

	if( CurrentInstance.InstanceSettingsEnabled )
	{
		Component->SetMobility(CurrentInstance.InstanceSettings.Mobility);
		Component->SetCastShadow(CurrentInstance.InstanceSettings.ShadowsCasting);

		if( CurrentInstance.InstanceSettings.CustomColisionEnabled )
		{
			Component->SetCollisionProfileName(CurrentInstance.InstanceSettings.CustomCollisionName);
		}
	}
	else
	{
		Component->SetMobility(InstanceSettings.Mobility);
		Component->SetCastShadow(InstanceSettings.ShadowsCasting);

		if( InstanceSettings.CustomColisionEnabled )
		{
			Component->SetCollisionProfileName(InstanceSettings.CustomCollisionName);
		}
	}
}

void APZ_MeshInstanceParent::BakeStaticMeshComponent(UWorld* InWorld, UStaticMeshComponent* Component, FTransform ActorTransform)
{
	FActorSpawnParameters LSpawnParams;
	AStaticMeshActor* LStaticMeshActor = nullptr;

	LStaticMeshActor = InWorld->SpawnActor<AStaticMeshActor>(LStaticMeshActor->StaticClass(), ActorTransform, LSpawnParams);
	LStaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(Component->GetStaticMesh());
	LStaticMeshActor->SetMobility(Component->Mobility);
	LStaticMeshActor->GetStaticMeshComponent()->SetCastShadow(Component->CastShadow);
	if( IsCollisionEnabled )
	{
		LStaticMeshActor->GetStaticMeshComponent()->SetCollisionProfileName(Component->GetCollisionProfileName());
	}
	else
	{
		LStaticMeshActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
	}
}

void APZ_MeshInstanceParent::DestroyComponents()
{
	UStaticMeshComponent* StaticMeshComponent;
	UInstancedStaticMeshComponent* ISMComponent;
	UHierarchicalInstancedStaticMeshComponent* HISMComponent;

	DeleteAllComponentsWithClass(StaticMeshComponent->StaticClass());
	DeleteAllComponentsWithClass(ISMComponent->StaticClass());
	DeleteAllComponentsWithClass(HISMComponent->StaticClass());
}

void APZ_MeshInstanceParent::SetupInstances()
{
	DestroyComponents();
	RandomStream = Seed;
	SetActorEnableCollision(IsCollisionEnabled);

	MeshContainer.Empty();

	for( int i = 0; i < Instances.Num(); i++ )
	{
		switch( InstanceType )
		{
		case EMeshInstanceType::StaticMesh: MeshContainer.Add(Instances[i].Mesh); break;

		case EMeshInstanceType::InstancedStaticMesh:

			UInstancedStaticMeshComponent* LISMComponent;
			LISMComponent = NewObject<UInstancedStaticMeshComponent>(this);

			SetupInstance(LISMComponent, i);

			MeshContainer.Add(LISMComponent);

			break;

		case EMeshInstanceType::HierarchicalInstancedStaticMesh:

			UHierarchicalInstancedStaticMeshComponent* LHISMComponent;
			LHISMComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this);

			SetupInstance(LHISMComponent, i);

			MeshContainer.Add(LHISMComponent);

			break;
		}
	}
}

void APZ_MeshInstanceParent::SetCurrentInstance(int32& RandIndex)
{
	SetInstanceContainer(MeshContainer.Num(), RandIndex);

	if( RandIndex < MeshContainer.Num() )
	{
		switch( InstanceType )
		{
		case EMeshInstanceType::StaticMesh:
			if( UStaticMesh* LStaticMesh = Cast<UStaticMesh>(MeshContainer[RandIndex]) ) StaticMeshToSpawn = LStaticMesh;
			break;

		case EMeshInstanceType::InstancedStaticMesh:
			if( UInstancedStaticMeshComponent* LISM = Cast<UInstancedStaticMeshComponent>(MeshContainer[RandIndex]) ) InstancedStaticMeshToSpawn = LISM;
			break;

		case EMeshInstanceType::HierarchicalInstancedStaticMesh:
			if( UHierarchicalInstancedStaticMeshComponent* LHISM = Cast<UHierarchicalInstancedStaticMeshComponent>(MeshContainer[RandIndex]) ) HISMToSpawn = LHISM;
			break;
		}
	}
}



void APZ_MeshInstanceParent::CreateInstance(FTransform Transform)
{
	switch( InstanceType )
	{
	case EMeshInstanceType::StaticMesh:

		UStaticMeshComponent* LSMComponent;
		LSMComponent = NewObject<UStaticMeshComponent>(this);

		SetupStaticMesh(LSMComponent);

		LSMComponent->SetRelativeTransform(Transform);

		if( CurrentInstance.IsOverwritingMaterials )
		{
			if( CurrentInstance.OverwriteMaterials.Num() != 0 )
			{
				for( int j = 0; j < CurrentInstance.OverwriteMaterials.Num(); j++ )
				{
					if( CurrentInstance.OverwriteMaterials[j] )
					{
						LSMComponent->SetMaterial(j, CurrentInstance.OverwriteMaterials[j]);
					}
				}
			}
		}

		FinishAndRegisterComponent(LSMComponent);

		CheckOverlapping(LSMComponent);

		break;

	case EMeshInstanceType::InstancedStaticMesh:

		if( IsValid(InstancedStaticMeshToSpawn) )
		{
			InstancedStaticMeshToSpawn->AddInstance(Transform);
		}

		break;

	case EMeshInstanceType::HierarchicalInstancedStaticMesh:

		if( IsValid(HISMToSpawn) )
		{
			HISMToSpawn->AddInstance(Transform);
		}

		break;
	}
}

void APZ_MeshInstanceParent::Update()
{
	RerunConstructionScripts();
}

void APZ_MeshInstanceParent::RandomizeSeed()
{
	RandomStream.GenerateNewSeed();
	Seed = RandomStream.GetCurrentSeed();
	Update();
}

void APZ_MeshInstanceParent::BakeInstances()
{
#if WITH_EDITOR

	UWorld* LWorld = GetWorld();

	if( !IsValid(LWorld) ) return;

	if( !LWorld->HasBegunPlay() )
	{
		TArray<USceneComponent*> LChildComponents;
		RootComponent->GetChildrenComponents(false, LChildComponents);

		FTransform LActorTransform;
		FActorSpawnParameters LSpawnParams;

		switch( InstanceType )
		{
		case EMeshInstanceType::StaticMesh:

			for( USceneComponent* Component : LChildComponents )
			{
				if( UStaticMeshComponent* LStaticMeshComponent = Cast<UStaticMeshComponent>(Component) )
				{
					LActorTransform = Component->GetComponentTransform();
					BakeStaticMeshComponent(LWorld, LStaticMeshComponent, LActorTransform);
				}
			}
			break;

		case EMeshInstanceType::InstancedStaticMesh:

			for( USceneComponent* Component : LChildComponents )
			{
				if( UInstancedStaticMeshComponent* LInstancedStaticMeshComponent = Cast<UInstancedStaticMeshComponent>(Component) )
				{
					for( int i = 0; i < LInstancedStaticMeshComponent->GetInstanceCount(); i++ )
					{
						LInstancedStaticMeshComponent->GetInstanceTransform(i, LActorTransform, true);
						BakeStaticMeshComponent(LWorld, LInstancedStaticMeshComponent, LActorTransform);
					}
				}
			}
			break;

		case EMeshInstanceType::HierarchicalInstancedStaticMesh:

			for( USceneComponent* Component : LChildComponents )
			{
				if( UHierarchicalInstancedStaticMeshComponent* LHierarchicalInstancedStaticMeshComponent = Cast<UHierarchicalInstancedStaticMeshComponent>(Component) )
				{
					for( int i = 0; i < LHierarchicalInstancedStaticMeshComponent->GetInstanceCount(); i++ )
					{
						LHierarchicalInstancedStaticMeshComponent->GetInstanceTransform(i, LActorTransform, true);
						BakeStaticMeshComponent(LWorld, LHierarchicalInstancedStaticMeshComponent, LActorTransform);
					}
				}
			}
			break;
		}
	}
#endif // WITH_EDITOR
}

void APZ_MeshInstanceParent::SetInstanceContainer(int ContainerNum, int32& RandomIndexOut)
{
	if( ContainerNum == 0 ) return;

	switch( SpawnType )
	{
	case EMeshInstanceSpawnType::Random: RandomIndexOut = RandomStream.RandRange(0, ContainerNum - 1); break;
	case EMeshInstanceSpawnType::InOrder: RandomIndexOut = (++ RandomIndexOut) % ContainerNum; break;
	}
	CurrentInstance = Instances[RandomIndexOut];
}
