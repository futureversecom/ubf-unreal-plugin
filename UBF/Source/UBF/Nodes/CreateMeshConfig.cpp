// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "CreateMeshConfig.h"

#include "glTFRuntimeAsset.h"
#include "GraphProvider.h"
#include "UBFMeshConfigSettings.h"
#include "DataTypes/MeshConfig.h"
#include "GlobalArtifactProvider/CachedMesh.h"
#include "Util/BoneRemapperUtil.h"

void UBF::FCreateMeshConfig::ExecuteAsync() const
{
	FString ResourceID;
	if (!TryReadInputValue("Resource", ResourceID))
	{
		UBF_LOG(Warning, TEXT("[CreateMeshConfig] failed to read MeshResource input"));
		
		const UUBFMeshConfigSettings* Settings = GetDefault<UUBFMeshConfigSettings>();
		check(Settings);
		Complete(Settings->GetMeshConfigData());
		return;
	}

	FString ConfigKey;
	TryReadInputValue("ConfigOverrideKey", ConfigKey);

	UBF_LOG(Verbose, TEXT("[CreateMeshConfig] Finding MeshConfig for: %s"), *ConfigKey);

	// if config key is empty, it will use default config
	const UUBFMeshConfigSettings* Settings = GetDefault<UUBFMeshConfigSettings>();
	check(Settings);
	FMeshConfigData MeshConfigData = Settings->GetMeshConfigData(ConfigKey);

	if (MeshConfigData.SkeletalMeshConfig.Skeleton)
	{
		Complete(MeshConfigData);
		return;
	}

	if (!MeshConfigData.SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.IsBound())
	{
		MeshConfigData.SkeletalMeshConfig.SkeletonConfig.BoneRemapper.Remapper.BindDynamic(NewObject<UBoneRemapperUtil>(), &UBoneRemapperUtil::RemapFormatBoneName);	
	}
	
	GetContext().GetUserData()->ExecutionSetConfig->GetMesh(ResourceID, FMeshImportSettings(MeshConfigData.RuntimeConfig)).Next([this, MeshConfigData](const FLoadMeshResult& Result)
	{
		FMeshConfigData MutableMeshConfig = MeshConfigData;
		
		if (Result.bSuccess)
		{
			MutableMeshConfig.SkeletalMeshConfig.Skeleton = Result.Value->LoadSkeleton(0, MeshConfigData.SkeletalMeshConfig.SkeletonConfig);
			MutableMeshConfig.SkeletalMeshConfig.bOverwriteRefSkeleton = true;
		}
		
		Complete(MutableMeshConfig);
	});
}

void UBF::FCreateMeshConfig::Complete(const FMeshConfigData& MeshConfigData) const
{
	WriteOutput("MeshConfig", FDynamicHandle::ForeignHandled(new FMeshConfig(MeshConfigData)));
	TriggerNext();
	CompleteAsyncExecution();
}
