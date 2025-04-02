// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

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