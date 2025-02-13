#include "SGPropertiesBase.h"

#include "GraphProvider.h"
#include "UBFLog.h"
#include "UBFValues.h"

bool UBF::FSGPropertiesBase::ExecuteSync() const
{
	UBF_LOG(Verbose, TEXT("[SGPropertiesBase] Executing Node"));
	FShaderProperties* DecalProperties = new FShaderProperties();
	DecalProperties->BaseShaderName = GetBaseShaderName();
	AddProperties(DecalProperties->Properties);
            
	WriteOutput("Properties", FDynamicHandle::ForeignHandled(DecalProperties));
	
	return true;
}

void UBF::FSGPropertiesBase::AddColor(TMap<FString, FShaderPropertyValue>& PropertyMap,
	const FString& ResourceName, const FString& PropertyName) const
{
	FString Property;
	if (!TryReadInputValue(ResourceName, Property)) return;

	FColor Color;
	if (HexToColor(Property, Color))
	{
		PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeColor(Color));
	}
}

void UBF::FSGPropertiesBase::AddFloat(TMap<FString, FShaderPropertyValue>& PropertyMap,
	const FString& ResourceName, const FString& PropertyName) const
{
	float Property;
	if (!TryReadInputValue<float>(ResourceName, Property)) return;

	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeFloat(Property));
}

void UBF::FSGPropertiesBase::AddInt(TMap<FString, FShaderPropertyValue>& PropertiesDictionary,
	const FString& ResourceName, const FString& PropertyName) const
{
	int Property;
	if (!TryReadInputValue<int>(ResourceName, Property)) return;

	PropertiesDictionary.Add(PropertyName, FShaderPropertyValue::MakeInt(Property));
}

void UBF::FSGPropertiesBase::AddBool(TMap<FString, FShaderPropertyValue>& PropertyMap,
	const FString& ResourceName, const FString& PropertyName) const
{
	bool Property;
	if (!TryReadInputValue<bool>(ResourceName, Property)) return;

	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeBool(Property));
}

void UBF::FSGPropertiesBase::AddTexture(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
	const FString& PropertyName, bool bIsNormal) const
{
	FString ResourceID;
	if (!TryReadInputValue(ResourceName, ResourceID))
	{
		UBF_LOG(Verbose, TEXT("UBF::FSGPropertiesBase::AddTexture failed to get ResourceInfo from ResourceID %s"), *ResourceName);
		return;
	}
	FTextureHandle TextureHandle;
	TextureHandle.ResourceId = ResourceID;
	TextureHandle.IsNormal = bIsNormal;
	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeTexture(TextureHandle));
}
