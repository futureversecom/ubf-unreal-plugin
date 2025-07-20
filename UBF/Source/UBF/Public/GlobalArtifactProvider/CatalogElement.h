// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#pragma once
#include "JsonObjectWrapper.h"

namespace UBF
{
	struct UBF_API FCatalogElement
	{
		FCatalogElement(){}

		// Equal operator
		bool operator==(const FCatalogElement& Other) const
		{
			return Id == Other.Id &&
				   Type == Other.Type &&
				   Uri == Other.Uri &&
				   Hash == Other.Hash;
		}

		bool EqualWithoutHash(const FCatalogElement& Other) const
		{
			return Id == Other.Id &&
				   Uri == Other.Uri;
				   //Type == Other.Type &&
				  // MetadataJsonWrapper.JsonString == Other.MetadataJsonWrapper.JsonString;
		}
		
		FString ToString() const
		{
			return FString::Printf(TEXT("Id: %s, Type: %s, Uri: %s, Hash: %s, Metadata: %s"),
				*Id, *Type, *Uri, *Hash, *MetadataJsonWrapper.JsonString);
		}
	
		FString Id;
		FString Type;
		FString Uri;
		FString Hash;
		FJsonObjectWrapper MetadataJsonWrapper; 
	};
}
