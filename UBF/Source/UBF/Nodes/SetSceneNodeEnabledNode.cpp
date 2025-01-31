#include "SetSceneNodeEnabledNode.h"

bool UBF::FSetSceneNodeEnabledNode::ExecuteSync() const
{
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

	if (!SceneNode || !IsValid(SceneNode->GetAttachmentComponent())) return false;

	SceneNode->GetAttachmentComponent()->SetActive(bEnabled);
	
	return true;
}
