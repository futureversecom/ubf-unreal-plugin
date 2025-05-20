// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Nodes/TransformPositionNode.h"

namespace UBF
{
	void FTransformPositionNode::ExecuteAsync() const
	{
		FSceneNode* SceneNode;
		if (!TryReadInput("Transform Object", SceneNode))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] No Transform Object input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		bool bUseWorldSpace = false;
		if (!TryReadInputValue("Use World Space", bUseWorldSpace))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] Failed to Read bUseWorldSpace Input"));
		}

		bool bIsAdditive = false;
		if (!TryReadInputValue("Is Additive", bIsAdditive))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] Failed to Read Is Additive Input"));
		}
		
		float Right = 0.0f;
		if (!TryReadInputValue("Right", Right))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] Failed to Read X Input"));
		}
		
		float Up = 0.0f;
		if (!TryReadInputValue("Up", Up))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] Failed to Read Y Input"));
		}
		
		float Forward = 0.0f;
		if (!TryReadInputValue("Forward", Forward))
		{
			UBF_LOG(Warning, TEXT("[TransformPositionNode] Failed to Read Z Input"));
		}
		
		// these values are in meters
		FVector ValueToApply = FVector(Forward, Right, Up) * 100.0f;
		
		FVector TargetLocation = bUseWorldSpace
			? SceneNode->GetAttachmentComponent()->GetComponentLocation()
			: SceneNode->GetAttachmentComponent()->GetRelativeLocation();
		
		TargetLocation = bIsAdditive
			? TargetLocation + ValueToApply
			: ValueToApply;
		
		if (bUseWorldSpace)
		{
			SceneNode->GetAttachmentComponent()->SetWorldLocation(TargetLocation);
		}
		else
		{
			SceneNode->GetAttachmentComponent()->SetRelativeLocation(TargetLocation);
		}
		
		TriggerNext();
		CompleteAsyncExecution();
	}
}

