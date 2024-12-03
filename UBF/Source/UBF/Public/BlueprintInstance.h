#pragma once
#include "Dynamic.h"

class UBF_API IBlueprintInstance
{
public:
	virtual FString GetId() = 0;
	virtual FString GetBlueprintId() = 0;
	virtual TMap<FString, UBF::FDynamicHandle>& GetVariables() = 0;

	virtual ~IBlueprintInstance() {}
};


struct UBF_API FDefaultBlueprintInstance : IBlueprintInstance
{
	
public:

	FDefaultBlueprintInstance(const FString& Id, const TMap<FString, UBF::FDynamicHandle>& NewVariables)
	{
		BlueprintId = Id;
		Variables = NewVariables;
	}
	
	virtual FString GetId() override { return BlueprintId; }
	virtual FString GetBlueprintId() override { return BlueprintId; }
	virtual TMap<FString, UBF::FDynamicHandle>& GetVariables() override { return Variables; }

private:
	FString BlueprintId;
	TMap<FString, UBF::FDynamicHandle> Variables;
};
