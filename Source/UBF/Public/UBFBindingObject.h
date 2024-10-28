// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph.h"
#include "UObject/Object.h"
#include "UBFBindingObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValueOverriden);
/**
 * 
 */
UCLASS(Blueprintable)
class UBF_API UUBFBindingObject : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const UBF::FBindingInfo& NewBindingInfo)
	{
		BindingInfo = NewBindingInfo;
		BindingValue = DynamicToString(BindingInfo.DynamicPtr);
		bIsOverridden = false;
		UE_LOG(LogUBF, VeryVerbose, TEXT("Initializing UBFBindingObject. Id: %s Type: %s Value: %s"),
			*BindingInfo.Id, *BindingInfo.Type, *BindingValue);
	}

	UFUNCTION(BlueprintCallable)
	FString GetId() const { return BindingInfo.Id; }

	UFUNCTION(BlueprintCallable)
	FString GetType() const { return BindingInfo.Type; }
	
	UFUNCTION(BlueprintCallable)
	FString GetValue() const { return BindingValue; }
	
	UBF::FDynamicHandle GetDynamicFromValue() const { return DynamicToString(BindingValue); }

	UBF::FBindingInfo GetBindingInfo() const { return BindingInfo; }

	UFUNCTION(BlueprintCallable)
	void OverrideValue(const FString& NewValue)
	{
		BindingValue = NewValue;
		bIsOverridden = true;
		OnValueOverridenDelegate.Broadcast();
		UE_LOG(LogUBF, VeryVerbose, TEXT("Overriding UBFBindingObject. Id: %s Type: %s Value: %s"),
			*BindingInfo.Id, *BindingInfo.Type, *BindingValue);
	}

	bool IsMatchingBindingAndOverridden(const UBF::FBindingInfo& Other) const
	{
		return BindingInfo.Id.Equals(Other.Id) && BindingInfo.Type == Other.Type && bIsOverridden;
	}

	FString ToString()
	{
		return FString::Printf(TEXT("Id: %s, Type: %s, Value: %s"), 
		*BindingInfo.Id, *BindingInfo.Type, *BindingValue);
	}

	UPROPERTY(BlueprintAssignable)
	FOnValueOverriden OnValueOverridenDelegate;
	
private:
	FString DynamicToString(const UBF::FDynamicHandle& DynamicHandle) const;
	UBF::FDynamicHandle DynamicToString(const FString& InputValue) const;

	UBF::FBindingInfo BindingInfo;
	FString BindingValue;
	bool bIsOverridden = false;
};
