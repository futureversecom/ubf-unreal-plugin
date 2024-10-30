#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FTextureSettings
	{
	public:
		bool bUseSRGB = false;
	};
	
	class FSetTextureSettingsNode : public FCustomNode
	{
	protected:
		virtual bool ExecuteSync() const override;

	public:
		
	};
}