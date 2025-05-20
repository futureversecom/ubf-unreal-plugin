// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GraphProvider.h"
#include "UObject/Object.h"
#include "URIResolverBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, EditInlineNew)
class UBF_API UURIResolverBase : public UObject
{
	GENERATED_BODY()
public:
	/* Return true if this resolver can handle that URI */
	virtual bool CanResolveURI(const FString& URI);
	
	/* TypeId, is used to identify what type of download this is, Mesh, Texture etc */
	virtual TFuture<UBF::FLoadDataArrayResult> ResolveURI(const FString& TypeId, const FString& URI);
};
