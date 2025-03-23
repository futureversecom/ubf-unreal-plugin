// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FFindRenderer : public FCustomNode
	{
	public:
		virtual bool ExecuteSync() const override;
	};
}