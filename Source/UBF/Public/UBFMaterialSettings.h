// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UBFMaterialSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, defaultconfig, meta = (DisplayName = "UBF Material Settings"))
class UBF_API UUBFMaterialSettings : public UDeveloperSettings
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
