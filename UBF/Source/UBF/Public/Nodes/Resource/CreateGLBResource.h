#pragma once
#include "CustomNode.h"

namespace UBF
{
	class UBF_API FCreateGLBResource final : public FCustomNode
	{
		protected:
			virtual void ExecuteAsync() const override;
	};
}

