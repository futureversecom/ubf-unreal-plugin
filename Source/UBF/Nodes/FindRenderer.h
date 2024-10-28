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