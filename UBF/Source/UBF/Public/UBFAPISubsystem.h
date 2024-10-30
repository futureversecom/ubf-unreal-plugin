// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Registry.h"
#include "UBFRuntimeController.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UBFAPISubsystem.generated.h"

class IHttpResponse;
class UUBFGraphReference;

struct FResourceManifestElement
{
	FString Path;
	FString Hash;

	// Declare serialization function for this struct
	bool Serialize(const TSharedPtr<FJsonObject>& JsonObject)
	{
		if (!JsonObject.IsValid())
		{
			return false;
		}

		JsonObject->TryGetStringField(TEXT("Path"), Path);
		JsonObject->TryGetStringField(TEXT("Hash"), Hash);

		return true;
	}
};

class FResourceManifest
{
public:
	TArray<FResourceManifestElement> ResourceManifestElements;
	
	bool TryGetResourceManifestElement(const FString& ManifestPath, FResourceManifestElement& ManifestElement)
	{
		for (auto& ResourceManifestElement : ResourceManifestElements)
		{
			if (ManifestPath.Contains(ResourceManifestElement.Path))
			{
				ManifestElement = ResourceManifestElement;
				return true;
			}
		}
		return false;
	}

	static TSharedPtr<FResourceManifest> FromJSON(const FString& Json)
	{
		TSharedPtr<FResourceManifest> ResourceManifest = MakeShared<FResourceManifest>();
		TSharedPtr<FJsonObject> JsonObject;

		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			TArray<TSharedPtr<FJsonValue>> JsonArray = JsonObject->GetArrayField(TEXT("manifest"));
			for (auto& Item : JsonArray)
			{
				TSharedPtr<FJsonObject> ElementObject = Item->AsObject();
				FResourceManifestElement Element;
				if (Element.Serialize(ElementObject))
				{
					ResourceManifest->ResourceManifestElements.Add(Element);
				}
			}
		}

		return ResourceManifest;
	}
};

/**
 * 
 */
UCLASS()
class UBF_API UUBFAPISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UUBFAPISubsystem* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable)
	void UpdateGraphsFromLocalURI(const FString& Location, const FOnComplete& OnComplete);
	
	UFUNCTION(BlueprintCallable)
	void GetLoadedGraphs(TArray<UUBFGraphReference*>& Graphs) const { Graphs = LoadedGraphs; }
	
	UFUNCTION(BlueprintCallable)
	UUBFGraphReference* FindLoadedGraph(const FString& GraphId);
	
	UFUNCTION(BlueprintCallable)
	void ClearLoadedGraphs() { LoadedGraphs.Empty(); }
	
	TFuture<bool> UpdateManifestAsync(const FString& BlueprintId);
	bool ManifestRequiresUpdate(const FString& BlueprintId);
	TSharedPtr<FResourceManifest> GetManifest(const FString& BlueprintId);
	
	void SaveResourceHash(const FString& BlueprintId, const FString& ResourceId, const FString& Hash);
	FString GetResourceHash(const FString& BlueprintId, const FString& ResourceId);
	TFuture<bool> DownloadGraphAsync(const FString& GraphURI);

	static TFuture<TSharedPtr<IHttpResponse>> MakeDownloadRequest(const FString& Path);
	
	UPROPERTY(BlueprintReadWrite)
	FString ServerLocation = TEXT("http://localhost:8080/Resources/");
	UPROPERTY(BlueprintReadWrite)
	float ManifestUpdateFrequency = 3;
private:
	TFuture<FString> DownloadManifestAsync(const FString& GraphID);

	UPROPERTY()
	TArray<UUBFGraphReference*> LoadedGraphs;
	UPROPERTY()
	TArray<UUBFGraphReference*> LoadingGraphs;
	
	TMap<FString, TSharedPtr<FResourceManifest>> ResourceManifestMap;
	TMap<FString, FDateTime> ResourceManifestRecordMap;
	TMap<FString, FString> ResourceHashesMap;
};

UCLASS()
class UBF_API UUBFGraphReference : public UObject
{
	GENERATED_BODY()
public:
	TFuture<bool> InitAsync(FString GraphURI, const FString& ServerLocation)
	{
		URIRef = GraphURI;
		URI = ServerLocation + GraphURI + TEXT("/ubf.json");

		return DownloadGraphDataAsync();
	}
	
	FString GetResourceURI() const
	{
		return URI.Replace(TEXT("ubf.json"), TEXT("Assets/Resources/"));
	}

	FString GetURI() const
	{
		return URI;
	}

	UFUNCTION(BlueprintCallable)
	FString GetBlueprintId() const
	{
		return BlueprintId;
	}
	
	bool TryGetGraph(UBF::FGraphHandle& Graph) const;
	
private:
	TFuture<bool> DownloadGraphDataAsync();
	
	UPROPERTY(VisibleAnywhere)
	FString URIRef; // The original short location reference. Aka Raicers/Chassis, or Partybears/Clothing
	UPROPERTY(VisibleAnywhere)
	FString URI;
	UPROPERTY(VisibleAnywhere)
	FString Json;
	UPROPERTY(VisibleAnywhere)
	FString BlueprintId;
};