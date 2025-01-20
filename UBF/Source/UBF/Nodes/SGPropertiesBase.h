#pragma once

#include "CustomNode.h"

namespace UBF
{
    enum class EShaderPropertyType : uint8
    {
        Boolean,
        Int,
        Float,
        Color,
        Texture
    };

    inline bool HexToColor(const FString& HexString, FColor& OutColor)
    {
        FString CleanHexString = HexString.Replace(TEXT("#"), TEXT("")).TrimStartAndEnd();

        if (CleanHexString.Len() == 6 || CleanHexString.Len() == 8)
        {
            // Extract RGB(A) values from the hex string
            FString RHex = CleanHexString.Mid(0, 2);
            FString GHex = CleanHexString.Mid(2, 2);
            FString BHex = CleanHexString.Mid(4, 2);
            FString AHex = CleanHexString.Len() == 8 ? CleanHexString.Mid(6, 2) : TEXT("FF");

            // Convert hex strings to integers
            int32 Red = FParse::HexNumber(*RHex);
            int32 Green = FParse::HexNumber(*GHex);
            int32 Blue = FParse::HexNumber(*BHex);
            int32 Alpha = FParse::HexNumber(*AHex);

            OutColor = FColor(Red, Green, Blue, Alpha);
            return true;
        }

        // Return black if the string is invalid
        return false;
    }

    struct FTextureHandle
    {
        FTextureHandle(): IsNormal(false) {}
        
        FString ResourceId;
        bool IsNormal;
    };
    
    struct FShaderPropertyValue
    {
        FShaderPropertyValue(): PropertyType(), BoolValue(false), IntValue(0), FloatValue(0)
        {
        }

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

    
	class FSGPropertiesBase : public FCustomNode
	{
    public:
        virtual bool ExecuteSync() const override;

    protected:
        virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const = 0;
        virtual FName GetBaseShaderName() const = 0;

        void AddColor(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddFloat(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddInt(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddBool(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddTexture(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,const FString& PropertyName, bool bIsNormal = false) const;
	};
}

