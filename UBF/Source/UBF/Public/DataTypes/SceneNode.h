// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

namespace UBF
{
	class FSceneNode
	{
	public:
		FSceneNode(USceneComponent* SceneComponent, FName Socket = NAME_None) : SceneComponent(SceneComponent), Socket(Socket){}
		FSceneNode(const FSceneNode* Other) : SceneComponent(Other->SceneComponent), Socket(Other->Socket){}
		
		USceneComponent* GetAttachmentComponent() const {return SceneComponent.Get();}
		FName GetAttachmentSocket() const {return Socket;}
		UWorld* GetWorld() const {return SceneComponent.IsValid() ? SceneComponent->GetWorld() : nullptr;}
	
		FString ToString() const;

	private:
		TWeakObjectPtr<USceneComponent> SceneComponent = nullptr;
		FName Socket = NAME_None;
	};
}