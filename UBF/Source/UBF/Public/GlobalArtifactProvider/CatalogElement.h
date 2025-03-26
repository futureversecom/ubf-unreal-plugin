// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#pragma once

struct FCatalogElement
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
			   Type == Other.Type &&
			   Uri == Other.Uri;
	}

	// ToString method
	FString ToString() const
	{
		return FString::Printf(TEXT("Id: %s, Type: %s, Uri: %s, Hash: %s"),
							   *Id, *Type, *Uri, *Hash);
	}
	
	FString Id;
	FString Type;
	FString Uri;
	FString Hash;
};