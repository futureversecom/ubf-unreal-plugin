// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "Graph.h"
#include "GraphProvider.h"
#include "HttpModule.h"
#include "UBFUtils.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

namespace APIUtils
{
	static TFuture<TSharedPtr<IHttpResponse>> MakeDownloadRequest(const FString& Path)
	{
		TSharedPtr<TPromise<TSharedPtr<IHttpResponse>>> Promise = MakeShareable(new TPromise<TSharedPtr<IHttpResponse>>());
		TFuture<TSharedPtr<IHttpResponse>> Future = Promise->GetFuture();
	
		const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

		// encode space character
		const FString EncodedUrl = Path.Replace(TEXT(" "), TEXT("%20"));
		Request->SetURL(EncodedUrl);
		Request->SetVerb(TEXT("GET"));
	
		auto Callback = [Promise](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bWasSuccessful) mutable
		{
			if (Response == nullptr)
			{
				Promise->SetValue(nullptr);
				return;
			}
		
			if (bWasSuccessful && Response.IsValid())
			{
				Promise->SetValue(Response);
			}
			else
			{
				Promise->SetValue(nullptr);
			}
		};
	
		Request->OnProcessRequestComplete().BindLambda(Callback);
		Request->ProcessRequest();

		return Future;
	}

	static bool LoadLocalFileToData(const FString& ResourcePath, TArray<uint8>& Data)
	{
		if (!FPaths::FileExists(ResourcePath))
		{
			UE_LOG(LogUBF, Warning, TEXT("Could not find file with given path: %s"), *ResourcePath);
			return false;
		}
	
		if (!FFileHelper::LoadFileToArray(Data, *ResourcePath))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to load file from path: %s"), *ResourcePath);
			return false;
		}

		return true;
	}

	static bool LoadLocalFileToString(const FString& FilePath, FString& LoadedString)
	{
		if (!FPaths::FileExists(FilePath))
		{
			UE_LOG(LogUBF, Warning, TEXT("Could not find file with given path: %s"), *FilePath);
			return false;
		}
		
		if (!FFileHelper::LoadFileToString(LoadedString, *FilePath))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to read file: %s"), *FilePath);
			return false;
		}
	
		return true;
	}
	
	static TFuture<UBF::FLoadDataArrayResult> LoadDataFromURI(const FString& Path)
	{
		TSharedPtr<TPromise<UBF::FLoadDataArrayResult>> Promise = MakeShareable(new TPromise<UBF::FLoadDataArrayResult>());
		TFuture<UBF::FLoadDataArrayResult> Future = Promise->GetFuture();

		TArray<uint8> CachedData;

		FString TrimmedPath = Path.TrimStartAndEnd();

		if (TrimmedPath.StartsWith(TEXT("http")))
		{
			MakeDownloadRequest(TrimmedPath).Next([Promise, TrimmedPath](const TSharedPtr<IHttpResponse>& Response)
			{
				UBF::FLoadDataArrayResult LoadResult;
				TSharedPtr<TArray<uint8>> Data = MakeShared<TArray<uint8>>();
				
				if (Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
				{
					*Data = Response->GetContent();

					LoadResult.bSuccess = true;
					LoadResult.Value = Data;
					Promise->SetValue(LoadResult);
				}
				else if (Response.IsValid())
				{
					FString ErrorMessage = GetDescription(static_cast<EHttpResponseCodes::Type>(Response->GetResponseCode())).ToString();
					UE_LOG(LogUBF, Error, TEXT("Failed to download with URL: %s Message: %s"), *TrimmedPath, *ErrorMessage);
					Promise->SetValue(LoadResult);
				}
				else
				{
					UE_LOG(LogUBF, Error, TEXT("Invalid Response from URL: %s"), *TrimmedPath);
					Promise->SetValue(LoadResult);
				}
			});
		}
		else
		{
			// path is local
			UBF::FLoadDataArrayResult LoadResult;
			TSharedPtr<TArray<uint8>> Data = MakeShared<TArray<uint8>>();
			
			bool WasSuccessful = LoadLocalFileToData(TrimmedPath, *Data);
			if (!WasSuccessful)
			{
				UE_LOG(LogUBF, Error, TEXT("Failed to load file to data from path: %s"), *TrimmedPath);
			}

			LoadResult.bSuccess = WasSuccessful;
			LoadResult.Value = Data;
			Promise->SetValue(LoadResult);
		}
		
		return Future;
	}

	static TFuture<UBF::FLoadStringResult> LoadStringFromURI(const FString& Path)
	{
		TSharedPtr<TPromise<UBF::FLoadStringResult>> Promise = MakeShareable(new TPromise<UBF::FLoadStringResult>());
		TFuture<UBF::FLoadStringResult> Future = Promise->GetFuture();
		
		if (Path.StartsWith(TEXT("http")))
		{
			MakeDownloadRequest(Path).Next([Promise, Path](const TSharedPtr<IHttpResponse>& Response)
			{
				UBF::FLoadStringResult LoadResult;
				FString LoadedString;
				if (Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
				{
					LoadedString = Response->GetContentAsString();

					LoadResult.bSuccess = true;
					LoadResult.Value = LoadedString;
					Promise->SetValue(LoadResult);

				}
				else if (Response.IsValid())
				{
					FString ErrorMessage = GetDescription(static_cast<EHttpResponseCodes::Type>(Response->GetResponseCode())).ToString();
					UE_LOG(LogUBF, Error, TEXT("Failed to download with URL: %s Message: %s"), *Path, *ErrorMessage);
					Promise->SetValue(LoadResult);
				}
				else
				{
					UE_LOG(LogUBF, Error, TEXT("Invalid Response from URL: %s"), *Path);
					Promise->SetValue(LoadResult);
				}
			});
		}
		else
		{
			// path is local
			UBF::FLoadStringResult LoadResult;
			FString LoadedString;
			
			bool WasSuccessful = LoadLocalFileToString(Path, LoadedString);
			if (!WasSuccessful)
			{
				UE_LOG(LogUBF, Error, TEXT("Failed to load file to string from path: %s"), *Path);
			}

			LoadResult.Value = LoadedString;
			LoadResult.bSuccess = WasSuccessful;
			
			Promise->SetValue(LoadResult);
		}
		
		return Future;
	}
}
