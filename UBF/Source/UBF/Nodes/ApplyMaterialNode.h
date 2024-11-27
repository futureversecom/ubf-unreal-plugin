#pragma once
#include "CustomNode.h"

namespace UBF
{
	struct FShaderPropertyValue;

	class UBF_API FApplyMaterialNode final : public FCustomNode
	{
	protected:
		virtual void ExecuteAsync() const override;

	private:
		TFuture<bool> EvaluateProperty(const TTuple<FString, FShaderPropertyValue>& Prop,
		                                      TWeakObjectPtr<UMaterialInstanceDynamic> Mat) const;

		void CheckFuturesComplete(bool bWasSuccessful) const;

		mutable int AwaitingFutures = 0;
		mutable UMeshComponent* WorkingMeshRenderer = nullptr;
		mutable TWeakObjectPtr<UMaterialInstanceDynamic> WorkingMaterialInstance = nullptr;
	};
}

