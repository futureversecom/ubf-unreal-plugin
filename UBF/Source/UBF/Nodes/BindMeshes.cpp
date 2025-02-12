// Fill out your copyright notice in the Description page of Project Settings.


#include "BindMeshes.h"

#include "DataTypes/MeshRenderer.h"


namespace UBF
{
	void FBindMeshes::ExecuteAsync() const
	{
		TArray<FMeshRenderer*> Mesh;
		if (!TryReadInputArray("Mesh", Mesh))
		{
			UBF_LOG(Verbose, TEXT("[BindMeshes] No Mesh input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		FMeshRenderer* Target;
		if (!TryReadInput("Skeleton", Target))
		{
			UBF_LOG(Verbose, TEXT("[BindMeshes] No Skeleton input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (Mesh.Num() == 0 || !Target)
		{
			UBF_LOG(Verbose, TEXT("[BindMeshes] Skeleton or Mesh input was null"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (Target->GetSkeletalMesh())
		{
			for (auto MeshRenderer : Mesh)
			{
				if (MeshRenderer->GetSkeletalMesh() && Target->GetSkeletalMesh())
					MeshRenderer->GetSkeletalMesh()->SetLeaderPoseComponent(Target->GetSkeletalMesh());
			}	
		}
		
		TriggerNext();
		CompleteAsyncExecution();
	}
}
