#pragma once
#include "CustomNode.h"

namespace UBF
{
	class FSpawnMeshFromLODsNode : FCustomNode
	{
	protected:
		virtual void ExecuteAsync() const override;

		void HandleFailureFinish() const;
	};
}

