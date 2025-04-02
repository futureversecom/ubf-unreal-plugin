// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

namespace UBF
{
	class FMaterialValue
	{
	public:
		class FShaderProperties* ShaderProperties;
		UMaterialInterface* MaterialInterface;
	};
	
	enum class EShaderPropertyType : uint8
	{
		Boolean,
		Int,
		Float,
		Color,
		Texture
	};
	
	struct FTextureHandle
	{
		FTextureHandle(): IsNormal(false) {}
        
		FString ResourceId;
		bool IsNormal;
	};
	
	struct FShaderPropertyValue
	{
		FShaderPropertyValue(): PropertyType(), BoolValue(false), IntValue(0), FloatValue(0){}

		EShaderPropertyType PropertyType;
		
		bool BoolValue;
		int IntValue;
		float FloatValue;
		FColor ColorValue;
		FTextureHandle TextureValue;

		static FShaderPropertyValue MakeBool(bool InBoolValue)
		{
			FShaderPropertyValue PropertyValue;
			PropertyValue.PropertyType = EShaderPropertyType::Boolean;
			PropertyValue.BoolValue = InBoolValue;
			return PropertyValue;
		}

		static FShaderPropertyValue MakeInt(int InIntValue)
		{
			FShaderPropertyValue PropertyValue;
			PropertyValue.PropertyType = EShaderPropertyType::Int;
			PropertyValue.IntValue = InIntValue;
			return PropertyValue;
		}

		static FShaderPropertyValue MakeFloat(float InFloatValue)
		{
			FShaderPropertyValue PropertyValue;
			PropertyValue.PropertyType = EShaderPropertyType::Float;
			PropertyValue.FloatValue = InFloatValue;
			return PropertyValue;
		}
        
		static FShaderPropertyValue MakeColor(FColor InColorValue)
		{
			FShaderPropertyValue PropertyValue;
			PropertyValue.PropertyType = EShaderPropertyType::Color;
			PropertyValue.ColorValue = InColorValue;
			return PropertyValue;
		}

		static FShaderPropertyValue MakeTexture(const FTextureHandle& InTexture)
		{
			FShaderPropertyValue PropertyValue;
			PropertyValue.PropertyType = EShaderPropertyType::Texture;
			PropertyValue.TextureValue = InTexture;
			return PropertyValue;
		}
	};

	class FShaderProperties
	{
	public:
		FName BaseShaderName;
		TMap<FString, FShaderPropertyValue> Properties;
	};
}

