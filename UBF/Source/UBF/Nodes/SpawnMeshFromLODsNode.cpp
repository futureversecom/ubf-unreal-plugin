#include "SpawnMeshFromLODsNode.h"

#include "glTFRuntimeFunctionLibrary.h"
#include "UBFMeshConfigSettings.h"
#include "DataTypes/MeshConfig.h"
#include "GlobalArtifactProvider/CachedMesh.h"

namespace UBF
{
	void FSpawnMeshFromLODsNode::ExecuteAsync() const
	{
		UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] Executing Node"));
		
		TArray<FString> MeshResources;
		if (!TryReadInputValueArray("Meshes", MeshResources))
		{
			UBF_LOG(Warning, TEXT("[SpawnMeshFromLODs] failed to read Array input 'Meshes'"));
			HandleFailureFinish();
			return;
		}
		
		FMeshConfig* MeshConfig = nullptr;
		FMeshConfigData MeshConfigData;
		if (!TryReadInput("Config", MeshConfig))
		{
			UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] no MeshConfig provided, using default mesh config"));
			MeshConfigData = GetDefault<UUBFMeshConfigSettings>()->GetMeshConfigData();
		}
		else
		{
			UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] found meshconfig override"));
			MeshConfigData = MeshConfig->MeshConfigData;
		}
		
		FSceneNode* ParentInput = nullptr;
		if (!TryReadInput("Parent", ParentInput))
		{
			UBF_LOG(Warning, TEXT("[SpawnMeshFromLODs] Failed to Read 'Parent' Input"));
		
			// todo: disabling this temporarily for the demo
			//HandleFailureFinish();
			//return;
			ParentInput = GetRoot();
		}

		TArray<FglTFRuntimeMeshLOD> RuntimeMeshLODs;
		
		for (int i = 0; i < MeshResources.Num(); ++i)
		{
			const FString MeshResourceID = MeshResources[i];
			FglTFRuntimeMeshLOD MeshLOD = RuntimeMeshLODs[i];
			FString ResourceID;
			FString MeshName;
			const bool bIsCombinedResourceID = MeshResourceID.Contains(TEXT("/"));
			
			// Id can either be in <id> or <id>/<mesh-name> formats
			if (bIsCombinedResourceID)
			{
				MeshResourceID.Split(TEXT("/"), &ResourceID, &MeshName);
				UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] Resource ID Contains a MeshName %s"), *MeshName);
			}
			else
			{
				ResourceID = MeshResourceID;
			}

			GetContext().GetSetConfig()->GetMesh(ResourceID, FMeshImportSettings(MeshConfigData.RuntimeConfig)).Next(
				[this, ResourceID, MeshName, &MeshLOD, MeshConfigData, ParentInput](const FLoadMeshResult Result)
			{
				if (!Result.Result.Key)
				{
					UBF_LOG(Error, TEXT("[SpawnMeshFromLODs] Failed to load mesh %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}
		
				if (!CheckExecutionStillValid())
				{
					HandleFailureFinish();
					return;
				}
					
				UglTFRuntimeAsset* Asset = Result.Result.Value;
			
				if (!Asset)
				{
					UBF_LOG(Error, TEXT("[SpawnMeshFromLODs] MeshAsset invalid %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}
				
				UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] ParentComp: %s"), *ParentInput->ToString());
				int32 MeshIndex = 0;
				
				for (auto Node : Asset->GetNodes())
				{
					if (Node.Name.Equals(MeshName))
					{
						MeshIndex = Node.MeshIndex;
					}
				}
				FglTFRuntimeMaterialsConfig MaterialsConfig;
				bool bSuccess = Asset->LoadMeshAsRuntimeLOD(MeshIndex, MeshLOD, MaterialsConfig);

				UglTFRuntimeAsset* LODObjectMesh = NewObject<UglTFRuntimeAsset>();

				LODObjectMesh->LoadSkeletalMeshFromRuntimeLODs()
					
				// do next one and report whether it was successful or not
			
				// AglTFRuntimeAssetActor* SpawnedActor;
				//
				// {
				// 	TRACE_CPUPROFILER_EVENT_SCOPE(FSpawnMeshNode::ExecuteAsync_SpawnGLTFActor);
				// 	
				// 	FActorSpawnParameters SpawnParameters;
				// 	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				// 			
				// 	SpawnedActor = GetWorld()->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), FTransform::Identity);
				// 	SpawnedActor->Asset = Asset;
				// 	SpawnedActor->SkeletalMeshConfig = MeshConfigData.SkeletalMeshConfig;
				// 				
				// 	if (!SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.IsBound())
				// 	{
				// 		SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.BindDynamic(NewObject<UBoneRemapperUtil>(), &UBoneRemapperUtil::RemapFormatBoneName);
				// 	}
				// 				
				// 	SpawnedActor->bAllowNodeAnimations = MeshConfigData.bLoadAnimation;
				// 	SpawnedActor->bAllowPoseAnimations = MeshConfigData.bLoadAnimation;
				// 	SpawnedActor->bAllowSkeletalAnimations = MeshConfigData.bLoadAnimation;
				// 	SpawnedActor->bAutoPlayAnimations = MeshConfigData.bLoadAnimation;
				// 	SpawnedActor->FinishSpawning(FTransform::Identity);
				//
				// 	if (!GetRoot()->GetAttachmentComponent()->IsVisible())
				// 	{
				// 		SpawnedActor->SetActorHiddenInGame(true);
				// 	}
				//
				// 	SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.Clear();
				// 	SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Context = nullptr;
				// }
				//
				// {
				// 	TRACE_CPUPROFILER_EVENT_SCOPE(FSpawnMeshNode::ExecuteAsync_SetupActor);
				// 	
				// 	// assuming that if the parent is a child transform, it's a bone transform
				// 	SpawnedActor->AttachToComponent(ParentInput->GetAttachmentComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, ParentInput->GetAttachmentSocket());
				// 			
				// 	TArray<UMeshComponent*> MeshComponents;
				// 	SpawnedActor->GetComponents(MeshComponents);
				//
				// 	// temp fix: assume first mesh component is the leader mesh component
				// 	auto HasMultipleMeshes = MeshComponents.Num() > 1;
				// 	USkeletalMeshComponent* LeaderSkeletalMeshComponent = HasMultipleMeshes
				// 		? Cast<USkeletalMeshComponent>(MeshComponents[0])
				// 		: nullptr;
				// 	
				// 	FDynamicHandle MeshArray = FDynamicHandle::Array();
				// 			
				// 	// need to decide whether it is okay to pass mesh renderers as scene nodes
				// 	FDynamicHandle SceneNodeArray = FDynamicHandle::Array();
				// 	for (auto MeshComponent : MeshComponents)
				// 	{
				// 		MeshArray.Push(FDynamicHandle::ForeignHandled(new FMeshRenderer(MeshComponent)));
				// 		SceneNodeArray.Push(FDynamicHandle::ForeignHandled(new FSceneNode(MeshComponent)));
				// 	}
				//
				// 	// temp fix
				// 	if(HasMultipleMeshes)
				// 	{
				// 		// skip first one as setting itself as the leader will cause infinite loop
				// 		for (int i = 1; i < MeshComponents.Num(); ++i)
				// 		{
				// 			const auto SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponents[i]);
				// 			if (SkeletalMeshComponent && LeaderSkeletalMeshComponent)
				// 			{
				// 				SkeletalMeshComponent->SetLeaderPoseComponent(LeaderSkeletalMeshComponent);
				// 			}
				// 		}
				// 	}
				// 	UBF_LOG(Verbose, TEXT("[SpawnMeshFromLODs] Outputting %d Renderers"), MeshComponents.Num());
				// 	WriteOutput("Renderers", MeshArray);
				// 	WriteOutput("Scene Nodes", SceneNodeArray);
				// }
				
				TriggerNext();
				CompleteAsyncExecution();
			});
		}
	}

	void FSpawnMeshFromLODsNode::HandleFailureFinish() const
	{
		WriteOutput("Renderers", FDynamicHandle::Array());
		
		FDynamicHandle SceneNodeArray = FDynamicHandle::Array();
		SceneNodeArray.Push(FDynamicHandle::ForeignHandled(new FSceneNode(GetRoot())));
		WriteOutput("Scene Nodes", SceneNodeArray);
		
		TriggerNext();
		CompleteAsyncExecution();
	}

}
