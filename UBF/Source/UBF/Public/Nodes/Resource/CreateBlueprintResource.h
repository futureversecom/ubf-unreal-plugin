#pragma once
#include "CustomNode.h"

namespace UBF
{
	class UBF_API FCreateBlueprintResource final : public FCustomNode
	{
		protected:
			virtual void ExecuteAsync() const override;
	};
}
