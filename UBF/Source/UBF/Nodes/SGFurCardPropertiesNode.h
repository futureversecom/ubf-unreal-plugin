// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGPropertiesBase.h"

/**
 * 
 */
namespace UBF
{
	class UBF_API FSGFurCardPropertiesNode : public FSGPropertiesBase
	{
		protected:
			virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const override;
			virtual FName GetBaseShaderName() const override {return FName("FurCard");};
	};
}

