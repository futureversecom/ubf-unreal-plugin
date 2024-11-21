// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionContext.h"
#include "UBFBindingObject.h"
#include "Components/ActorComponent.h"
#include "UBFRuntimeController.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnComplete);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UBF_API UUBFRuntimeController : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootComponent;

	UFUNCTION(BlueprintCallable)
	void ExecuteGraph(FString GraphId, const TMap<FString, UUBFBindingObject*>& InputMap, const FOnComplete& OnComplete);
	
	void TryExecute(const FString& GraphId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
		IGraphProvider* GraphProvider, ISubGraphResolver* SubGraphResolver,
		UBF::FExecutionContextHandle& ExecutionContext, const FOnComplete& OnComplete) const;

	void SetGraphProviders(IGraphProvider* GraphProvider, ISubGraphResolver* SubGraphResolver);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	mutable IGraphProvider* CurrentGraphProvider;
	mutable ISubGraphResolver* CurrentSubGraphResolver;
	UBF::FExecutionContextHandle LastExecutionContext;
};
