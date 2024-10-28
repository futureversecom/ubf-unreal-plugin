#pragma once
#include "CustomNode.h"

namespace UBF
{
	class UBF_API FMaterialNode final : public FCustomNode
	{
	protected:
		virtual bool ExecuteSync() const override;
	};
}
