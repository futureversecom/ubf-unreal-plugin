#include "MaterialNode.h"

#include "SGPropertiesBase.h"
#include "UBFMaterialSettings.h"
#include "UBFValues.h"
#include "UBFLog.h"

namespace UBF
{
	bool FMaterialNode::ExecuteSync() const
	{
		FShaderProperties* ShaderProperties;
		if (!TryReadInput<FShaderProperties>("Properties", ShaderProperties))
		{
			UBF_LOG(Warning, TEXT("Material node could not find input 'Properties'"));
			return true;
		}

		check(ShaderProperties);

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

		// Don't need to trigger next on material it has no exec output
		return true;
	}
}


