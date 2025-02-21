// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/BoneRemapperUtil.h"

FString UBoneRemapperUtil::RemapFormatBoneName(const int32 NodeIndex, const FString& BoneName, UObject* Context)
{
	return BoneName.Replace(TEXT("."), TEXT("_"));
}