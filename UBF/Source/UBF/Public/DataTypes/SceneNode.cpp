// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "SceneNode.h"

namespace UBF
{
	FString FSceneNode::ToString() const
	{
		if (SceneComponent.IsValid())
		{
			return FString::Printf(TEXT("Actor:{%s}SceneComponent:{%s}Socket{%s}"), *SceneComponent->GetOwner()->GetName(), *SceneComponent->GetName(), *Socket.ToString());
		}
	
		return FString(TEXT("{NullSceneNode}"));
	}

}