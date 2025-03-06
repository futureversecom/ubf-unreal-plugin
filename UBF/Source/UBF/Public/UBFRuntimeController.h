// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionContext.h"
#include "UBFBindingObject.h"
#include "Components/ActorComponent.h"
#include "UBFRuntimeController.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnComplete, bool, Success, FUBFExecutionReport, ExecutionReport);

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

	UFUNCTION(BlueprintCallable)
	void ClearBlueprint();
	
	void TryExecute(const FString& BlueprintId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
	                const TSharedPtr<IGraphProvider>&  GraphProvider,
	                const TMap<FString, UBF::FBlueprintInstance>& BlueprintInstances, UBF::FExecutionContextHandle& ExecutionContext, const
	                FOnComplete& OnComplete);

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetLastOutputNames();

	UFUNCTION(BlueprintCallable)
	bool TryReadLastContextOutput(const FString& OutputId, FString& OutString) const;

	// Tries to read last context output as UObject, returns nullptr if not read or if the type is not supported
	UFUNCTION(BlueprintCallable)
	UObject* TryReadLastContextUObjectOutput(const FString& OutputId) const;

	void SetGraphProviders(const TSharedPtr<IGraphProvider>& GraphProvider);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetUBFActorsHidden(bool bIsHidden);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartWithUBFActorsHidden = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoUnHideUBFActorsOnComplete = true;

private:
	void OnComplete();
	TArray<AActor*> GetSpawnedActors() const;
	
	mutable TSharedPtr<IGraphProvider> CurrentGraphProvider;
	mutable UBF::FExecutionContextHandle LastExecutionContext;
	mutable UBF::FGraphHandle LastGraphHandle;
};
