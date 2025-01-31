#include "ApplyMaterialNode.h"

#include "GraphProvider.h"
#include "SetTextureSettingsNode.h"
#include "SGPropertiesBase.h"
#include "UBFLog.h"
#include "UBFValues.h"
#include "DataTypes/MeshRenderer.h"

namespace UBF
{
	void FApplyMaterialNode::ExecuteAsync() const
	{
		UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Executing Node"));
		
		FMeshRenderer* Renderer;
		if (!TryReadInput("Renderer", Renderer))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Failed to Read Renderer Input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		const FMaterialValue* Material;
		if (!TryReadInput("Material", Material))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Failed to Read Material Input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		int ElementIndex = 0;
		if (!TryReadInputValue("Index", ElementIndex))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Failed to Read Index"));
		}
		
		WorkingMeshRenderer = Renderer->GetMesh();
		if (!IsValid(WorkingMeshRenderer))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] cannot find valid WorkingMeshRenderer"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		if (!IsValid(Material->MaterialInterface))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Material does not have valid MaterialInterface"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		FName DynamicMaterialName = FName(Material->MaterialInterface->GetName() + TEXT("_Dynamic"));
		// TODO this can be GC because there is no UPROPERTY() ref. Need to add to root or something
		WorkingMaterialInstance = UMaterialInstanceDynamic::Create(Material->MaterialInterface, GetWorld(), DynamicMaterialName);
		WorkingMeshRenderer->SetMaterial(ElementIndex, WorkingMaterialInstance.Get());
		
		auto OnNext = [this](bool bWasSuccess){CheckFuturesComplete(bWasSuccess);};

		AwaitingFutures = 1; // Prevent futures from completing before iteration over
		
		for (auto Property : Material->ShaderProperties->Properties)
		{
			AwaitingFutures++;
			EvaluateProperty(Property, WorkingMaterialInstance).Next(OnNext);
		}
		
		// In-case futures triggered instantly
		CheckFuturesComplete(true);
	}

	void FApplyMaterialNode::CheckFuturesComplete(bool bWasSuccessful) const
	{
		AwaitingFutures--;

		if (AwaitingFutures > 0) return;
		// if game shuts down while still waiting, it will be invalid
		if (!WorkingMaterialInstance.Get() || !WorkingMeshRenderer)
		{
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Material %s to MeshComponent %s"),*WorkingMaterialInstance->GetName(), *WorkingMeshRenderer->GetName());
		
		TriggerNext();
		CompleteAsyncExecution();
	}

	TFuture<bool> FApplyMaterialNode::EvaluateProperty(const TTuple<FString, FShaderPropertyValue>& Prop,
	                                                   TWeakObjectPtr<UMaterialInstanceDynamic> Mat) const
	{
		TSharedPtr<TPromise<bool>> Promise = MakeShareable(new TPromise<bool>());
		TFuture<bool> Future = Promise->GetFuture();
		
		if (Prop.Value.PropertyType == EShaderPropertyType::Int)
		{
			UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Int Property: %s With Value: %d to Material"), *Prop.Key, Prop.Value.IntValue);
			
			float TestValue;
			if (!Mat->GetScalarParameterValue(*Prop.Key,TestValue))
			{
				UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Int Property: %s not found from Material"), *Prop.Key);
			}
			
			Mat->SetScalarParameterValue(*Prop.Key, Prop.Value.IntValue);
		}
		else if (Prop.Value.PropertyType == EShaderPropertyType::Boolean)
		{
			const auto Value = Prop.Value.BoolValue ? 1.0f : 0.0f;
			UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Bool Property: %s With Value: %f to Material"), *Prop.Key, Value);
			
			float TestValue;
			if (!Mat->GetScalarParameterValue(*Prop.Key,TestValue))
			{
				UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Bool Property: %s not found from Material"), *Prop.Key);
			}
			
			Mat->SetScalarParameterValue(*Prop.Key, Value);
		}
		else if (Prop.Value.PropertyType == EShaderPropertyType::Float)
		{
			UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Float Property: %s With Value: %f to Material"), *Prop.Key, Prop.Value.FloatValue);

			float TestValue;
			if (!Mat->GetScalarParameterValue(*Prop.Key,TestValue))
			{
				UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Float Property: %s not found from Material"), *Prop.Key);
			}
			
			Mat->SetScalarParameterValue(*Prop.Key, Prop.Value.FloatValue);
		}
		else if (Prop.Value.PropertyType == EShaderPropertyType::Color)
		{
			UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Color Property: %s With Value: %s to Material"), *Prop.Key, *Prop.Value.ColorValue.ToHex());
			
			FLinearColor TestValue;
			if (!Mat->GetVectorParameterValue(*Prop.Key,TestValue))
			{
				UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Color Property: %s not found from Material"), *Prop.Key);
			}
			
			Mat->SetVectorParameterValue(*Prop.Key, Prop.Value.ColorValue);
		}
		else if (Prop.Value.PropertyType == EShaderPropertyType::Texture)
		{
			UTexture* TestValue;
			if (!Mat->GetTextureParameterValue(*Prop.Key,TestValue))
			{
				UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Texture Property: %s not found from Material"), *Prop.Key);
				Promise->SetValue(false);
				return Future;
			}

			const auto TextureHandle = Prop.Value.TextureValue;
			FString ResourceId = TextureHandle.ResourceId;
			GetContext().GetGraphProvider()->GetTextureResource(TextureHandle.ResourceId).Next(
				[this, Promise, Mat, Prop, ResourceId](const FLoadTextureResult& TextureResult)
			{
				if (!TextureResult.Result.Key)
				{
					UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Texture Property failed because Texture download failed"));
					Promise->SetValue(false);
					return;
				}
				
				UTexture* Texture = TextureResult.Result.Value;
				if (Texture == nullptr)
				{
					UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Texture Property failed because Texture was null"));
					Promise->SetValue(false);
					return;
				}

				if (!IsValid(Mat.Get()))
				{
					UE_LOG(LogUBF, Warning, TEXT("[ApplyMaterial] Applying Texture Property failed because Mat was no longer valid"));
					Promise->SetValue(false);
					return;
				}

				FDynamicHandle DynamicHandle;
				if (GetContext().GetDynamicDataEntry(ResourceId, DynamicHandle))
				{
					FTextureSettings* TextureSettings = nullptr;
									
					if (DynamicHandle.TryInterpretAs(TextureSettings) && TextureSettings)
					{
						Texture->SRGB = TextureSettings->bUseSRGB;
						Texture->UpdateResource();
					}
				}
				
				UE_LOG(LogUBF, Verbose, TEXT("[ApplyMaterial] Applying Texture Property: %s With Value: %s to Material"), *Prop.Key,
					Texture ? *Texture->GetName() : TEXT("Null"));
				Mat->SetTextureParameterValue(*Prop.Key, Texture);
				Promise->SetValue(true);
			});

			return Future;
		}

		Promise->SetValue(true);
		return Future;
	}


}
