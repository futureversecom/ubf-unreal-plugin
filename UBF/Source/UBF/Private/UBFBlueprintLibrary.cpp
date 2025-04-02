// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "UBFBlueprintLibrary.h"
#include "UBFUtils.h"

UUBFBindingObject* UUBFBlueprintLibrary::CreateNewInputBindingObject(const FString& Id, const FString& Type, const FString& Value)
{
	return UBFUtils::CreateNewInputBindingObject(Id, Type, Value);
}
