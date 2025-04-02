// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "UBFActor.h"


// Sets default values
AUBFActor::AUBFActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneComponent);
}


