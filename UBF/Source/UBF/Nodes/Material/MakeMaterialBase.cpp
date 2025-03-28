#include "MakeMaterialBase.h"

#include "UBFMaterialSettings.h"
#include "UBFUtils.h"
#include "UBFValues.h"

bool UBF::FMakeMaterialBase::ExecuteSync() const
{
	UBF_LOG(Verbose, TEXT("[FMakeMaterialBase] Executing Node"));
	FShaderProperties* ShaderProperties = new FShaderProperties();
	ShaderProperties->BaseShaderName = GetBaseShaderName();
	AddProperties(ShaderProperties->Properties);
	
	const UUBFMaterialSettings* Settings = GetDefault<UUBFMaterialSettings>();

	check(Settings);
		
	bool bUseTransparent = false;
	if (ShaderProperties->Properties.Contains("_UseAlpha"))
	{
		const FShaderPropertyValue AlphaPropertyValue = ShaderProperties->Properties["_UseAlpha"];
			
		if (AlphaPropertyValue.PropertyType == EShaderPropertyType::Boolean)
		{
			bUseTransparent = AlphaPropertyValue.BoolValue;
		}
	}
	
	UMaterialInterface* MaterialInterface = Settings->GetMaterialForName(ShaderProperties->BaseShaderName, bUseTransparent);

	if (!ensure(MaterialInterface)) return true;
	
	FMaterialValue* MaterialValue = new FMaterialValue();
	MaterialValue->MaterialInterface = MaterialInterface;
	MaterialValue->ShaderProperties = ShaderProperties;
	
	WriteOutput("Material", FDynamicHandle::ForeignHandled(MaterialValue));

	return true;
}

void UBF::FMakeMaterialBase::AddColor(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
	const FString& PropertyName) const
{
	FString Property;
	if (!TryReadInputValue(ResourceName, Property)) return;

	FColor Color;
	if (UBFUtils::HexToColor(Property, Color))
	{
		PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeColor(Color));
	}
}

void UBF::FMakeMaterialBase::AddFloat(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
	const FString& PropertyName) const
{
	float Property;
	if (!TryReadInputValue<float>(ResourceName, Property)) return;

	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeFloat(Property));
}

void UBF::FMakeMaterialBase::AddInt(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
	const FString& PropertyName) const
{
	int Property;
	if (!TryReadInputValue<int>(ResourceName, Property)) return;

	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeInt(Property));
}

void UBF::FMakeMaterialBase::AddBool(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
	const FString& PropertyName) const
{
	bool Property;
	if (!TryReadInputValue<bool>(ResourceName, Property)) return;

	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeBool(Property));
}

void UBF::FMakeMaterialBase::AddTexture(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,
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

void UBF::FMakeMaterialBase::AddRenderMode(TMap<FString, FShaderPropertyValue>& PropertyMap,
	const FString& ResourceName, const FString& PropertyName) const
{
	FString Property;
	if (!TryReadInputValue(ResourceName, Property))
	{
		PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeInt(0));
		return;
	}
	
	const int RenderModeValue = Property.Equals(TEXT("UseDiffuse")) ? 0
		: Property.Equals(TEXT("SolidColor")) ? 1
		: 2;
	
	PropertyMap.Add(PropertyName, FShaderPropertyValue::MakeInt(RenderModeValue));
}
