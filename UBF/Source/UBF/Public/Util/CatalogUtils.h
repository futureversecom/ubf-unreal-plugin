#pragma once
#include "UBFLog.h"
#include "GlobalArtifactProvider/CatalogElement.h"

namespace CatalogUtils
{
	const FString Resources = TEXT("resources");
	const FString Id = TEXT("id");
	const FString Uri = TEXT("uri");
	const FString Hash = TEXT("hash");
	
	inline void ParseCatalog(const FString& Json, TMap<FString, UBF::FCatalogElement>& CatalogElementMap)
	{
		// Create a shared pointer to hold the JSON object
		TSharedPtr<FJsonObject> JsonObject;

		// Create a JSON reader
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);

		// Deserialize the JSON data into the JsonObject
		if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			UE_LOG(LogUBF, Warning, TEXT("UBFUtils::ParseCatalog Failed to parse JSON string\n %s."), *Json);
			return;
		}
		
		// Get the "resources" array from the JSON
		TArray<TSharedPtr<FJsonValue>> ResourcesArray = JsonObject->GetArrayField(Resources);

		// Iterate over each element in the resources array
		for (const TSharedPtr<FJsonValue>& Value : ResourcesArray)
		{
			// Each element is a JSON object, get it as such
			TSharedPtr<FJsonObject> ResourceObject = Value->AsObject();
			if (ResourceObject.IsValid())
			{
				if (!ResourceObject->HasField(Id)
					|| !ResourceObject->HasField(Uri)
					|| !ResourceObject->HasField(Hash))
				{
					UE_LOG(LogUBF, Error, TEXT("Cannot parse Json, missing %s, %s or %s field in element %s in json %s"),
						*Id, *Uri, *Hash, *Value->AsString(), *Json);
					continue;
				}
				UBF::FCatalogElement CatalogElement;
				CatalogElement.Id = ResourceObject->GetStringField(Id);
				CatalogElement.Uri = ResourceObject->GetStringField(Uri);
				CatalogElement.Hash = ResourceObject->GetStringField(Hash);
				CatalogElementMap.Add(CatalogElement.Id, CatalogElement);
				UE_LOG(LogUBF, VeryVerbose, TEXT("AssetProfileUtils::ParseCatalog "
					"Added CatalogElement %s: %s %s: %s %s: %s"),
					*Id, *CatalogElement.Id, *Uri, *CatalogElement.Uri, *Hash, *CatalogElement.Hash);
			}
		}
	}
};
