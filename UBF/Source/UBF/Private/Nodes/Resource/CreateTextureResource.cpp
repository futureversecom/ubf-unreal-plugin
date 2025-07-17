#include "Nodes/Resource/CreateTextureResource.h"

#include "JsonObjectConverter.h"
#include "GlobalArtifactProvider/CatalogElement.h"
#include "GlobalArtifactProvider/ImportSettings/TextureArtifactImportSettings.h"

namespace UBF
{
	void FCreateTextureResource::ExecuteAsync() const
	{
		FString URI;
		if (!TryReadInputValue(TEXT("URI"), URI))
		{
			UBF_LOG(Error, TEXT("[CreateTextureResource] Failed to read 'URI' input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		// Currently unused, but will be added to TextureArtifactImportSettings in a future update
		bool bUseSRGB;
		if (!TryReadInputValue<bool>(TEXT("Use SRGB"), bUseSRGB))
		{
			UBF_LOG(Error, TEXT("[CreateTextureResource] Failed to read 'Use SRGB' input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		const auto Settings = FTextureArtifactImportSettings(bUseSRGB);
	
		FString SettingsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Settings, SettingsJson);
		
		FJsonObjectWrapper SettingsJsonObjectWrapper;
		SettingsJsonObjectWrapper.JsonString = SettingsJson;

		// todo: generate id using uri + import settings?
		FCatalogElement CatalogElement;
		CatalogElement.Id = URI;
		CatalogElement.Uri = URI;
		CatalogElement.Type = TEXT("texture");
		CatalogElement.Hash = URI;
		CatalogElement.MetadataJsonWrapper = SettingsJsonObjectWrapper;

		UBF_LOG(Verbose, TEXT("[CreateTextureResource] Registering Runtime Catalog: %s"), *CatalogElement.ToString());
		GetContext().GetSetConfig()->RegisterRuntimeCatalog(CatalogElement);
		
		WriteOutput(TEXT("Resource"), FDynamicHandle::String(CatalogElement.Id));
		TriggerNext();
		CompleteAsyncExecution();
	}
}

