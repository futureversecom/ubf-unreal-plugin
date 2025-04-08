// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dynamic.h"
#include "Managed.h"
#include "UBFExecutionReport.h"

class FUBFLogData;
class ISubGraphResolver;
class IGraphProvider;

namespace UBF
{
	class IExecutionSetConfig;
	class FCustomNode;
	class FOnComplete;

	struct FRegistryHandle;
//	struct FExecutionContextHandle;
	
	struct FBindingInfo
	{
		FString Id;
		FString Type;
		FDynamicHandle DynamicPtr = FDynamicHandle::Null();

		FBindingInfo() {}
		FBindingInfo(const FString& NewId, const FString& NewType, const FDynamicHandle& Dynamic)
		{
			Id = NewId;
			Type = NewType;
			DynamicPtr = Dynamic;
		};
	};

	struct FResourceInfo
	{
		FString Id;
		FString Uri;
		
		FResourceInfo() {}
		FResourceInfo(const FString& NewId, const FString& NewUri)
		{
			Id = NewId;
			Uri = NewUri;
		}

		FString ToString() const
		{
			return FString::Printf(TEXT("Id: %s Uri: %s"), *Id, *Uri);
		}
	};

	struct FGraphVersion
	{
		FGraphVersion() {}
		FGraphVersion(const FString& VersionString)
		{
			const auto Version = FromString(VersionString);
			Major = Version.Get<0>();
			Minor = Version.Get<1>();
			Patch = Version.Get<2>();
		}

		bool IsValid() const
		{
			return Major > -1 && Minor > -1 && Patch > -1;
		}
		
		FString ToString() const
		{
			return FString::Printf(TEXT("%d.%d.%d"), Major, Minor, Patch);
		}

		bool operator>(const FGraphVersion& Other) const
		{
			// worry about Major and Minor for now
			const auto OtherVersion = FromString(Other.ToString());
			return Major > OtherVersion.Get<0>()
				|| (Major == OtherVersion.Get<0>() && Minor > OtherVersion.Get<1>());
				//|| (Major == OtherVersion.Get<0>() && Minor == OtherVersion.Get<1>() && Patch > OtherVersion.Get<2>());
		}
		
		bool operator==(const FGraphVersion& Other) const
		{
			const auto OtherVersion = FromString(Other.ToString());
			return Major > OtherVersion.Get<0>()
				|| (Major == OtherVersion.Get<0>() && Minor == OtherVersion.Get<1>())
				|| (Major == OtherVersion.Get<0>() && Minor == OtherVersion.Get<1>() && Patch == OtherVersion.Get<2>());
		}
		
		bool operator<(const FGraphVersion& Other) const { return Other > *this; }
		bool operator<=(const FGraphVersion& Other) const { return !(Other < *this); }
		bool operator>=(const FGraphVersion& Other) const { return !(*this < Other); }
		
	private:
		static TTuple<int32,int32,int32> FromString(const FString& VersionString)
		{
			TArray<FString> Parts;
			VersionString.ParseIntoArray(Parts, TEXT("."),true);

		 	TTuple<int32,int32,int32> Version(-1, -1, -1);
			if(Parts.IsValidIndex(0))
			{
				Version.Get<0>() = FCString::Atoi(*Parts[0]);
			}
			if(Parts.IsValidIndex(1))
			{
				Version.Get<1>() = FCString::Atoi(*Parts[1]);
			}
			if(Parts.IsValidIndex(2))
			{
				Version.Get<2>()= FCString::Atoi(*Parts[2]);
			}
			return Version;
		}
		
		int32 Major = -1;
		int32 Minor = -1;
		int32 Patch = -1;
	
	};
	
	struct UBF_API FGraphHandle
	{
		IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(FGraphHandle, FFI::GraphInstance)
		IMPLEMENT_MANAGED_TYPE(FGraphHandle, FFI::GraphInstance, graph_release, graph_retain)
	public:
		
		void Execute(
			const FString& BlueprintId,
			const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			const TMap<FString, FDynamicHandle>& Inputs,
			TFunction<void(bool, FUBFExecutionReport)>&& OnComplete,
			struct FExecutionContextHandle& Handle) const;
		
		static void OnNodeComplete(FFI::Dynamic*, FFI::ScopeID);
		
		static bool Load(const FRegistryHandle& Registry, const FString &Json, FGraphHandle& Graph);

		void GetOutputs(TArray<FBindingInfo>& Outputs) const;
		void GetInputs(TArray<FBindingInfo>& Inputs) const;
		FGraphVersion GetGraphVersion() const;

	private:
	
		mutable FString CachedId;
		mutable FGraphVersion CachedVersion;
	};

	static FGraphVersion MinSupportedGraphVersion = FGraphVersion(TEXT("0.0.0"));
	static FGraphVersion MaxSupportedGraphVersion = FGraphVersion(TEXT("0.1.0"));
}
