#pragma once
#include "MakeMaterialBase.h"

namespace UBF
{
	struct FShaderPropertyValue;

	class FMakeFurMaterial : public FMakeMaterialBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("Fur");}
	};
}

