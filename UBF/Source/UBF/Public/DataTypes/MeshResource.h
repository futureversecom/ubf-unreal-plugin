#pragma once

class FMeshResource
{
public:
	FMeshResource() {}
	FMeshResource(const FString& ResourceID) : ArtifactID(ResourceID)
	{
		// Id can either be in <id> or <id>/<mesh-name> formats
		if (ArtifactID.Contains(TEXT("/")))
		{
			ArtifactID.Split(TEXT("/"), &ArtifactID, &MeshName);
		}
	}
	FMeshResource(const FString& ResourceID, const FString& MeshName) : ArtifactID(ResourceID), MeshName(MeshName) {}
	
	FString ArtifactID;
	FString MeshName;
};
