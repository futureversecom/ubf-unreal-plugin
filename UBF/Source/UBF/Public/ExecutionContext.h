#pragma once

#include "CoreMinimal.h"
#include "BlueprintInstance.h"
#include "Dynamic.h"
#include "Graph.h"
#include "GraphProvider.h"
#include "UBFExecutionReport.h"
#include "UBFLogData.h"
#include "UBF/Lib/ubf_interpreter.h"
#include "DataTypes/SceneNode.h"

namespace UBF
{
	/**
	 * Holds Unreal specific UserData for graph execution
	 */
	class FContextData
	{
	public:
		~FContextData()
		{
			PinnedWorld->Release();
			delete Root;
		};

		FString BlueprintId;
		FSceneNode* Root;
		TSharedPtr<IGraphProvider> GraphProvider;
		TSharedPtr<FUBFLogData> LogData;
		TMap<FString, FBlueprintInstance> InstancedBlueprints;
		UGCPin* PinnedWorld;
		FGraphHandle Graph;
		TFunction<void()> OnComplete;
		
		explicit FContextData(const FString& BlueprintId, USceneComponent* Root, const TSharedPtr<IGraphProvider>& GraphProvider, const TSharedPtr<FUBFLogData>& LogData, const TMap<FString, FBlueprintInstance>& InstancedBlueprints,
			const FGraphHandle& Graph, TFunction<void()>&& OnComplete)
			: BlueprintId(BlueprintId), Root(new FSceneNode(Root)), GraphProvider(GraphProvider), LogData(LogData), InstancedBlueprints(InstancedBlueprints) ,Graph(Graph), OnComplete(MoveTemp(OnComplete))
		{
			if (Root->GetWorld())
			{
				PinnedWorld = UGCPin::Pin(Root->GetWorld());
			}
			else
			{
				LogData->Log(BlueprintId, EUBFLogLevel::Error, TEXT("Root GetWorld is Invalid"));
			}
		}

		void SetReadyToComplete() const;
		void SetComplete() const;

	private:
		mutable bool bIsReadyToComplete = false;
		mutable bool bIsComplete = false;
	};

	// Implement macros as base so constructors can be overridden 
	struct FExecutionHandleBase
	{
		IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(FExecutionHandleBase, FFI::ArcExecutionContext)
		IMPLEMENT_MANAGED_TYPE(FExecutionHandleBase, FFI::ArcExecutionContext, ctx_release, ctx_retain)
	};

	/**
	 * Handle for ExecutionContext that lives in rust.
	 * Holds reference to FContextData which contains relevant unreal specific data
	 *
	 * Responsible for interfacing which the execution in rust, such as TriggerNext(), TryReadInput(), TryReadOutput() and WriteOutput()
	 *
	 * Handle can be copied, when the last handle goes out of scope and rust no longer references the value, it will be deleted
	 */
	struct UBF_API FExecutionContextHandle : public FExecutionHandleBase
	{
	public:
		friend struct FGraphHandle;
		friend struct FRegistryHandle;
		
		explicit FExecutionContextHandle(FFI::ArcExecutionContext* RustPtr)
			: FExecutionHandleBase(RustPtr), DynamicContextData(CALL_RUST_FUNC(ctx_get_context_data)(RustPtr))
		{
			DynamicContextData.TryInterpretAs(this->ContextData);
		}

		FExecutionContextHandle() : FExecutionHandleBase(nullptr) {}

		FSceneNode* GetRoot() const { return ContextData->Root; }
		TSharedPtr<IGraphProvider> GetGraphProvider() const { return ContextData->GraphProvider; }
		const FContextData* GetUserData() const { return ContextData; }

		bool BlueprintInstanceExistsForId(const FString& InstanceId) const
		{
			if (ContextData == nullptr) return false;
			return ContextData->InstancedBlueprints.Contains(InstanceId);
		}

		FBlueprintInstance GetInstanceForId(const FString& InstanceId) const
		{
			if (ContextData == nullptr) return FBlueprintInstance();
			return ContextData->InstancedBlueprints[InstanceId];
		}

