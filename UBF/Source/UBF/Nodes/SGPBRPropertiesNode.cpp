#include "SGPBRPropertiesNode.h"

void UBF::FSGPBRPropertiesNode::AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const
{
	AddBool(Properties, TEXT("Use Diffuse"), TEXT("_USEDIFFUSE"));
	AddTexture(Properties, TEXT("Diffuse Texture"), TEXT("_DiffuseTex"));
	AddColor(Properties, TEXT("Base Color"), TEXT("_BaseColor"));
	AddBool(Properties, TEXT("Use Alpha"), TEXT("_UseAlpha"));
	AddFloat(Properties, TEXT("Fresnel_IOR"), TEXT("_Fresnel_IOR"));
	AddFloat(Properties, TEXT("Opacity"), TEXT("_Opacity"));

	AddBool(Properties, TEXT("Use Emission"), TEXT("_USEEMISSION"));
	AddBool(Properties, TEXT("Use Emissive Tint"), TEXT("_UseEmissiveTint"));
	AddTexture(Properties, TEXT("Emissive Tex"), TEXT("_EmissiveTex"));
	AddFloat(Properties, TEXT("Emissive Color Boost"), TEXT("_EmissiveColorBoost"));
	AddColor(Properties, TEXT("Emissive Tint"), TEXT("_EmissiveTint"));
	AddFloat(Properties, TEXT("Emissive Tint Boost"), TEXT("_EmissiveTintBoost"));

	AddBool(Properties, TEXT("Use Normal Map"), TEXT("_USENORMALMAP"));
	AddTexture(Properties, TEXT("Normal Tex"), TEXT("_NormalTex"), true);
	AddBool(Properties, TEXT("Use ORM"), TEXT("_USEORM"));
	AddTexture(Properties, TEXT("ORM"), TEXT("_ORMTex"));
	AddFloat(Properties, TEXT("Occlusion"), TEXT("_Occlusion"));
	AddFloat(Properties, TEXT("Roughness"), TEXT("_Roughness"));
	AddFloat(Properties, TEXT("Metallic"), TEXT("_Metallic"));
}
