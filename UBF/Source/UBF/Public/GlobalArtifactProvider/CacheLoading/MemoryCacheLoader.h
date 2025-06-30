// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "ICacheLoader.h"
#include "UBFLog.h"

class UBF_API FMemoryCacheLoader final : public ICacheLoader
{
public:
	virtual void CacheBytes(const FString& Uri, const FString& Hash, const TArray<uint8>& Bytes) override
	{
		CachedBytesMap.Add(Uri, FCacheEntry{ Hash, Bytes });
		UE_LOG(LogUBF, VeryVerbose, TEXT("FMemoryCacheLoader::CacheBytes Cached bytes for %s with hash %s"), *Uri, *Hash);
	}

	virtual bool TryGetCachedBytes(const FString& Uri, const FString& Hash, TArray<uint8>& CachedBytes) const override
	{
		const FCacheEntry* Entry = CachedBytesMap.Find(Uri);
		if (Entry && Entry->Hash == Hash)
		{
			CachedBytes = Entry->Bytes;
			UE_LOG(LogUBF, VeryVerbose, TEXT("FMemoryCacheLoader::TryGetCachedBytes Cache hit for %s with matching hash %s"), *Uri, *Hash);
			return true;
		}

		UE_LOG(LogUBF, VeryVerbose, TEXT("FMemoryCacheLoader::TryGetCachedBytes Cache miss or hash mismatch for %s (requested hash: %s)"), *Uri, *Hash);
		return false;
	}

private:
	struct FCacheEntry
	{
		FString Hash;
		TArray<uint8> Bytes;
	};

	TMap<FString, FCacheEntry> CachedBytesMap;
};

