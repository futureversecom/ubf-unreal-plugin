#pragma once

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

		UE_LOG(LogUBF, Warning, TEXT("Failed to create new dynamic for Type:%s Value: %s"), *Type, *Value);
		return UBF::FDynamicHandle::Null();
	}
}
