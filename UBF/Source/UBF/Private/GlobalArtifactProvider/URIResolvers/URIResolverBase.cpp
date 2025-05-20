// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "GlobalArtifactProvider/URIResolvers/URIResolverBase.h"

bool UURIResolverBase::CanResolveURI(const FString& URI)
{
	return false;
}

TFuture<UBF::FLoadDataArrayResult> UURIResolverBase::ResolveURI(const FString& TypeId, const FString& URI)
{
	TPromise<UBF::FLoadDataArrayResult> Promise = TPromise<UBF::FLoadDataArrayResult>();

	UBF::FLoadDataArrayResult Result;
	Promise.SetValue(Result);
	return Promise.GetFuture();
}