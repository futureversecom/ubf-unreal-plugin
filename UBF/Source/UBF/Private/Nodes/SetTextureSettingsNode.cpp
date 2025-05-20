// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Nodes/SetTextureSettingsNode.h"

bool UBF::FSetTextureSettingsNode::ExecuteSync() const
{
	FString ResourceID;
	if (!TryReadInputValue("Texture Resource", ResourceID))
	{
		UBF_LOG(Verbose, TEXT("[SetTextureSettings] No valid texture provided"));
		WriteOutput("Texture", FDynamicHandle::String(FString()));
		return true;
	}

	FTextureSettings* TextureSettings = new FTextureSettings();
	
	TryReadInputValue<bool>("sRGB", TextureSettings->bUseSRGB);
	
	SetDynamicDataEntry(ResourceID, FDynamicHandle::ForeignHandled(TextureSettings));
	//Debug.Log("Writing texture import settings for id: " + resourceId);
            
	WriteOutput("Texture", FDynamicHandle::String(ResourceID));
	
	return true;
}
