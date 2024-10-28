#pragma once
#include "CustomNode.h"

namespace UBF
{
	class UBF_API FFindSceneNodes final : public FCustomNode
	{
	protected:
		virtual bool ExecuteSync() const override;

	private:
		static void FindNodes(const FString& Filter, const USceneComponent* Root, const TFunction<void(TObjectPtr<USceneComponent>)>& OnNodeFound);
	};
}
