// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "SetBlendshapeNode.h"

#include "DataTypes/SceneNode.h"

namespace UBF
{
	void FSetBlendshapeNode::ExecuteAsync() const
	{
		FSceneNode* Target;
		if (!TryReadInput("Target", Target))
		{
			UBF_LOG(Warning, TEXT("FSetBlendshapeNode::ExecuteAsync No Target input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (!Target || !IsValid(Target->GetAttachmentComponent()) || !IsValid(Target->GetAttachmentComponent()->GetOwner()))
		{
			UBF_LOG(Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Target input was null"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		FString ID;
		float Value;
		if (!TryReadInputValue("ID", ID))
		{
			UBF_LOG(Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Failed to read input 'ID' cannot set blendshape"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		if (!TryReadInputValue("Value", Value))
		{
			UBF_LOG(Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Failed to read input 'Value' cannot set blendshape"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
		Target->GetAttachmentComponent()->GetOwner()->GetComponents(SkeletalMeshComponents);
		if (SkeletalMeshComponents.Num() == 0)
		{
			UBF_LOG(Warning, TEXT("FSetBlendshapeNode::ExecuteAsync Failed to find any USkeletalMeshComponent for Target input "));
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
