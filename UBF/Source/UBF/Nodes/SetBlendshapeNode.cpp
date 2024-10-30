// Fill out your copyright notice in the Description page of Project Settings.


#include "SetBlendshapeNode.h"

#include "DataTypes/SceneNode.h"

namespace UBF
{
	void FSetBlendshapeNode::ExecuteAsync() const
	{
		FSceneNode* Target;
		if (!TryReadInput("Target", Target))
		{
			UE_LOG(LogUBF, Warning, TEXT("FSetBlendshapeNode::ExecuteAsync No Target input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (!Target)
		{
			UE_LOG(LogUBF, Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Target input was null"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		FString ID;
		float Value;
		if (!ensure(TryReadInputValue("ID", ID))) return;
		if (!ensure(TryReadInputValue("Value", Value))) return;
		
		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Target->GetAttachmentComponent()->GetOwner()->GetComponents(SkeletalMeshComponents);
		if (SkeletalMeshComponents.Num() == 0)
		{
			UE_LOG(LogUBF, Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Failed to find any USkeletalMeshComponent for Target input "));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		for (auto SkeletalMeshComponent : SkeletalMeshComponents)
		{
			SkeletalMeshComponent->SetMorphTarget(FName(ID), Value);
		}
		
		TriggerNext();
		CompleteAsyncExecution();
	}
}
