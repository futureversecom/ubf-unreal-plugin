// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/MeshConfig.h"
#include "Engine/DeveloperSettings.h"
#include "UBFMeshConfigSettings.generated.h"

/**
 * Lets UBF users customize default MeshConfigs
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "UBF MeshConfig Settings"))
class UBF_API UUBFMeshConfigSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	FMeshConfigData GetMeshConfigData(const FString& ResourceID) const;
	FMeshConfigData GetMeshConfigData() const;
private:
	UPROPERTY(EditAnywhere, Config)
	FMeshConfigData DefaultMeshConfigData;
	UPROPERTY(EditAnywhere, Config)
	TMap<FString, FMeshConfigData> DefaultMeshConfigDataOverrides;
};
