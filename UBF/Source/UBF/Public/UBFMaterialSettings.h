// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UBFMaterialSettings.generated.h"

/**
 * 
 */
UCLASS(Config=UBF, meta = (DisplayName = "UBF Material Settings"))
class UBF_API UUBFMaterialSettings : public UObject
{
	GENERATED_BODY()
public:
	UMaterialInterface* GetMaterialForName(const FName& InName, const bool bUseTransparent) const;
	
private:
	UPROPERTY(EditAnywhere, Config)
	TMap<FName, TSoftObjectPtr<UMaterialInterface>> OpaqueMaterialMap;
	
	UPROPERTY(EditAnywhere, Config)
	TMap<FName, TSoftObjectPtr<UMaterialInterface>> TransparentMaterialMap;
};
