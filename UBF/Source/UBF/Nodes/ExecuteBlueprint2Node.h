// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FExecuteBlueprint2Node final : public FCustomNode
	{
	public:
	protected:
		virtual void ExecuteAsync() const override;
		
		mutable FExecutionContextHandle ExecContext;
	};
}