		// Gets world from UserData Root
		UWorld* GetWorld() const
		{
			check(this);
			
			if (ContextData == nullptr)
				DynamicContextData.TryInterpretAs(ContextData);
			
			check(ContextData);
			check(ContextData->Root);

			if (USceneComponent* SceneComponent = ContextData->Root->GetAttachmentComponent())
			{
				if (!IsValid(SceneComponent))
					return nullptr;

				return SceneComponent->GetWorld();
			}
			
			return nullptr;
		}
		
		FString GetBlueprintID() const
		{
			if (ContextData == nullptr)
				return FString("NoBlueprintIdContextDataIsNull");
			
			return ContextData->BlueprintId;
		}

		void Log(EUBFLogLevel Level, const FString& Log) const;
		
		void PrintBlueprintDebug(const FString& ContextString = FString()) const;

		void CompleteNode(const FFI::CompletionID CompletionID) const
		{
			CALL_RUST_FUNC(ctx_complete_node)(RustPtr, CompletionID);
		}

		bool TryTriggerNode(const FString& SourceNodeId, const FString& SourcePortKey) const;
		
		bool TryReadInput(const FString& NodeId, const FString& PortKey, FDynamicHandle& Dynamic) const;

		// Use when DynamicValue is a pointer
		template <class T>
		bool TryReadInput(const FString& NodeId, const FString& PortKey, T*& Out) const
		{
			FDynamicHandle Ptr;
			if (!TryReadInput(NodeId, PortKey, Ptr))
			{
				UBF_LOG(Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
				return false;
			}
		
			if constexpr (TIsDerivedFrom<T, UObject>::Value)
			{
				UObject* Input;
				if (!Ptr.TryInterpretAs(Input))
				{
					UBF_LOG(Warning, TEXT("Failed to read input as UObject (%s->%s)"), *NodeId, *FString(PortKey));
					return false;
				}
				Out = Cast<T>(Input);
				return true;
			}
			else
			{
				return Ptr.TryInterpretAs<T>(Out);
			}
		}

		// Use when DynamicValue is a value such as string, bool, int, float
		template <typename T>
		bool TryReadInputValue(const FString& NodeId, const FString& PortKey, T& Out) const
		{
			FDynamicHandle Ptr;
			if (!TryReadInput(NodeId, PortKey, Ptr))
			{
				return false;
			}

			UBF_LOG(VeryVerbose, TEXT("Read dynamic input value %s"), *Ptr.ToString());

			return Ptr.TryInterpretAs(Out);
		}
		
		// Use when DynamicValue is a pointer
		template <class T>
		bool TryReadInputArray(const FString& NodeId, const FString& PortKey, TArray<T*>& Out) const
		{
			FDynamicHandle DynamicArray;
			if (!TryReadInput(NodeId, PortKey, DynamicArray))
			{
				UBF_LOG(Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
				return false;
			}

			return DynamicArray.TryInterpretAsArray<T*>(Out);
		}
		// Use when DynamicValue is a value such as string, bool, int, float
		template <typename T>
		bool TryReadInputValueArray(const FString& NodeId, const FString& PortKey,
		TArray<T>& Out) const
		{
			FDynamicHandle DynamicArray;
			if (!TryReadInput(NodeId, PortKey, DynamicArray))
			{
				UBF_LOG(Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
				return false;
			}

			return DynamicArray.TryInterpretAsArray<T>(Out);
		}
		
		bool TryReadOutput(const FString& BindingId, FDynamicHandle& Dynamic) const;
		void WriteOutput(const FString& NodeId, const FString& PortKey, const FDynamicHandle& Dynamic) const;

		/*
		 * Used to retrieve generic data used in graph execution,
		 * Eg ApplyMaterial uses it to find texture settings for a certain resource id
		 */
		bool GetDynamicDataEntry(const FString& Key, FDynamicHandle& DynamicHandle) const;
		
		/*
		 * Used to store generic data used in graph execution,
		 * Eg SetTextureSettings uses it apply texture settings to a certain resource id
		 */
		bool SetDynamicDataEntry(const FString& Key, const FDynamicHandle& DynamicHandle) const;

		/*
		 * Used to get node's declared inputs
		 * Eg ExecuteBlueprintNode uses it to get inputs
		 */
		bool GetDeclaredNodeInputs(const FString& NodeId, TArray<FString>& OutInputs) const;

	private:
		FDynamicHandle DynamicContextData = FDynamicHandle::Null();
		mutable const FContextData* ContextData = nullptr;
	};
}
