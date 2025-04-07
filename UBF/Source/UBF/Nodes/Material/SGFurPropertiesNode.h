// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "SGPropertiesBase.h"

namespace UBF
{
	class FSGFurPropertiesNode : public FSGPropertiesBase
	{
	protected:
		virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
		virtual FName GetBaseShaderName() const override {return FName("Fur");};
	};

}