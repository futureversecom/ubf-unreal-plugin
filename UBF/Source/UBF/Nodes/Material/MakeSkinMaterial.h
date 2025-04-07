#pragma once
#include "MakeMaterialBase.h"

namespace UBF
{
	class FMakeSkinMaterial : public FMakeMaterialBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("Skin");}
	};
}

