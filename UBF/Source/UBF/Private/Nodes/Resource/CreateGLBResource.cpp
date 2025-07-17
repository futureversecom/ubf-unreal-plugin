#include "Nodes/Resource/CreateGLBResource.h"

#include "JsonObjectConverter.h"
#include "GlobalArtifactProvider/CatalogElement.h"
#include "GlobalArtifactProvider/ImportSettings/MeshArtifactImportSettings.h"

namespace UBF
{
	void FCreateGLBResource::ExecuteAsync() const
	{
		FString URI;
		if (!TryReadInputValue(TEXT("URI"), URI))
		{
			UBF_LOG(Error, TEXT("[CreateGLBResource] Failed to read 'URI' input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		const auto Settings = FMeshArtifactImportSettings();
	
		FString SettingsJson;
		FJsonObjectConverter::UStructToJsonObjectString(Settings, SettingsJson);
		
		FJsonObjectWrapper SettingsJsonObjectWrapper;
		SettingsJsonObjectWrapper.JsonString = SettingsJson;
		
		FCatalogElement CatalogElement;
		CatalogElement.Id = FString::Printf(TEXT("%s_%s"), *URI, *Settings.MeshIdentifier);
		CatalogElement.Uri = URI;
		CatalogElement.Type = TEXT("glb");
		CatalogElement.Hash = FMD5::HashAnsiString(*URI);
		CatalogElement.MetadataJsonWrapper = SettingsJsonObjectWrapper;

		UBF_LOG(Verbose, TEXT("[CreateGLBResource] Registering Runtime Catalog: %s"), *CatalogElement.ToString());
		GetContext().GetSetConfig()->RegisterRuntimeCatalog(CatalogElement);
		
		WriteOutput(TEXT("Resource"), FDynamicHandle::String(CatalogElement.Id));
		TriggerNext();
		CompleteAsyncExecution();
	}
}

