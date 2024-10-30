// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFBindingObject.h"

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
	if (BindingInfo.Type == "string")
	{
		return UBF::FDynamicHandle::String(InputValue);
	}
	if (BindingInfo.Type == "int")
	{
		return UBF::FDynamicHandle::Int(FCString::Atoi(*InputValue));
	}
	if (BindingInfo.Type == "Color")
	{
		return UBF::FDynamicHandle::String(InputValue);
	}
	if (BindingInfo.Type == "float")
	{
		return UBF::FDynamicHandle::Float(FCString::Atof(*InputValue));
	}
	if (BindingInfo.Type == "boolean")
	{
		return UBF::FDynamicHandle::Bool(InputValue == "true");
	}

	UE_LOG(LogUBF, Warning, TEXT("Failed to convert input to dynamic for Id:%s Type: %s"), *BindingInfo.Id, *BindingInfo.Type);
	return UBF::FDynamicHandle::Null();
}