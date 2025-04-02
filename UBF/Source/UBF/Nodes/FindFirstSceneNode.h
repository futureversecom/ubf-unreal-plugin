// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CustomNode.h"

/**
 * 
 */
namespace UBF
{
	class UBF_API FFindFirstSceneNode final : public FCustomNode
	{
	protected:
		virtual bool ExecuteSync() const override;
		
	private:
		void FindNodes(const FString& Filter, const USceneComponent* Root, const TFunction<void(TObjectPtr<USceneComponent>)>& OnNodeFound) const;
	};
}
