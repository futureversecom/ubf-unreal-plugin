// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "GLTFRuntimeUtils/SpawnGLTFMeshLibrary.h"

#include "glTFRuntimeAssetActor.h"
#include "UBFLog.h"
#include "Util/BoneRemapperUtil.h"

AActor* USpawnGLTFMeshLibrary::SpawnMesh(UObject* WorldContext, UglTFRuntimeAsset* Asset,
                                         const FMeshConfigData& MeshConfig)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpawnGLTFMeshLibrary::SpawnMesh);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;
				
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig = MeshConfig.SkeletalMeshConfig;
	
	FRichCurve LODCurve = *MeshConfig.LODCurve.GetRichCurveConst();
	if (MeshConfig.LODCurve.ExternalCurve != nullptr)
	{
		LODCurve = MeshConfig.LODCurve.ExternalCurve.Get()->FloatCurve;
	}
	
	TMap<int32, float> LODScreenSize;
	for (auto CurveKey : LODCurve.Keys)
	{
		LODScreenSize.Add(CurveKey.Time, CurveKey.Value);
	}
	SkeletalMeshConfig.LODScreenSize = LODScreenSize;
	
	AglTFRuntimeAssetActor* SpawnedActor = World->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), FTransform::Identity);
	SpawnedActor->Asset = Asset;
	SpawnedActor->SkeletalMeshConfig = SkeletalMeshConfig;
							
	if (!SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.IsBound())
	{
		SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.BindDynamic(NewObject<UBoneRemapperUtil>(), &UBoneRemapperUtil::RemapFormatBoneName);
	}
							
	SpawnedActor->bAllowNodeAnimations = MeshConfig.bLoadAnimation;
	SpawnedActor->bAllowPoseAnimations = MeshConfig.bLoadAnimation;
	SpawnedActor->bAllowSkeletalAnimations = MeshConfig.bLoadAnimation;
	SpawnedActor->bAutoPlayAnimations = MeshConfig.bLoadAnimation;
	SpawnedActor->FinishSpawning(FTransform::Identity);
	
	SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.Clear();
	SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Context = nullptr;

	return SpawnedActor;
}

UStreamableRenderAsset* USpawnGLTFMeshLibrary::LoadMeshLOD(const FglTFRuntimeLODData& LODData, const FMeshConfigData& MeshConfig)
{
	bool bIsSkeletal = true;
	for (const auto& LOD : LODData.LODs)
	{
		for (auto& Primitive : LOD.Primitives)
		{
			if (Primitive.Joints.IsEmpty())
			{
				bIsSkeletal = false;
				break;
			}	
		}
	}
	
	UE_LOG(LogUBF, Verbose, TEXT("[LoadMeshLOD] Loading SkeltalMesh: %s "), bIsSkeletal? TEXT("true") : TEXT("false"));
	if (bIsSkeletal)
	{
		FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig = MeshConfig.SkeletalMeshConfig;
		if (!SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.IsBound())
		{
			SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.BindDynamic(NewObject<UBoneRemapperUtil>(), &UBoneRemapperUtil::RemapFormatBoneName);
		}
		
		FRichCurve LODCurve = *MeshConfig.LODCurve.GetRichCurveConst();
		if (MeshConfig.LODCurve.ExternalCurve != nullptr)
		{
			LODCurve = MeshConfig.LODCurve.ExternalCurve.Get()->FloatCurve;
		}
		
		TMap<int32, float> LODScreenSize;
		for (auto CurveKey : LODCurve.Keys)
		{
			LODScreenSize.Add(CurveKey.Time, CurveKey.Value);
		}
		SkeletalMeshConfig.LODScreenSize = LODScreenSize;
		
		USkeletalMesh* Mesh = LODData.ParsingAsset->LoadSkeletalMeshFromRuntimeLODs(LODData.LODs, 0, SkeletalMeshConfig);

		SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.Clear();
		SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Context = nullptr;
		return Mesh;
	}
	else
	{
		UStaticMesh* Mesh = LODData.ParsingAsset->LoadStaticMeshFromRuntimeLODs(LODData.LODs, FglTFRuntimeStaticMeshConfig());
		return Mesh;
	}
}

bool USpawnGLTFMeshLibrary::LoadAssetAsLOD(UglTFRuntimeAsset* Asset, const FString& MeshName, FglTFRuntimeMeshLOD& OutLOD)
{
	int32 MeshIndex = 0;
				
	for (auto Node : Asset->GetNodes())
	{
		if (Node.Name.Equals(MeshName))
		{
			MeshIndex = Node.MeshIndex;
		}
	}
	FglTFRuntimeMaterialsConfig MaterialsConfig;
	return  Asset->LoadMeshAsRuntimeLOD(MeshIndex, OutLOD, MaterialsConfig);
}

AActor* USpawnGLTFMeshLibrary::SpawnLODMesh(UObject* WorldContext,  UStreamableRenderAsset* LODMesh)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(USpawnGLTFMeshLibrary::SpawnLODMesh);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	// Spawn Actor
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = World->SpawnActor<AActor>(SpawnParameters);
	// Add root node
	Actor->AddComponentByClass(USceneComponent::StaticClass(), false, FTransform(), false);
	
	// Determine if LODs is staticmesh or skeletal mesh
	if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(LODMesh))
	{
		USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Actor->AddComponentByClass(USkeletalMeshComponent::StaticClass(), false, FTransform(), false));
		
		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
		Actor->SetRootComponent(SkeletalMeshComponent);
	}
	else if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(LODMesh))
	{
		UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Actor->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, FTransform(), false));
		
		StaticMeshComponent->SetStaticMesh(StaticMesh);
		Actor->SetRootComponent(StaticMeshComponent);
	}

	return Actor;
}
