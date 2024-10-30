#include "FindSceneNodes.h"
#include "UBFLog.h"
#include "DataTypes/SceneNode.h"

namespace UBF
{
	bool FFindSceneNodes::ExecuteSync() const
	{
		UE_LOG(LogUBF, Verbose, TEXT("[FindSceneNodes] Executing Node"));
		
		FString Filter;
		if (!TryReadInputValue("Filter", Filter))
		{
			UE_LOG(LogUBF, Warning, TEXT("[FindSceneNodes] Failed to Read Filter Input"));
			TriggerNext();
			return true;
		}
		
		FSceneNode* RootInput;
		if (!TryReadInput("Root", RootInput))
		{
			UE_LOG(LogUBF, Warning, TEXT("[FindSceneNodes] Failed to Read Root Input."));
			TriggerNext();
			return true;
		}
	
		if (!RootInput || !RootInput->GetAttachmentComponent())
		{
			UE_LOG(LogUBF, Warning, TEXT("[FindSceneNodes] Root is invalid"));
			TriggerNext();
			return true;
		}

		int32 Count = 0;
		auto FoundNodes = FDynamicHandle::Array();

		auto OnNodeFound = [FoundNodes, &Count](const TObjectPtr<USceneComponent> FoundNode)
		{
			FoundNodes.Push(FDynamicHandle::ForeignHandled(new FSceneNode(FoundNode)));
			Count++;
		};
		
		FindNodes(Filter, RootInput->GetAttachmentComponent(), OnNodeFound);
		
		UE_LOG(LogUBF, Verbose, TEXT("[FindSceneNodes] Found %d Nodes"), Count);
		WriteOutput("Nodes", FoundNodes);
		TriggerNext();
		return true;
	}

	void FFindSceneNodes::FindNodes(const FString& Filter, const USceneComponent* Root,
		const TFunction<void(TObjectPtr<USceneComponent>)>& OnNodeFound)
	{
		if (Root == nullptr) return;
		
		for (auto AttachedChild : Root->GetAttachChildren())
		{
			UE_LOG(LogUBF, Verbose, TEXT("[FindSceneNodes] Comparing USceneComponent %s to Filter: %s"),
				*AttachedChild->GetName(), *Filter);
			
			if(AttachedChild.GetName().StartsWith(Filter))
			{
				UE_LOG(LogUBF, Verbose, TEXT("[FindSceneNodes] Adding USceneComponent %s to FoundNodes"),
				*AttachedChild->GetName());
				OnNodeFound(AttachedChild);
			}

			FindNodes(Filter, AttachedChild, OnNodeFound);
		}
	}
}
