// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "Dynamic.h"

namespace UBF
{
	struct UBF_API FBlueprintInstance
	{
	public:
		FBlueprintInstance(){}
		FBlueprintInstance(const FString& BlueprintId) : BlueprintId(BlueprintId), InstanceId(FGuid::NewGuid().ToString()){}

		void AddInput(const FString& Key, const FDynamicHandle& Handle);
		void AddInputs(const TMap<FString, FDynamicHandle>& NewInputs);

		FString GetBlueprintId() const {return BlueprintId;}
		FString GetInstanceId() const {return InstanceId;}
		const TMap<FString, FDynamicHandle>& GetInputs() const {return Inputs;}

		FString ToString() const
		{
			// Implement please ChatGPT
			FString InputStrings;
			for (const auto& InputPair : Inputs)
			{
				InputStrings += FString::Printf(TEXT("[%s: %s], "), *InputPair.Key, *InputPair.Value.ToString());
			}

			// Remove the trailing comma and space, if any
			if (!InputStrings.IsEmpty())
			{
				InputStrings.LeftChopInline(2);
			}

			return FString::Printf(
				TEXT("BlueprintId: %s, InstanceId: %s, Inputs: {%s}"),
				*BlueprintId,
				*InstanceId,
				*InputStrings
			);
		}

		bool IsValid() const {return !BlueprintId.IsEmpty();}
	private:
		FString BlueprintId;
		FString InstanceId;
		TMap<FString, FDynamicHandle> Inputs;
	};

}
