#include "TransformScaleNode.h"

namespace UBF
{
	void FTransformScaleNode::ExecuteAsync() const
	{
		FSceneNode* SceneNode;
		if (!TryReadInput("Transform Object", SceneNode))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] No Transform Object input found"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		bool bUseWorldSpace = false;
		if (!TryReadInputValue("Use World Space", bUseWorldSpace))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] Failed to Read bUseWorldSpace Input"));
		}

		bool bIsAdditive = false;
		if (!TryReadInputValue("Is Additive", bIsAdditive))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] Failed to Read Is Additive Input"));
		}
		
		float Right = 0.0f;
		if (!TryReadInputValue("Right", Right))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] Failed to Read X Input"));
		}
		
		float Up = 0.0f;
		if (!TryReadInputValue("Up", Up))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] Failed to Read Y Input"));
		}
		
		float Forward = 0.0f;
		if (!TryReadInputValue("Forward", Forward))
		{
			UE_LOG(LogUBF, Warning, TEXT("[TransformScaleNode] Failed to Read Z Input"));
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

