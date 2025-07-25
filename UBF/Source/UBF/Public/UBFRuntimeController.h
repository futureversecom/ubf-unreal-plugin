// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionContext.h"
#include "UBFBindingObject.h"
#include "Components/ActorComponent.h"
#include "ExecutionSets/ExecutionInstanceData.h"
#include "ExecutionSets/ExecutionSetHandle.h"
#include "UBFRuntimeController.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnComplete, bool, Success, FUBFExecutionReport, ExecutionReport);

USTRUCT(BlueprintType)
struct FBlueprintExecutionData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FString, UUBFBindingObject*> InputMap;
	TArray<UBF::FExecutionInstanceData> BlueprintInstances;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UBF_API UUBFRuntimeController : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootComponent;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnComplete"))
	void ExecuteBlueprint(FString RootID, const FBlueprintExecutionData& ExecutionData, const FOnComplete& OnComplete);

	UFUNCTION(BlueprintCallable)
	void ClearBlueprint();
	
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetLastOutputNames();

	UFUNCTION(BlueprintCallable)
	bool TryReadLastContextOutput(const FString& OutputId, FString& OutString) const;

	// Tries to read last context output as UObject, returns nullptr if not read or if the type is not supported
	UFUNCTION(BlueprintCallable)
	UObject* TryReadLastContextUObjectOutput(const FString& OutputId) const;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetUBFActorsHidden(bool bIsHidden);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStartWithUBFActorsHidden = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoUnHideUBFActorsOnComplete = true;

private:

	
	void OnComplete(bool bWasSuccessful);
	TArray<AActor*> GetSpawnedActors() const;
	
	UBF::FExecutionSetHandle LastSetHandle;
};
