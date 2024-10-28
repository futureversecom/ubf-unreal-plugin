#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FExecuteBlueprintNode final : public FCustomNode
	{
	public:
	protected:
		virtual void ExecuteAsync() const override;
		
		mutable FExecutionContextHandle ExecContext;
	};
}
