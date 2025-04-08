// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomNode.h"

namespace UBF
{
	struct UBF_API FRegistryHandle
	{
	public:
		typedef FCustomNode* (*CreateInstanceFn)(const FString& NodeId,
		                                         const FFI::ScopeID ScopeID,
		                                         FExecutionContextHandle Context);
		friend struct FGraphHandle;

		template <std::derived_from<FCustomNode> T>
		void Register(const char* Name) const;
		
		static FRegistryHandle Default();

		IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(FRegistryHandle, FFI::NodeRegistry)
		IMPLEMENT_MANAGED_TYPE(FRegistryHandle, FFI::NodeRegistry, registry_release, registry_retain)
	};
}
