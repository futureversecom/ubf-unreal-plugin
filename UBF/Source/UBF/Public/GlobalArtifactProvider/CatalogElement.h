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
			// legacy catalog elements have either empty or unknown as the value for type
			if (Type.IsEmpty() || Type.ToLower().Equals("unknown")
				|| (Other.Type.IsEmpty() || Other.Type.ToLower().Equals("unknown")))
			{
				return Id == Other.Id &&
					   Uri == Other.Uri;
			}
			
			return Id == Other.Id &&
					Type == Other.Type &&
					Uri == Other.Uri;
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
