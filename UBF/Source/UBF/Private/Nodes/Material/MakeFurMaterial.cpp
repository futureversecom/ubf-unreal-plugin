#include "Nodes/Material/MakeFurMaterial.h"

namespace UBF
{
	void FMakeFurMaterial::AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const
	{
		AddInt(Properties, TEXT("Render Mode"), TEXT("_RenderMode"));
		AddTexture(Properties, TEXT("Diffuse Texture"), TEXT("_DiffuseTex"));
		AddColor(Properties, TEXT("Base Color"), TEXT("_Tint"));
		AddBool(Properties, TEXT("Use Alpha"), TEXT("_UseAlpha"));
	
		AddFloat(Properties, TEXT("Fresnel_IOR"), TEXT("_Fresnel_IOR"));
		AddFloat(Properties, TEXT("Opacity"), TEXT("_Opacity"));

		AddBool(Properties, TEXT("Use Emission"), TEXT("_UseEmission"));
		AddBool(Properties, TEXT("Use Emissive Tint"), TEXT("_UseEmissiveTint"));
		AddTexture(Properties, TEXT("Emissive Tex"), TEXT("_EmissiveTex"));
		AddFloat(Properties, TEXT("Emissive Color Boost"), TEXT("_EmissiveColorBoost"));
		AddColor(Properties, TEXT("Emissive Tint"), TEXT("_EmissiveTint"));
		AddFloat(Properties, TEXT("Emissive Tint Boost"), TEXT("_EmissiveTintBoost"));

		AddBool(Properties, TEXT("Use Normal Map"), TEXT("_UseNormal"));
		AddTexture(Properties, TEXT("Normal Tex"), TEXT("_NormalTex"), true);
		AddBool(Properties, TEXT("Use ORM"), TEXT("_UseORM"));
		AddTexture(Properties, TEXT("ORM"), TEXT("_ORM"));
		AddFloat(Properties, TEXT("Occlusion"), TEXT("_Occlusion"));
		AddFloat(Properties, TEXT("Roughness"), TEXT("_Roughness"));
		AddFloat(Properties, TEXT("Metallic"), TEXT("_Metallic"));
		AddTexture(Properties, TEXT("Height Map"), TEXT("_HeightMap"));
		AddTexture(Properties, TEXT("Id Map"), TEXT("_IdMap"));
	}
}

