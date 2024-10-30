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
}
