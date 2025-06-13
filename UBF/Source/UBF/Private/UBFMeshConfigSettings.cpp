// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "UBFMeshConfigSettings.h"

UUBFMeshConfigSettings::UUBFMeshConfigSettings()
{
	CategoryName = TEXT("Plugins");
}

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
