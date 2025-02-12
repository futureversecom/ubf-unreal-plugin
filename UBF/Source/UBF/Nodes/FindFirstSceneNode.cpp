// Fill out your copyright notice in the Description page of Project Settings.


#include "FindFirstSceneNode.h"

#include "DataTypes/SceneNode.h"

bool UBF::FFindFirstSceneNode::ExecuteSync() const
{
	UBF_LOG(Verbose, TEXT("[FindFirstSceneNode] Executing Node"));
		
	FString Filter;
	if (!TryReadInputValue("Filter", Filter))
	{
		UBF_LOG(Warning, TEXT("[FindFirstSceneNode] Failed to Read Filter Input"));
		TriggerNext();
		return true;
	}
	
	FSceneNode* RootInput;
	if (!TryReadInput("Root", RootInput))
	{
		UBF_LOG(Warning, TEXT("[FindFirstSceneNode] Failed to Read Root Input."));
		TriggerNext();
		return true;
	}
	
	if (!RootInput || !RootInput->GetAttachmentComponent())
	{
		UBF_LOG(Warning, TEXT("[FindFirstSceneNode] Root is invalid"));
		TriggerNext();
		return true;
	}
	
	TArray<USceneComponent*> FoundNodes;
	
	auto OnNodeFound = [this, &FoundNodes](USceneComponent* Node)
	{
		FoundNodes.Add(Node);
	};
		
	FindNodes(Filter, RootInput->GetAttachmentComponent(), OnNodeFound);

	if (FoundNodes.Num() > 0)
	{
		UBF_LOG(Verbose, TEXT("[FindFirstSceneNode] Found Node: %s with Filter %s"), *FoundNodes[0]->GetName(), *Filter);
		WriteOutput("Node", FDynamicHandle::ForeignHandled(new FSceneNode(FoundNodes[0])));
	}
	else
	{
		UBF_LOG(Warning, TEXT("[FindFirstSceneNode] Found No Matching Nodes with Filter %s"), *Filter);
	}

	TriggerNext();
	return true;
}

void UBF::FFindFirstSceneNode::FindNodes(const FString& Filter, const USceneComponent* Root,
	const TFunction<void(TObjectPtr<USceneComponent>)>& OnNodeFound)
{
	if (Root == nullptr) return;
	
	for (auto AttachedChild : Root->GetAttachChildren())
	{
		UBF_LOG(VeryVerbose, TEXT("[FindFirstSceneNode] Comparing USceneComponent %s to Filter: %s"),
			*AttachedChild->GetName(), *Filter);
		
		if(AttachedChild.GetName().StartsWith(Filter))
		{
			UBF_LOG(VeryVerbose, TEXT("[FindFirstSceneNode] Adding USceneComponent %s to FoundNodes"),
			*AttachedChild->GetName());
			OnNodeFound(AttachedChild);
		}
		
		FindNodes(Filter, AttachedChild, OnNodeFound);
	}
}
