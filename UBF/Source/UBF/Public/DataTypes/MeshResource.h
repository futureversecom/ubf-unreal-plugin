#pragma once

class FMeshResource
{
public:
	FMeshResource() {}
	FMeshResource(const FString& ResourceID) : RawArtifactID(ResourceID)
	{
		// Id can either be in <id> or <id>/<mesh-name> formats
		if (RawArtifactID.Contains(TEXT("/")))
		{
			RawArtifactID.Split(TEXT("/"), &ArtifactID, &MeshName);
		}
	}
	FMeshResource(const FString& ResourceID, const FString& MeshName) : ArtifactID(ResourceID), MeshName(MeshName) {}

	FString RawArtifactID;
	FString ArtifactID;
	FString MeshName;
};
