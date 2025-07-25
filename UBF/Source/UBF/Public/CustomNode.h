// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dynamic.h"
#include "ExecutionContext.h"
#include "DataTypes/SceneNode.h"

namespace UBF
{
	/**
	 * Base class for nodes that need to be implemented in unreal instead of native rust.
	 * If node can be executed synchronous override ExecuteSync() and return true. This returns false by default
	 * If node needs to be async, override ExecuteAsync() and call CompleteAsyncExecution() once done
	 * If node has Exec output you must call TriggerNext() when you want that to trigger, usually done at the end of ExecuteSync() or ExecuteAsync()
	 */
	class UBF_API FCustomNode
	{
		FString NodeId;
		FExecutionContextHandle Context;
		FFI::ScopeID ScopeID = -1;
		
	protected:
		// Use when DynamicValue is a pointer
		template <typename T>
		bool TryReadInput(const FString& PortKey, T*& Out) const 
		{
			return Context.TryReadInput<T>(NodeId, PortKey, ScopeID, Out);
		}

		// Use when DynamicValue is a value such as string, bool, int, float
		template <typename T>
		bool TryReadInputValue(const FString& PortKey, T& Out) const
		{
			return Context.TryReadInputValue(NodeId, PortKey, ScopeID, Out);
		}

		// Use when DynamicValue is a pointer
		template <typename T>
		bool TryReadInputArray(const FString& PortKey, TArray<T*>& Out) const
		{
			return Context.TryReadInputArray<T>(NodeId, PortKey, ScopeID, Out);
		}

		// Use when DynamicValue is a value such as string, bool, int, float
		template <typename T>
		bool TryReadInputValueArray(const FString& PortKey, TArray<T>& Out) const
		{
			return Context.TryReadInputValueArray(NodeId, PortKey, ScopeID, Out);
		}

		bool TryReadInput(const FString& PortKey, FDynamicHandle& Dynamic) const
		{
			return Context.TryReadInput(NodeId, PortKey, ScopeID, Dynamic);
		}
		
		void TriggerNext() const
		{
			Context.TryTriggerNode(NodeId, FString("Exec"), ScopeID);
		}

		bool TryGetResourceId(const FString& PinName, FString& Id) const
		{
			return Context.TryReadInputValue(NodeId, PinName, ScopeID, Id);
		}

		void WriteOutput(const FString& Key, const FDynamicHandle& Dynamic) const
		{
			Context.WriteOutput(NodeId, Key, Dynamic);
		}

		/*
		 * Used to retrieve generic data used in graph execution,
		 * Eg ApplyMaterial uses it to find texture settings for a certain resource id
		 */
		bool GetDynamicDataEntry(const FString& Key, FDynamicHandle& DynamicHandle) const
		{
			return Context.GetDynamicDataEntry(Key, DynamicHandle);
		}
		
		/*
		 * Used to store generic data used in graph execution,
		 * Eg SetTextureSettings uses it apply texture settings to a certain resource id
		 */
		bool SetDynamicDataEntry(const FString& Key, const FDynamicHandle& DynamicHandle) const
		{
			return Context.SetDynamicDataEntry(Key, DynamicHandle);
		}

		const FContextData* GetUserData() const {return Context.GetUserData();}
		FContextData* GetMutableUserData() const {return const_cast<FContextData*>(Context.GetUserData());}

		// Return true if execution handled here, return false if ExecuteAsync() required
		virtual bool ExecuteSync() const { return false;}

		// Call CompleteAsyncExecution on GT when complete
		virtual void ExecuteAsync() const {}

		void CompleteAsyncExecution() const
		{
			// maybe its fine to complete in other threads? we just need to make sure execute sync nodes run in GT
			check(IsInGameThread());
			Context.CompleteNode(ScopeID);
			delete this; // Should this delete? FRegistry makes it seem like rust should be in charge of deleting nodes
		}

		void Log(EUBFLogLevel Level, const FString& Log) const
		{
			GetContext().Log(Level, Log);
		}

		bool CheckExecutionStillValid() const
		{
			if (!IsValid(GetWorld()) || GetWorld()->bIsTearingDown)
				return false;

			if (!GetRoot() || !IsValid(GetRoot()->GetAttachmentComponent()))
				return false;

			if (GetContext().GetCancelExecution())
				return false;

			return true;
		}
		
	public:
		virtual ~FCustomNode() = default;

		FString GetNodeId() const
		{
			return NodeId;
		}
		
		FString GetGraphId() const
		{
			return Context.GetBlueprintID();
		}

		const FExecutionContextHandle& GetContext() const
		{
			return Context;
		}
		
		UWorld* GetWorld() const
		{
			return Context.GetWorld();
		}

		FSceneNode* GetRoot() const
		{
			return Context.GetRoot();
		}

		FFI::ArcExecutionContext* GetRustContext() const
		{
			return Context.GetRustPtr();
		}

		void Execute() const
		{
			// We could support executing on other threads in the future, although spawning and applying materials
			// have to be run on the game thread
			check(IsInGameThread());
			
			if (!CheckExecutionStillValid() || ExecuteSync())
			{
				Context.CompleteNode(ScopeID);
				delete this;
				return;
			}

			ExecuteAsync();
		}

		void Bind(FString OurNodeId, const FFI::ScopeID OurScopeID, const FExecutionContextHandle& OurContext)
		{
			NodeId = OurNodeId;
			Context = OurContext;
			ScopeID = OurScopeID;
		}
	};
}
