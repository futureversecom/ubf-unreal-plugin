// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFBindingObject.h"

#include "UBFUtils.h"

FString UUBFBindingObject::DynamicToString(const UBF::FDynamicHandle& DynamicHandle) const
{
	FString InputValue;
			
	if (BindingInfo.Type == "string")
	{
		FString StringValue;
		DynamicHandle.TryInterpretAs(StringValue);
		InputValue = StringValue; 
	}
	if (BindingInfo.Type == "Color")
	{
		FString StringValue;
		DynamicHandle.TryInterpretAs(StringValue);
		InputValue = StringValue; 
	}
	if (BindingInfo.Type == "int")
	{
		int32 IntValue;
		DynamicHandle.TryInterpretAs(IntValue);
		InputValue = FString::FromInt(IntValue);
	}
	if (BindingInfo.Type == "float")
	{
		float FloatValue;
		DynamicHandle.TryInterpretAs(FloatValue);
		InputValue = FString::SanitizeFloat(FloatValue);
	}
	if (BindingInfo.Type == "boolean")
	{
		bool BoolValue;
		DynamicHandle.TryInterpretAs(BoolValue);
		InputValue = BoolValue ? "true" : "false";
	}
			
	return InputValue;
}

UBF::FDynamicHandle UUBFBindingObject::DynamicToString(const FString& InputValue) const
{
	return UBFUtils::CreateNewDynamicHandle(BindingInfo.Type, InputValue);
}