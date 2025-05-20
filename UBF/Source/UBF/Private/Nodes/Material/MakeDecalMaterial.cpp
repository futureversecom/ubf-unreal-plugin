#include "Nodes/Material/MakeDecalMaterial.h"

namespace UBF
{
	void FMakeDecalMaterial::AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const
	{
		AddInt(Properties, TEXT("Render Mode"), TEXT("_RenderMode"));
		AddColor(Properties, TEXT("Base Color"), TEXT("_Tint"));
		AddBool(Properties, TEXT("Use Alpha"), TEXT("_UseAlpha"));
		AddBool(Properties, TEXT("Use Alpha Tex"), TEXT("_UseAlphaTexture"));
		AddFloat(Properties, TEXT("Fresnel_IOR"), TEXT("_Fresnel_IOR"));
		AddFloat(Properties, TEXT("Opacity"), TEXT("_Opacity"));
		AddTexture(Properties, TEXT("AlphaTex"), TEXT("_AlphaTex"));
		
		AddBool(Properties, TEXT("Use Emission"), TEXT("_UseEmission"));
		AddBool(Properties, TEXT("Use Emissive Tint"), TEXT("_UseEmissiveTint"));
		AddTexture(Properties, TEXT("Emissive Tex"), TEXT("_EmissiveTex"));
		AddFloat(Properties, TEXT("Emissive Color Boost"), TEXT("_EmissiveColorBoost"));
		AddColor(Properties, TEXT("Emissive Tint"), TEXT("_EmissiveTint"));
		AddFloat(Properties, TEXT("Emissive Tint Boost"), TEXT("_EmissiveTintBoost"));
		
		AddBool(Properties, TEXT("Use Normal Map"), TEXT("_UseNormal"));
		AddBool(Properties, TEXT("Use ORM"), TEXT("_UseORM"));
		AddFloat(Properties, TEXT("Occlusion"), TEXT("_Occlusion"));
		AddFloat(Properties, TEXT("Roughness"), TEXT("_Roughness"));
		AddFloat(Properties, TEXT("Metallic"), TEXT("_Metallic"));
		AddBool(Properties, TEXT("Use Decals"), TEXT("_UseDecals"));
		AddColor(Properties, TEXT("Tint Base"), TEXT("_TintBase"));
		AddFloat(Properties, TEXT("Darken Base"), TEXT("_DarkenBase"));
		AddFloat(Properties, TEXT("Metal Base"), TEXT("_MetalBase"));
		AddFloat(Properties, TEXT("Flakes Base"), TEXT("_FlakesBase"));
		AddColor(Properties, TEXT("Tint A"), TEXT("_TintA"));
		AddFloat(Properties, TEXT("Darken A"), TEXT("_DarkenA"));
		AddFloat(Properties, TEXT("Metal A"), TEXT("_MetalA"));
		AddFloat(Properties, TEXT("Flakes A"), TEXT("_FlakesA"));
		AddColor(Properties, TEXT("Tint B"), TEXT("_TintB"));
		AddFloat(Properties, TEXT("Darken B"), TEXT("_DarkenB"));
		AddFloat(Properties, TEXT("Metal B"), TEXT("_MetalB"));
		AddFloat(Properties, TEXT("Flakes B"), TEXT("_FlakesB"));
		AddColor(Properties, TEXT("Tint C"), TEXT("_TintC"));
		AddFloat(Properties, TEXT("Darken C"), TEXT("_DarkenC"));
		AddFloat(Properties, TEXT("Metal C"), TEXT("_MetalC"));
		AddFloat(Properties, TEXT("Flakes C"), TEXT("_FlakesC"));
		
		AddTexture(Properties, TEXT("Diffuse Texture"), TEXT("_DiffuseTex"));
		AddTexture(Properties, TEXT("ORM"), TEXT("_ORMTex"));
		AddTexture(Properties, TEXT("Decal Tex"), TEXT("_DecalTex"));
		AddTexture(Properties, TEXT("Normal Tex"), TEXT("_NormalTex"), true);
	}
}
