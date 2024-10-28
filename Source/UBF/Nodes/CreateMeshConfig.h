#pragma once
#include "CustomNode.h"
#include "DataTypes/MeshConfig.h"

namespace UBF
{
	class FCreateMeshConfig : public FCustomNode
	{
	protected:
		virtual void ExecuteAsync() const override;

	private:
		void Complete(const FMeshConfigData& MeshConfigData) const;
	
	};

}
