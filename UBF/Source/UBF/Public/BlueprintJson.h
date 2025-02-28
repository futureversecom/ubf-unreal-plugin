#pragma once

struct UBF_API FBlueprintJson
{
public:
	FBlueprintJson(){}
	virtual ~FBlueprintJson(){}
	
	FBlueprintJson(const FString& Id, const FString& Json) : Id(Id), GraphJson(Json)
	{

	}

	bool IsValid() const { return !Id.IsEmpty(); }
	
	FString GetId() const { return Id; }
	const FString& GetGraphJson() const {return GraphJson;}
	
	bool operator==(const FBlueprintJson& Other) const
	{
		return Id == Other.GetId();
	}

private:
	FString Id;
	FString GraphJson;
};
