#pragma once
#include "Dynamic.h"

struct UBF_API FBlueprintInstanceBinding
{
	FBlueprintInstanceBinding(){}
	
	FString Id;
	FString Type;
	FString Value;
};

struct UBF_API FBlueprintInstance
{
public:
	FBlueprintInstance(){}
	virtual ~FBlueprintInstance(){}
	
	FBlueprintInstance(const FString& NewBlueprintId, const TMap<FString, FBlueprintInstanceBinding>& NewBindings)
	{
		BlueprintId = NewBlueprintId;
		Bindings = NewBindings;
	}
	
	virtual FString GetId() const { return Id; }
	
	virtual FString GetBlueprintId() const { return BlueprintId; }
	void SetBlueprintId(const FString& NewBlueprintId) { BlueprintId = NewBlueprintId; }
	
	virtual TMap<FString, UBF::FDynamicHandle>& GetVariablesRef() const;
	
	TMap<FString, FBlueprintInstanceBinding>& GetBindingsRef() { return Bindings; }
	void AddBinding(const FString& BindingId, const FBlueprintInstanceBinding& Binding)
	{
		Bindings.Add(BindingId, Binding);
	}

	bool operator==(const FBlueprintInstance& Other) const
	{
		return Id == Other.GetId();
	}

private:
	FString Id = FGuid::NewGuid().ToString();
	FString BlueprintId;
	TMap<FString, UBF::FDynamicHandle> Variables;
	TMap<FString, FBlueprintInstanceBinding> Bindings;
};
