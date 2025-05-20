// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "GlobalArtifactProvider/URIResolvers/DefaultURIResolver.h"

#include "GraphProvider.h"
#include "GlobalArtifactProvider/DownloadRequestManager.h"

bool UDefaultURIResolver::CanResolveURI(const FString& URI)
{
	return true;
}

TFuture<UBF::FLoadDataArrayResult> UDefaultURIResolver::ResolveURI(const FString& TypeId, const FString& URI)
{
	return FDownloadRequestManager::GetInstance()->LoadDataFromURI(TypeId, URI);
}
