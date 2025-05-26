// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Nodes/Mesh/SpawnMeshNode.h"

#include "glTFRuntimeAssetActor.h"
#include "GraphProvider.h"
#include "UBFMeshConfigSettings.h"
#include "DataTypes/MeshConfig.h"
#include "DataTypes/MeshRenderer.h"
#include "DataTypes/SceneNode.h"
#include "GlobalArtifactProvider/CachedMesh.h"
#include "GLTFRuntimeUtils/SpawnGLTFMeshLibrary.h"

namespace UBF
{
	void FSpawnMeshNode::ExecuteAsync() const
	{
		UBF_LOG(Verbose, TEXT("[SpawnMesh] Executing Node"));
		
		FString ResourceID;
		TArray<FMeshResource> Meshes;
		bool bUsesMeshes = false;
		
		if (!TryReadInputValue("Resource", ResourceID))
		{
			TArray<FString> RawMeshes;
			if (!TryReadInputValueArray("Meshes", RawMeshes))
			{
				UBF_LOG(Warning, TEXT("[SpawnMesh] failed to get Resource or Meshes input"));
				HandleFailureFinish();
				return;
			}

			for (auto ID : RawMeshes)
			{
				Meshes.Add(FMeshResource(ID));
			}
			bUsesMeshes = true;
		}
		
		UBF_LOG(Verbose, TEXT("[SpawnMesh] found Resource with ID %s"), *ResourceID);

		FSceneNode* ParentInput = nullptr;
		if (!TryReadInput("Parent", ParentInput))
		{
			UBF_LOG(Warning, TEXT("[SpawnMesh] Failed to Read 'Parent' Input"));
			
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
		
		if (bUsesMeshes)
		{
			GetContext().GetSetConfig()->GetMeshLOD(Meshes, MeshConfigData).Next([this, ResourceID, MeshConfigData, ParentInput](const FLoadMeshLODResult Result)
			{
				if (!Result.bSuccess)
				{
					UBF_LOG(Error, TEXT("[SpawnMesh] Failed to load mesh %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}

				if (!CheckExecutionStillValid())
				{
					HandleFailureFinish();
					return;
				}
			
				UStreamableRenderAsset* Mesh = Result.Value;
			
				if (!Mesh)
				{
					UBF_LOG(Error, TEXT("[SpawnMesh] MeshAsset invalid %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}
			
				UBF_LOG(Verbose, TEXT("[SpawnMesh] ParentComp: %s"), *ParentInput->ToString());

				AActor* SpawnedActor = USpawnGLTFMeshLibrary::SpawnLODMesh(GetWorld(), Mesh);

				if (!SpawnedActor)
				{
					HandleFailureFinish();
					return;
				}

				HandleSpawnedActor(SpawnedActor, ParentInput);
			});
		}
		else
		{
			GetContext().GetSetConfig()->GetMesh(ResourceID, FMeshImportSettings(MeshConfigData.RuntimeConfig)).Next([this, ResourceID, MeshConfigData, ParentInput](const FLoadMeshResult Result)
			{
				if (!Result.bSuccess)
				{
					UBF_LOG(Error, TEXT("[SpawnMesh] Failed to load mesh %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}

				if (!CheckExecutionStillValid())
				{
					HandleFailureFinish();
					return;
				}
				
				UglTFRuntimeAsset* Asset = Result.Value;
				
				if (!Asset)
				{
					UBF_LOG(Error, TEXT("[SpawnMesh] MeshAsset invalid %s"), *ResourceID);
					HandleFailureFinish();
					return;
				}
				
				UBF_LOG(Verbose, TEXT("[SpawnMesh] ParentComp: %s"), *ParentInput->ToString());

				AActor* SpawnedActor = USpawnGLTFMeshLibrary::SpawnMesh(GetWorld(), Asset, MeshConfigData);

				if (!SpawnedActor)
				{
					HandleFailureFinish();
					return;
				}

				HandleSpawnedActor(SpawnedActor, ParentInput);
			});
		}
	}
	
	void FSpawnMeshNode::HandleSpawnedActor(AActor* SpawnedActor, const FSceneNode* ParentInput) const
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(FSpawnMeshNode::ExecuteAsync_SetupActor);

		if (!GetRoot()->GetAttachmentComponent()->IsVisible())
		{
			SpawnedActor->SetActorHiddenInGame(true);
		}
					
		// assuming that if the parent is a child transform, it's a bone transform
		SpawnedActor->AttachToComponent(ParentInput->GetAttachmentComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, ParentInput->GetAttachmentSocket());
							
		TArray<UMeshComponent*> MeshComponents;
		SpawnedActor->GetComponents(MeshComponents);

		// temp fix: assume first mesh component is the leader mesh component
		auto HasMultipleMeshes = MeshComponents.Num() > 1;
		USkeletalMeshComponent* LeaderSkeletalMeshComponent = HasMultipleMeshes
			? Cast<USkeletalMeshComponent>(MeshComponents[0])
			: nullptr;
					
		FDynamicHandle MeshArray = FDynamicHandle::Array();
							
		// need to decide whether it is okay to pass mesh renderers as scene nodes
		FDynamicHandle SceneNodeArray = FDynamicHandle::Array();
		for (auto MeshComponent : MeshComponents)
		{
			MeshArray.Push(FDynamicHandle::ForeignHandled(new FMeshRenderer(MeshComponent)));
			SceneNodeArray.Push(FDynamicHandle::ForeignHandled(new FSceneNode(MeshComponent)));
		}

		// temp fix
		if(HasMultipleMeshes)
		{
			// skip first one as setting itself as the leader will cause infinite loop
			for (int i = 1; i < MeshComponents.Num(); ++i)
			{
				const auto SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComponents[i]);
				if (SkeletalMeshComponent && LeaderSkeletalMeshComponent)
				{
					SkeletalMeshComponent->SetLeaderPoseComponent(LeaderSkeletalMeshComponent);
				}
			}
		}
		UBF_LOG(Verbose, TEXT("[SpawnMesh] Outputting %d Renderers"), MeshComponents.Num());
		WriteOutput("Renderers", MeshArray);
		WriteOutput("Scene Nodes", SceneNodeArray);
	
				
		TriggerNext();
		CompleteAsyncExecution();
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
