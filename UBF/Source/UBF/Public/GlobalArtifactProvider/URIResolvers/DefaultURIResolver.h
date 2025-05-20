// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphProvider.h"
#include "URIResolverBase.h"
#include "DefaultURIResolver.generated.h"

/**
 * 
 */
UCLASS()
class UBF_API UDefaultURIResolver : public UURIResolverBase
{
	GENERATED_BODY()
public:
	virtual bool CanResolveURI(const FString& URI) override;

	virtual TFuture<UBF::FLoadDataArrayResult> ResolveURI(const FString& TypeId, const FString& URI) override;
};
