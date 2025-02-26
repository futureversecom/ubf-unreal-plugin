#include "SpawnMeshNode.h"

#include "glTFRuntimeAssetActor.h"
#include "GraphProvider.h"
#include "UBFLog.h"
#include "UBFMeshConfigSettings.h"
#include "DataTypes/MeshConfig.h"
#include "DataTypes/MeshRenderer.h"
#include "DataTypes/SceneNode.h"
#include "Util/BoneRemapperUtil.h"

namespace UBF
{
	void FSpawnMeshNode::ExecuteAsync() const
	{
		UBF_LOG(Verbose, TEXT("[SpawnMesh] Executing Node"));
		
		FString ResourceID;
		if (!TryReadInputValue("Resource", ResourceID))
		{
			UBF_LOG(Warning, TEXT("[SpawnMesh] failed ResourceInfo for ID %s"), *ResourceID);
			HandleFailureFinish();
			return;
		}
		
		UBF_LOG(Verbose, TEXT("[SpawnMesh] found Resource with ID %s"), *ResourceID);

		FSceneNode* ParentInput = nullptr;
		if (!TryReadInput("Parent", ParentInput))
		{
			UBF_LOG(Warning, TEXT("[SpawnMesh] Failed to Read 'Parent' Input"));

			// todo: disabling this temporarily for the demo
			//HandleFailureFinish();
			//return;
			ParentInput = GetRoot();
		}

		FMeshConfig* MeshConfig = nullptr;
		FMeshConfigData MeshConfigData;
		if (!TryReadInput("Config", MeshConfig))
		{
			UBF_LOG(Verbose, TEXT("[SpawnMesh] no MeshConfig provided, using default mesh config"));
			MeshConfigData = GetDefault<UUBFMeshConfigSettings>()->GetMeshConfigData();
		}
		else
		{
			UBF_LOG(Verbose, TEXT("[SpawnMesh] found meshconfig override"));
			MeshConfigData = MeshConfig->MeshConfigData;
		}
		
		if (!GetWorld() || !IsValid(GetWorld()) || GetWorld()->bIsTearingDown)
		{
			UBF_LOG(Error, TEXT("[SpawnMesh] GetWorld() is invalid %s"), *ResourceID);
			HandleFailureFinish();
			return;
		}
		
		GetContext().GetGraphProvider()->GetMeshResource(ResourceID, MeshConfigData.RuntimeConfig).Next([this, ResourceID, MeshConfigData, ParentInput](const FLoadMeshResult Result)
		{
			if (!Result.Result.Key)
			{
				UBF_LOG(Error, TEXT("[SpawnMesh] Failed to load mesh %s"), *ResourceID);
				HandleFailureFinish();
				return;
			}

			if (!GetWorld() || !IsValid(GetWorld()) || GetWorld()->bIsTearingDown)
			{
				UBF_LOG(Error, TEXT("[SpawnMesh] GetWorld() is invalid %s"), *ResourceID);
				HandleFailureFinish();
				return;
			}
			
			UglTFRuntimeAsset* Asset = Result.Result.Value;
			
			if (!Asset)
			{
				UBF_LOG(Error, TEXT("[SpawnMesh] MeshAsset invalid %s"), *ResourceID);
				HandleFailureFinish();
				return;
			}
			
			UBF_LOG(Verbose, TEXT("[SpawnMesh] ParentComp: %s"), *ParentInput->ToString());
				
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
			const auto SpawnedActor = GetWorld()->SpawnActorDeferred<AglTFRuntimeAssetActor>(AglTFRuntimeAssetActor::StaticClass(), FTransform::Identity);
			SpawnedActor->Asset = Asset;
			SpawnedActor->SkeletalMeshConfig = MeshConfigData.SkeletalMeshConfig;
			SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.BindDynamic(NewObject<UBoneRemapperUtil>(), &UBoneRemapperUtil::RemapFormatBoneName);
			
			SpawnedActor->bAllowNodeAnimations = MeshConfigData.bLoadAnimation;
			SpawnedActor->bAllowPoseAnimations = MeshConfigData.bLoadAnimation;
			SpawnedActor->bAllowSkeletalAnimations = MeshConfigData.bLoadAnimation;
			SpawnedActor->bAutoPlayAnimations = MeshConfigData.bLoadAnimation;
			SpawnedActor->FinishSpawning(FTransform::Identity);

			SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.Clear();
			SpawnedActor->SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Context = nullptr;
			
			// assuming that if the parent is a child transform, it's a bone transform
			SpawnedActor->AttachToComponent(ParentInput->GetAttachmentComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, ParentInput->GetAttachmentSocket());
			
			TArray<UMeshComponent*> MeshComponents;
			SpawnedActor->GetComponents(MeshComponents);

			// assume first mesh component is the leader mesh component
			auto HasMultipleMeshes = MeshComponents.Num() > 1;
			const USkeletalMeshComponent* LeaderSkeletalMeshComponent = HasMultipleMeshes
				? Cast<USkeletalMeshComponent>(MeshComponents[0])
				: nullptr;
	
			FDynamicHandle MeshArray = FDynamicHandle::Array();
			
			// need to decide whether it is okay to pass mesh renderers as scene nodes
			FDynamicHandle SceneNodeArray = FDynamicHandle::Array();
			for (auto MeshComponent : MeshComponents)
			{
				MeshArray.Push(FDynamicHandle::ForeignHandled(new FMeshRenderer(MeshComponent)));
				SceneNodeArray.Push(FDynamicHandle::ForeignHandled(new FSceneNode(MeshComponent)));

				// need to decide whether this should be done in graph
				if(HasMultipleMeshes)
				{
					const auto SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponent);
					if (SkeletalMeshComponent)
					{
						SkeletalMeshComponent->SetLeaderPoseComponent(LeaderSkeletalMeshComponent());
					}
				}
			}
			WriteOutput("Renderers", MeshArray);
			WriteOutput("Scene Nodes", SceneNodeArray);

			TriggerNext();
			CompleteAsyncExecution();
		});
	}

	void FSpawnMeshNode::HandleFailureFinish() const
	{
		WriteOutput("Renderers", FDynamicHandle::Array());
		
		FDynamicHandle SceneNodeArray = FDynamicHandle::Array();
		SceneNodeArray.Push(FDynamicHandle::ForeignHandled(new FSceneNode(GetRoot())));
		WriteOutput("Scene Nodes", SceneNodeArray);
		
		TriggerNext();
		CompleteAsyncExecution();
	}
}
