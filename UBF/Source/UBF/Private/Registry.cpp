// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Registry.h"

#include "Material/ApplyMaterialNode.h"
#include "BindMeshes.h"
#include "CreateMeshConfig.h"
#include "CreateSceneNode.h"
#include "DebugLog.h"
#include "ExecuteBlueprint2Node.h"
#include "FindFirstSceneNode.h"
#include "FindRenderer.h"
#include "FindSceneNodes.h"
#include "Material/MaterialNode.h"
#include "SetBlendshapeNode.h"
#include "SetSceneNodeEnabledNode.h"
#include "SetTextureSettingsNode.h"
#include "Material/SGDecalPropertiesNode.h"
#include "Material/SGFurCardPropertiesNode.h"
#include "Material/SGFurPropertiesNode.h"
#include "Material/SGHairPropertiesNode.h"
#include "Material/SGPBRPropertiesNode.h"
#include "SpawnMeshNode.h"
#include "TransformPositionNode.h"
#include "TransformScaleNode.h"
#include "UBF.h"
#include "UBFLog.h"
#include "Material/MakeDecalMaterial.h"
#include "Material/MakeFurMaterial.h"
#include "Material/MakeHairMaterial.h"
#include "Material/MakePBRMaterial.h"
#include "Material/MakeSkinMaterial.h"

namespace UBF
{
	void ReleaseCallback(intptr_t ThisPtr)
	{
		UE_LOG(LogUBF, Log, TEXT("Custom Node Released"));
	}

	void ExecuteCallback(intptr_t const ThisPtr,
	                     const char* NodeId,
	                     const FFI::CompletionID CompletionId,
	                     FFI::ArcExecutionContext* ContextPtr)
	{
		const FRegistryHandle::CreateInstanceFn CreateFn = reinterpret_cast<FRegistryHandle::CreateInstanceFn>(ThisPtr);
		
		const FCustomNode* Node = CreateFn(
			FString(strlen(NodeId), NodeId),
			CompletionId,
			FExecutionContextHandle(ContextPtr));
		
		Node->Execute();
	}

	template <std::derived_from<FCustomNode> T>
	void FRegistryHandle::Register(const char* Name) const
	{
		CreateInstanceFn CreateInstanceFn = [](
			const FString& NodeId,
			const FFI::CompletionID CompletionID,
			FExecutionContextHandle Context
		)
		{
			T* Node = new T;
			Node->Bind(NodeId, CompletionID, Context);
			return reinterpret_cast<FCustomNode*>(Node);
		};

		const FString NameString = FString(Name);
		CALL_RUST_FUNC(registry_register_node)(
			RustPtr,
			TCHAR_TO_UTF16(*NameString),
			NameString.Len(),
			reinterpret_cast<intptr_t>(CreateInstanceFn),
			ExecuteCallback,
			ReleaseCallback
		);
	}

	FRegistryHandle FRegistryHandle::Default()
	{
		FFI::NodeRegistry* RustPtr = CALL_RUST_FUNC(registry_new)();
		FRegistryHandle Registry = FRegistryHandle(RustPtr);
		
		// name must match the node type name in ubf.json
		Registry.Register<FDebugLog>("DebugLog");
		Registry.Register<FCreateSceneNode>("CreateSceneNode");
		Registry.Register<FFindSceneNodes>("FindSceneNodes");
		Registry.Register<FFindFirstSceneNode>("FindFirstSceneNode");
		Registry.Register<FSetSceneNodeEnabledNode>("SetSceneNodeEnabled");
		Registry.Register<FApplyMaterialNode>("ApplyMaterial");
		Registry.Register<FMaterialNode>("Material");
		Registry.Register<FSGDecalPropertiesNode>("SGDecalProperties");
		Registry.Register<FSGPBRPropertiesNode>("PBRProperties");
		Registry.Register<FSGHairPropertiesNode>("MakeHairProperties");
		Registry.Register<FSGFurPropertiesNode>("SGFurProperties");
		Registry.Register<FSGFurCardPropertiesNode>("SGFurCardProperties");
		Registry.Register<FSpawnMeshNode>("SpawnMesh");
		Registry.Register<FSetTextureSettingsNode>("SetTextureSettings");
		Registry.Register<FSetBlendshapeNode>("SetBlendshape");
		Registry.Register<FBindMeshes>("BindMeshes");
		Registry.Register<FExecuteBlueprint2Node>("ExecuteBlueprint");
		Registry.Register<FCreateMeshConfig>("CreateMeshConfig");
		Registry.Register<FFindRenderer>("FindRenderer");
		Registry.Register<FTransformPositionNode>("TransformPosition");
		Registry.Register<FTransformScaleNode>("TransformScale");
		Registry.Register<FExecuteBlueprint2Node>("ExecuteBlueprint2");
		Registry.Register<FMakePBRMaterial>("MakePBRMaterial");
		Registry.Register<FMakeDecalMaterial>("MakeDecalMaterial");
		Registry.Register<FMakeFurMaterial>("MakeFurMaterial");
		Registry.Register<FMakeHairMaterial>("MakeHairMaterial");
		Registry.Register<FMakeSkinMaterial>("MakeSkinMaterial");
		
		return Registry;
	}
}
