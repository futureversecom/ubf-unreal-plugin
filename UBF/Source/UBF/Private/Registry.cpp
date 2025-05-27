// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Registry.h"

#include "UBF.h"
#include "UBFLog.h"
#include "Nodes/CreateSceneNode.h"
#include "Nodes/DebugLog.h"
#include "Nodes/ExecuteBlueprint2Node.h"
#include "Nodes/FindFirstSceneNode.h"
#include "Nodes/FindRenderer.h"
#include "Nodes/FindSceneNodes.h"
#include "Nodes/SetBlendshapeNode.h"
#include "Nodes/SetSceneNodeEnabledNode.h"
#include "Nodes/SetTextureSettingsNode.h"
#include "Nodes/TransformPositionNode.h"
#include "Nodes/TransformScaleNode.h"
#include "Nodes/Material/ApplyMaterialNode.h"
#include "Nodes/Material/MakeDecalMaterial.h"
#include "Nodes/Material/MakeFurMaterial.h"
#include "Nodes/Material/MakeHairMaterial.h"
#include "Nodes/Material/MakePBRMaterial.h"
#include "Nodes/Material/MakeSkinMaterial.h"
#include "Nodes/Material/MaterialNode.h"
#include "Nodes/Material/SGDecalPropertiesNode.h"
#include "Nodes/Material/SGFurCardPropertiesNode.h"
#include "Nodes/Material/SGFurPropertiesNode.h"
#include "Nodes/Material/SGHairPropertiesNode.h"
#include "Nodes/Material/SGPBRPropertiesNode.h"
#include "Nodes/Mesh/BindMeshes.h"
#include "Nodes/Mesh/CreateMeshConfig.h"
#include "Nodes/Mesh/SpawnMeshNode.h"

namespace UBF
{
	void ReleaseCallback(intptr_t ThisPtr)
	{
		UE_LOG(LogUBF, Log, TEXT("Custom Node Released"));
	}

	void ExecuteCallback(intptr_t const ThisPtr,
	                     const char* NodeId,
	                     const FFI::ScopeID ScopeID,
	                     FFI::ArcExecutionContext* ContextPtr)
	{
		const FRegistryHandle::CreateInstanceFn CreateFn = reinterpret_cast<FRegistryHandle::CreateInstanceFn>(ThisPtr);
		
		const FCustomNode* Node = CreateFn(
			FString(strlen(NodeId), NodeId),
			ScopeID,
			FExecutionContextHandle(ContextPtr));
		
		Node->Execute();
	}

	template <std::derived_from<FCustomNode> T>
	void FRegistryHandle::Register(const char* Name) const
	{
		CreateInstanceFn CreateInstanceFn = [](
			const FString& NodeId,
			const FFI::ScopeID ScopeID,
			FExecutionContextHandle Context
		)
		{
			T* Node = new T;
			Node->Bind(NodeId, ScopeID, Context);
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
		Registry.Register<FSpawnMeshNode>("SpawnModel");
		Registry.Register<FSpawnMeshNode>("SpawnModelWithLODs");
		Registry.Register<FSetTextureSettingsNode>("SetTextureSettings");
		Registry.Register<FSetBlendshapeNode>("SetBlendshape");
		Registry.Register<FBindMeshes>("BindMeshes");
		Registry.Register<FExecuteBlueprint2Node>("ExecuteBlueprint");
		Registry.Register<FExecuteBlueprint2Node>("ExecuteBlueprint2");
		Registry.Register<FCreateMeshConfig>("CreateMeshConfig");
		Registry.Register<FFindRenderer>("FindRenderer");
		Registry.Register<FTransformPositionNode>("TransformPosition");
		Registry.Register<FTransformScaleNode>("TransformScale");
		Registry.Register<FMakePBRMaterial>("MakePBRMaterial");
		Registry.Register<FMakeDecalMaterial>("MakeDecalMaterial");
		Registry.Register<FMakeFurMaterial>("MakeFurMaterial");
		Registry.Register<FMakeHairMaterial>("MakeHairMaterial");
		Registry.Register<FMakeSkinMaterial>("MakeSkinMaterial");
		
		return Registry;
	}
}
