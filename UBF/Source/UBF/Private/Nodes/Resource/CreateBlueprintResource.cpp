#include "Nodes/Resource/CreateBlueprintResource.h"

#include "JsonObjectConverter.h"
#include "GlobalArtifactProvider/CatalogElement.h"
#include "GlobalArtifactProvider/ImportSettings/BlueprintArtifactImportSettings.h"

namespace UBF
{
	void FCreateBlueprintResource::ExecuteAsync() const
	{
		FString URI;
		if (!TryReadInputValue(TEXT("URI"), URI))
		{
			UBF_LOG(Error, TEXT("[CreateBlueprintResource] Failed to read 'URI' input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		const auto Settings = FBlueprintArtifactImportSettings();
	
		FString SettingsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Settings, SettingsJson);
		
		FJsonObjectWrapper SettingsJsonObjectWrapper;
		SettingsJsonObjectWrapper.JsonString = SettingsJson;
		
		FCatalogElement CatalogElement;
		CatalogElement.Id = URI;
		CatalogElement.Uri = URI;
		CatalogElement.Type = TEXT("blueprint");
		CatalogElement.MetadataJsonWrapper = SettingsJsonObjectWrapper;

		UBF_LOG(Verbose, TEXT("[CreateBlueprintResource] Registering Runtime Catalog: %s"), *CatalogElement.ToString());
		GetContext().GetSetConfig()->RegisterRuntimeCatalog(CatalogElement);
		
		WriteOutput(TEXT("Resource"), FDynamicHandle::String(CatalogElement.Id));
		TriggerNext();
		CompleteAsyncExecution();
	}
}

