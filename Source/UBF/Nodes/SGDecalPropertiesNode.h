#pragma once
#include "SGPropertiesBase.h"

namespace UBF
{
	class UBF_API FSGDecalPropertiesNode : public FSGPropertiesBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("Decal");};
	};
}

