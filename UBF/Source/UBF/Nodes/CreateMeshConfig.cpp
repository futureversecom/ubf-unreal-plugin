#include "CreateMeshConfig.h"

#include "glTFRuntimeAsset.h"
#include "GraphProvider.h"
#include "UBFMeshConfigSettings.h"
#include "DataTypes/MeshConfig.h"

void UBF::FCreateMeshConfig::ExecuteAsync() const
{
	FString ResourceID;
	if (!TryReadInputValue("Resource", ResourceID))
	{
		UE_LOG(LogUBF, Warning, TEXT("[CreateMeshConfig] to read MeshResource input"));
		
		const UUBFMeshConfigSettings* Settings = GetDefault<UUBFMeshConfigSettings>();
		check(Settings);
		Complete(Settings->GetMeshConfigData());
		return;
	}
	
	const UUBFMeshConfigSettings* Settings = GetDefault<UUBFMeshConfigSettings>();
	check(Settings);
	FMeshConfigData MeshConfigData = Settings->GetMeshConfigData(ResourceID);
	
	GetContext().GetGraphProvider()->GetMeshResource(GetGraphId(), ResourceID).Next([this, MeshConfigData, ResourceID](const FLoadDataArrayResult& Result)
	{
		FMeshConfigData MutableMeshConfig = MeshConfigData;
		
		if (Result.Result.Key)
		{
			const auto MeshData = Result.Result.Value;
			
			FglTFRuntimeConfig LoaderConfig = MeshConfigData.RuntimeConfig;
					
			UglTFRuntimeAsset* Asset = NewObject<UglTFRuntimeAsset>();
			Asset->RuntimeContextObject = LoaderConfig.RuntimeContextObject;
			Asset->RuntimeContextString = LoaderConfig.RuntimeContextString;
			
			if (!Asset->LoadFromData(MeshData.GetData(), MeshData.Num(), LoaderConfig))
			{
				UE_LOG(LogUBF, Error, TEXT("[FCreateMeshConfig] Failed to Load Mesh from Data %s"), *ResourceID);
				Complete(MutableMeshConfig);
				
				return;
			}
			
			MutableMeshConfig.SkeletalMeshConfig.Skeleton = Asset->LoadSkeleton(0, MeshConfigData.SkeletalMeshConfig.SkeletonConfig);
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
