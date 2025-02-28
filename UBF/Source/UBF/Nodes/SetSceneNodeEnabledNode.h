#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FSetSceneNodeEnabledNode : public FCustomNode
	{
	protected:
		virtual bool ExecuteSync() const override;
	};

}
