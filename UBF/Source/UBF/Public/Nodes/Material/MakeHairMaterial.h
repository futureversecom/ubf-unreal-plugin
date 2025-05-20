#pragma once
#include "MakeMaterialBase.h"

namespace UBF
{
	class FMakeHairMaterial : public FMakeMaterialBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("Hair");}
	};
}

