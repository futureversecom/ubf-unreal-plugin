// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "UBFBindingObject.h"

namespace UBFUtils
{
	inline bool HexToColor(const FString& HexString, FColor& OutColor)
	{
		FString CleanHexString = HexString.Replace(TEXT("#"), TEXT("")).TrimStartAndEnd();

		if (CleanHexString.Len() == 6 || CleanHexString.Len() == 8)
		{
			// Extract RGB(A) values from the hex string
			FString RHex = CleanHexString.Mid(0, 2);
			FString GHex = CleanHexString.Mid(2, 2);
			FString BHex = CleanHexString.Mid(4, 2);
			FString AHex = CleanHexString.Len() == 8 ? CleanHexString.Mid(6, 2) : TEXT("FF");

			// Convert hex strings to integers
			int32 Red = FParse::HexNumber(*RHex);
			int32 Green = FParse::HexNumber(*GHex);
			int32 Blue = FParse::HexNumber(*BHex);
			int32 Alpha = FParse::HexNumber(*AHex);

			OutColor = FColor(Red, Green, Blue, Alpha);
			return true;
		}

		// Return black if the string is invalid
		return false;
	}
	
	/* Converts Bytes to string that come from rust interpreter, prevents issues caused by builtin version */
	inline FString FromBytesToString(const uint8* In, int32 Count)
	{
		FString Result;
		Result.Empty(Count);

		while (Count)
		{
			int16 Value = *In;

			Result += static_cast<TCHAR>(Value);

			++In;
			Count--;
		}
		return Result;
	}

	inline FString FromBytesToString(const uint16* In, int32 Count)
	{
		FString Result;
		Result.Empty(Count);

		while (Count)
		{
			int16 Value = *In;

			Result += static_cast<TCHAR>(Value);

			++In;
			Count--;
		}
		return Result;
	}

	inline UBF::FDynamicHandle CreateNewDynamicHandle(const FString& Type, const FString& Value)
	{
		if (Type == "string")
		{
			return UBF::FDynamicHandle::String(Value);
		}
		if (Type == "int")
		{
			return UBF::FDynamicHandle::Int(FCString::Atoi(*Value));
		}
		if (Type == "Color")
		{
			return UBF::FDynamicHandle::String(Value);
		}
		if (Type == "float")
		{
			return UBF::FDynamicHandle::Float(FCString::Atof(*Value));
		}
		if (Type == "boolean" || Type == "bool")
		{
			return UBF::FDynamicHandle::Bool(Value == "true");
		}
		if (Type == "SceneNode")
		{
			return UBF::FDynamicHandle::Null();
		}
		if (Type == "Resource<Mesh>")
		{
			return UBF::FDynamicHandle::String(Value);
		}
		if (Type == "Resource<Texture>")
		{
			return UBF::FDynamicHandle::String(Value);
		}
		if (Type == "MeshConfig")
		{
			return UBF::FDynamicHandle::String(Value);
		}
		if (Type == "MeshRenderer")
		{
			return UBF::FDynamicHandle::Null();
		}

		UE_LOG(LogUBF, Warning, TEXT("Failed to create new dynamic for Type:%s Value: %s"), *Type, *Value);
		return UBF::FDynamicHandle::Null();
	}
	
	inline UUBFBindingObject* CreateNewInputBindingObject(const FString& Id, const FString& Type, const FString& Value)
	{
		UUBFBindingObject* BindingObject = NewObject<UUBFBindingObject>();

		const UBF::FDynamicHandle Dynamic = CreateNewDynamicHandle(Type, Value);
		const UBF::FBindingInfo BindingInfo(Id, Type, Dynamic);
		BindingObject->Initialize(BindingInfo);

		return BindingObject;
	}
	
	inline TMap<FString, UUBFBindingObject*> AsBindingObjectMap(const TMap<FString, UBF::FDynamicHandle>& NewInputMap)
	{
		TMap<FString, UUBFBindingObject*> ConvertedMap;
		
		for (auto& NewInput : NewInputMap)
		{
			UUBFBindingObject* BindingObject = NewObject<UUBFBindingObject>();
			const UBF::FBindingInfo BindingInfo(NewInput.Key, NewInput.Value.GetTypeString(), NewInput.Value);
			BindingObject->Initialize(BindingInfo);
			
			ConvertedMap.Add(NewInput.Key, BindingObject);
		}
		
		return ConvertedMap;
	}

	inline TMap<FString, UBF::FDynamicHandle> AsDynamicMap(const TMap<FString, UUBFBindingObject*>& NewInputMap)
	{
		TMap<FString, UBF::FDynamicHandle> ConvertedMap;
		
		for (auto& BindingObjectTuple : NewInputMap)
		{
			ConvertedMap.Add(BindingObjectTuple.Key, BindingObjectTuple.Value->GetDynamicFromValue());
		}
		
		return ConvertedMap;
	}
}
