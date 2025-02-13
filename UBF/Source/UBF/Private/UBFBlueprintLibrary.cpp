// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFBlueprintLibrary.h"
#include "UBFUtils.h"

UUBFBindingObject* UUBFBlueprintLibrary::CreateNewInputBindingObject(const FString& Id, const FString& Type, const FString& Value)
{
	return UBFUtils::CreateNewInputBindingObject(Id, Type, Value);
}
