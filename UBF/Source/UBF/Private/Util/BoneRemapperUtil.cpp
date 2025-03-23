// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Util/BoneRemapperUtil.h"

FString UBoneRemapperUtil::RemapFormatBoneName(const int32 NodeIndex, const FString& BoneName, UObject* Context)
{
	return BoneName.Replace(TEXT("."), TEXT("_"));
}