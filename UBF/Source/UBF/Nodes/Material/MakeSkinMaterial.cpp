#include "MakeSkinMaterial.h"

namespace UBF
{
	void FMakeSkinMaterial::AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const
	{
		AddTexture(Properties, TEXT("GCLS Tex"), TEXT("_GCLSTex"));
		AddTexture(Properties, TEXT("Normal Tex"), TEXT("_NormalTex"), true);
		AddTexture(Properties, TEXT("ORS Tex"), TEXT("_ORSTex"));
	
		AddColor(Properties, TEXT("Skin Color"), TEXT("_SkinColor"));
		AddColor(Properties, TEXT("Redness"), TEXT("_RednessColor"));
		AddColor(Properties, TEXT("Lip Color"), TEXT("_LipsColor"));
		AddColor(Properties, TEXT("Skin Variation"), TEXT("_SkinVariation"));
		
		AddFloat(Properties, TEXT("Dark Area Hue"), TEXT("_SkinColorDark_Hue"));
		AddFloat(Properties, TEXT("Dark Area Saturation"), TEXT("_SkinColorDark_Saturate"));
		AddFloat(Properties, TEXT("Dark Area Value"), TEXT("_SkinColorDark_Value"));
		AddFloat(Properties, TEXT("Roughness Strength"), TEXT("_RoughnessStrength"));
		AddFloat(Properties, TEXT("AO Strength"), TEXT("_AOStrength"));
		
		AddTexture(Properties, TEXT("Buzzcut Tex"), TEXT("Buzzcut Tex"));
		AddTexture(Properties, TEXT("Stubble Tex"), TEXT("_StubbleTex"));
		AddTexture(Properties, TEXT("Freckles Tex"), TEXT("_FrecklesTex"));
		AddTexture(Properties, TEXT("Moles Tex"), TEXT("_MolesTex"));
		AddTexture(Properties, TEXT("Scars Tex"), TEXT("_ScarsTex"));
		
		AddColor(Properties, TEXT("Hair Color"), TEXT("_HairColor"));
		AddColor(Properties, TEXT("Freckle Color"), TEXT("_FreckleColor"));
		AddColor(Properties, TEXT("Mole Color"), TEXT("_MoleColor"));
		AddColor(Properties, TEXT("Scar Color"), TEXT("_ScarColor"));
		
		AddFloat(Properties, TEXT("Stubble Growth"), TEXT("_StubbleGrowth"));
		AddFloat(Properties, TEXT("Stubble Contrast"), TEXT("_StubbleContrast"));
		AddFloat(Properties, TEXT("Mole Normal Strength"), TEXT("_MoleNormalStrength"));
		AddFloat(Properties, TEXT("Scar Color Contrast"), TEXT("_ScarColorContrast"));
		AddFloat(Properties, TEXT("Scar Color Falloff"), TEXT("_ScarColorFalloff"));
		AddFloat(Properties, TEXT("Scar Normal Strength"), TEXT("_ScarNormalStrength"));
	}
}

