// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFActor.h"


// Sets default values
AUBFActor::AUBFActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneComponent);
}


