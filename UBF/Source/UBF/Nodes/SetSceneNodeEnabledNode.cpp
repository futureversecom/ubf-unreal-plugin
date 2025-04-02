// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "SetSceneNodeEnabledNode.h"

bool UBF::FSetSceneNodeEnabledNode::ExecuteSync() const
{
	UBF_LOG(Verbose, TEXT("[SetSceneNodeEnabledNode] Executing Node"));
	
	FSceneNode* SceneNode = nullptr;
	if (!TryReadInput("Node", SceneNode))
	{
		TriggerNext();
		return true;
	}

	bool bEnabled = false;
	if (!TryReadInputValue("Enabled", bEnabled))
	{
		TriggerNext();
		return true;
	}

	if (!SceneNode || !IsValid(SceneNode->GetAttachmentComponent()))
	{
		UBF_LOG(Warning, TEXT("[SetSceneNodeEnabled] Failed to set SceneNode state as it was invalid!"));
		return false;
	}
	
	UBF_LOG(Verbose, TEXT("[SetSceneNodeEnabled] SceneNode: %s"), *SceneNode->ToString());
	
	SceneNode->GetAttachmentComponent()->SetVisibility(bEnabled, true);
	
	return true;
}
