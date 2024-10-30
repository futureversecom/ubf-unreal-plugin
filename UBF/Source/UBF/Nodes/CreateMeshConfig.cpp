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
	
	GetContext().GetGraphProvider()->GetMeshResource(GetGraphId(), ResourceID).Next([this, MeshConfigData](const FLoadMeshResult Result)
	{
		FMeshConfigData MutableMeshConfig = MeshConfigData;
		
		if (Result.Result.Key && Result.Result.Value)
		{
			MutableMeshConfig.SkeletalMeshConfig.Skeleton = Result.Result.Value->LoadSkeleton(0, MeshConfigData.SkeletalMeshConfig.SkeletonConfig);
			MutableMeshConfig.SkeletalMeshConfig.bOverwriteRefSkeleton = true;
		}
		
		Complete(MeshConfigData);
	});
}

void UBF::FCreateMeshConfig::Complete(const FMeshConfigData& MeshConfigData) const
{
	WriteOutput("Config", FDynamicHandle::ForeignHandled(new FMeshConfig(MeshConfigData)));
	TriggerNext();
	CompleteAsyncExecution();
}
