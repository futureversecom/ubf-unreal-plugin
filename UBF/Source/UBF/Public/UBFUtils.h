#pragma once
#include "UBFBindingObject.h"

namespace UBFUtils
{
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
		if (Type == "boolean")
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

		UE_LOG(LogUBF, Warning, TEXT("Failed to create new dynamic for Type:%s Value: %s"), *Type, *Value);
		return UBF::FDynamicHandle::Null();
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
