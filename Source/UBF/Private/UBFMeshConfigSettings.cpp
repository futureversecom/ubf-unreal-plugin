// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFMeshConfigSettings.h"

FMeshConfigData UUBFMeshConfigSettings::GetMeshConfigData(const FString& ResourceID) const
{
	// Check direct match first for efficiency
	if (DefaultMeshConfigDataOverrides.Contains(ResourceID))
		return DefaultMeshConfigDataOverrides[ResourceID];

	// Check for partial string matches
	for (const auto& Pair : DefaultMeshConfigDataOverrides)
	{
		if (ResourceID.Contains(Pair.Key))
		{
			return Pair.Value;
		}
	}

	return DefaultMeshConfigData;
}

FMeshConfigData UUBFMeshConfigSettings::GetMeshConfigData() const
{
	return DefaultMeshConfigData;
}
