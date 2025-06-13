// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "URIResolvers/URIResolverBase.h"
#include "GlobalArtifactProviderSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Engine, defaultconfig, meta = (DisplayName = "Global Artifact Provider"))
class UBF_API UGlobalArtifactProviderSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UGlobalArtifactProviderSettings();
	
	UPROPERTY(EditAnywhere, Config, meta=(MetaClass="/Script/UBF.URIResolverBase"))
	TArray<FSoftClassPath> URIResolvers {FSoftClassPath(TEXT("/Script/UBF.DefaultURIResolver"))};
};
