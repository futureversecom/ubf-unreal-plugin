// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "GCPin.generated.h"

UCLASS()
class UBF_API UGCPin : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UObject* Object;

	void Release()
	{
		Object = nullptr;
		RemoveFromRoot();
	}
	
	static UGCPin* Pin(UObject* Object)
	{
		UGCPin* GCPin = NewObject<UGCPin>();
		GCPin->AddToRoot();
		GCPin->Object = Object;
		return GCPin;
	}
};
