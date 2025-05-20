// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CustomNode.h"

namespace UBF
{
	class UBF_API FSetBlendshapeNode final : public FCustomNode
	{
	protected:
		virtual void ExecuteAsync() const override;
	};
}

