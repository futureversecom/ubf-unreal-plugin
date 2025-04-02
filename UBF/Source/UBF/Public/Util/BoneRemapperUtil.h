// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "glTFRuntimeParser.h"
#include "UObject/Object.h"
#include "BoneRemapperUtil.generated.h"

/**
 * 
 */
UCLASS()
class UBF_API UBoneRemapperUtil : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	FString RemapFormatBoneName(const int32 NodeIndex, const FString& BoneName, UObject* Context);
};
