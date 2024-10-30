#pragma once

namespace UBF
{
	struct FShaderPropertyValue;

	class FShaderProperties
	{
	public:
		FName BaseShaderName;
		TMap<FString, FShaderPropertyValue> Properties;
	};

	class FMaterialValue
	{
	public:
		FShaderProperties* ShaderProperties;
		UMaterialInterface* MaterialInterface;
	};
}

