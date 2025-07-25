#pragma once
#include "UBFLog.h"
#include "GlobalArtifactProvider/CatalogElement.h"

namespace CatalogUtils
{
	const FString ResourcesField = TEXT("resources");
	const FString IdField = TEXT("id");
	const FString TypeField = TEXT("type");
	const FString UriField = TEXT("uri");
	const FString HashField = TEXT("hash");
	const FString MetadataField = TEXT("metadata");
	
	inline void ParseCatalog(const FString& Json, TMap<FString, UBF::FCatalogElement>& CatalogElementMap, const FString& RelativePath = "")
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
		TArray<TSharedPtr<FJsonValue>> ResourcesArray = JsonObject->GetArrayField(ResourcesField);

		// Iterate over each element in the resources array
		for (const TSharedPtr<FJsonValue>& Value : ResourcesArray)
		{
			// Each element is a JSON object, get it as such
			TSharedPtr<FJsonObject> ResourceObject = Value->AsObject();
			if (ResourceObject.IsValid())
			{
				if (!ResourceObject->HasField(IdField)
					|| !ResourceObject->HasField(UriField)
					|| !ResourceObject->HasField(HashField))
				{
					UE_LOG(LogUBF, Error, TEXT("Cannot parse Json, missing %s, %s or %s field in element %s in json %s"),
						*IdField, *UriField, *HashField, *Value->AsString(), *Json);
					continue;
				}
				UBF::FCatalogElement CatalogElement;
				CatalogElement.Id = ResourceObject->GetStringField(IdField);
				
				FString Type;
				ResourceObject->TryGetStringField(TypeField, Type);
				CatalogElement.Type = Type;
				
				FString Uri = ResourceObject->GetStringField(UriField);
				if (!RelativePath.IsEmpty() && Uri.StartsWith(TEXT("./")))
				{
					// studio generates the catalogs in {./path} format
					FString LeftString;
					FString NewPath;
					Uri.Split(".", &LeftString, &NewPath, ESearchCase::Type::IgnoreCase);
					Uri = RelativePath + NewPath;
				}

				FJsonObjectWrapper MetadataJsonWrapper;
				FString MetadataJson;
				if (const TSharedPtr<FJsonObject>* MetadataObject; ResourceObject->TryGetObjectField(MetadataField, MetadataObject))
				{
					const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&MetadataJson);
					FJsonSerializer::Serialize(MetadataObject->ToSharedRef(), Writer);
				}
				MetadataJsonWrapper.JsonString = MetadataJson;
				CatalogElement.MetadataJsonWrapper = MetadataJsonWrapper;
				
				CatalogElement.Uri = Uri;
				CatalogElement.Hash = ResourceObject->GetStringField(HashField);
				CatalogElementMap.Add(CatalogElement.Id, CatalogElement);
				UE_LOG(LogUBF, VeryVerbose, TEXT("AssetProfileUtils::ParseCatalog "
					"Added CatalogElement: %s"),  *CatalogElement.ToString());
			}
		}
	}
};
