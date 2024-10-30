// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UBFActor.generated.h"

UCLASS()
class UBF_API AUBFActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUBFActor();

private:
	USceneComponent* SceneComponent;
};
