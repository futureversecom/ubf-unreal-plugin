// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateSceneNode.h"

#include "UBFActor.h"
#include "DataTypes/SceneNode.h"

void UBF::FCreateSceneNode::ExecuteAsync() const
{
	UE_LOG(LogUBF, Verbose, TEXT("Executing CreateSceneNode Node"));
		
	FString NodeName = "NewSceneNode";
	if (!TryReadInputValue("Name", NodeName))
	{
		UE_LOG(LogUBF, Warning, TEXT("[CreateSceneNode] Failed to Read Name Input"));
		TriggerNext();
		CompleteAsyncExecution();
		return;
	}
	
	if (!GetWorld())
	{
		UE_LOG(LogUBF, Error, TEXT("[CreateSceneNode] GetWorld() is invalid"));

		TriggerNext();
		CompleteAsyncExecution();
		return;
	}
	
	AActor* NewNode = GetWorld()->SpawnActor<AUBFActor>();
	FSceneNode* Parent;
	if (TryReadInput("Parent", Parent))
	{
		NewNode->AttachToComponent(Parent->GetAttachmentComponent(), FAttachmentTransformRules::KeepRelativeTransform, Parent->GetAttachmentSocket());
	}
	else
	{
		UE_LOG(LogUBF, Warning, TEXT("[CreateSceneNode] New node's Parent wasn't provided. Attaching new node to ContextRoot"));
		NewNode->AttachToComponent(GetContext().GetRoot()->GetAttachmentComponent(), FAttachmentTransformRules::KeepRelativeTransform, GetContext().GetRoot()->GetAttachmentSocket());
	}
#if WITH_EDITOR
	if(!NodeName.IsEmpty())
	{
		NewNode->SetActorLabel(NodeName, true);
	}
#endif
	WriteOutput("Node", FDynamicHandle::ForeignHandled(new FSceneNode(NewNode->GetRootComponent())));
	TriggerNext();
	CompleteAsyncExecution();
}
