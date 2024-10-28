// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFMaterialSettings.h"

UMaterialInterface* UUBFMaterialSettings::GetMaterialForName(const FName& InName, const bool bUseTransparent) const
{
	const auto MaterialMap = bUseTransparent ? TransparentMaterialMap : OpaqueMaterialMap;
	
	if (!MaterialMap.Contains(InName)) return nullptr;

	return MaterialMap[InName].LoadSynchronous();
}
