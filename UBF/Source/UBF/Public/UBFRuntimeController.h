// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionContext.h"
#include "UBFBindingObject.h"
#include "Components/ActorComponent.h"
#include "UBFRuntimeController.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnComplete);

USTRUCT(BlueprintType)
struct FBlueprintExecutionData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, UUBFBindingObject*> InputMap;
	TArray<UBF::FBlueprintInstance> BlueprintInstances;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UBF_API UUBFRuntimeController : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootComponent;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnComplete"))
	void ExecuteBlueprint(FString BlueprintId, const FBlueprintExecutionData& ExecutionData, const FOnComplete& OnComplete);
	
	void TryExecute(const FString& BlueprintId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
	                IGraphProvider* GraphProvider,
	                const TMap<FString, UBF::FBlueprintInstance>& BlueprintInstances, UBF::FExecutionContextHandle& ExecutionContext, const
	                FOnComplete& OnComplete) const;

	void SetGraphProviders(IGraphProvider* GraphProvider);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	mutable IGraphProvider* CurrentGraphProvider;
	UBF::FExecutionContextHandle LastExecutionContext;
};
