// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "TransformScaleNode.h"

namespace UBF
{
	void FTransformScaleNode::ExecuteAsync() const
	{
		FSceneNode* SceneNode;
		if (!TryReadInput("Transform Object", SceneNode))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] No Transform Object input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		bool bUseWorldSpace = false;
		if (!TryReadInputValue("Use World Space", bUseWorldSpace))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] Failed to Read bUseWorldSpace Input"));
		}

		bool bIsAdditive = false;
		if (!TryReadInputValue("Is Additive", bIsAdditive))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] Failed to Read Is Additive Input"));
		}
		
		float Right = 0.0f;
		if (!TryReadInputValue("Right", Right))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] Failed to Read X Input"));
		}
		
		float Up = 0.0f;
		if (!TryReadInputValue("Up", Up))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] Failed to Read Y Input"));
		}
		
		float Forward = 0.0f;
		if (!TryReadInputValue("Forward", Forward))
		{
			UBF_LOG(Warning, TEXT("[TransformScaleNode] Failed to Read Z Input"));
		}
		
		// these values are in meters
		FVector ValueToApply = FVector(Forward, Right, Up);
		
		FVector TargetScale = bUseWorldSpace
			? SceneNode->GetAttachmentComponent()->GetComponentScale()
			: SceneNode->GetAttachmentComponent()->GetRelativeScale3D();
		
		TargetScale = bIsAdditive
			? TargetScale + ValueToApply
			: ValueToApply;
		
		if (bUseWorldSpace)
		{
			SceneNode->GetAttachmentComponent()->SetWorldScale3D(TargetScale);
		}
		else
		{
			SceneNode->GetAttachmentComponent()->SetRelativeScale3D(TargetScale);
		}
		
		TriggerNext();
		CompleteAsyncExecution();
	}
}

