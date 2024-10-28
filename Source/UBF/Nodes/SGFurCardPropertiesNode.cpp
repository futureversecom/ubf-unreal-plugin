// Fill out your copyright notice in the Description page of Project Settings.


#include "SGFurCardPropertiesNode.h"

void UBF::FSGFurCardPropertiesNode::AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const
{
	AddTexture(Properties, TEXT("Body Diffuse Tex"), TEXT("_BodyDiffuseTex"));
	AddTexture(Properties, TEXT("Body Fur AO Tex"), TEXT("_BodyFurAOTex"));
	AddTexture(Properties, TEXT("Fur Diffuse Tex"), TEXT("_FurDiffuseTex"));
	AddTexture(Properties, TEXT("Fur Normal Tex"), TEXT("_FurNormalTex"));
	AddTexture(Properties, TEXT("Fur ORM Tex"), TEXT("_FurOPMTex"));
	AddTexture(Properties, TEXT("Fur Alpha Tex"), TEXT("_FurAlphaTex"));
	AddFloat(Properties, TEXT("Fur Alpha Clip"), TEXT("_FurAlphaClip"));
	AddFloat(Properties, TEXT("Fur Tiling"), TEXT("_FurTiling"));
	AddFloat(Properties, TEXT("Fur Rotation"), TEXT("_FurRotation"));
	AddFloat(Properties, TEXT("Wind Falloff"), TEXT("_WindFalloff"));
	AddFloat(Properties, TEXT("Wind Falloff Mult"), TEXT("_WindFalloffMultiplier"));
	AddFloat(Properties, TEXT("Wind Speed"), TEXT("_WindSpeed"));
	AddFloat(Properties, TEXT("Wind Scale"), TEXT("_WindScale"));
	AddFloat(Properties, TEXT("Wind Strength"), TEXT("_WindStrength"));
	AddFloat(Properties, TEXT("Fresnel"), TEXT("_Fresnel"));
	AddFloat(Properties, TEXT("Fresnel Saturation"), TEXT("_FresnelSaturation"));
	AddFloat(Properties, TEXT("Fresnel Brightness"), TEXT("_FresnelFalloff"));
	AddFloat(Properties, TEXT("Fresnel Falloff"), TEXT("_FresnelFalloff"));
	AddFloat(Properties, TEXT("Fresnel Falloff Mult"), TEXT("_FresnelFalloffMultiply"));
}
