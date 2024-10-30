#pragma once
#include "SGPropertiesBase.h"

namespace UBF
{
	class FSGPBRPropertiesNode : public FSGPropertiesBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("PBR");};
	};
}