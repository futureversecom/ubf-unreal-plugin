#include "SetTextureSettingsNode.h"

bool UBF::FSetTextureSettingsNode::ExecuteSync() const
{
	FString ResourceID;
	if (!TryReadInputValue("Texture Resource", ResourceID))
	{
		UE_LOG(LogUBF, Verbose, TEXT("[SetTextureSettings] No valid texture provided"));
		return true;
	}

	FTextureSettings* TextureSettings = new FTextureSettings();
	
	TryReadInputValue<bool>("sRGB", TextureSettings->bUseSRGB);
	
	SetDynamicDataEntry(ResourceID, FDynamicHandle::ForeignHandled(TextureSettings));
	//Debug.Log("Writing texture import settings for id: " + resourceId);
            
	WriteOutput("Texture", FDynamicHandle::String(ResourceID));
	
	return true;
}
