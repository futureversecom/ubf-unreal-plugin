#pragma once
#include "CustomNode.h"

namespace UBF
{
	class UBF_API FCreateMeshResource final : public FCustomNode
	{
		protected:
			virtual void ExecuteAsync() const override;
	};
}
