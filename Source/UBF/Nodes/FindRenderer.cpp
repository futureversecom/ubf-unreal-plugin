#include "FindRenderer.h"

#include "DataTypes/MeshRenderer.h"

bool UBF::FFindRenderer::ExecuteSync() const
{
	UE_LOG(LogUBF, Verbose, TEXT("[FindRenderer] Executing Node"));
	
	TArray<FMeshRenderer*> Mesh;
	if (!TryReadInputArray("Array", Mesh))
	{
		UE_LOG(LogUBF, Warning, TEXT("[FindRenderer] No Array input found"));
		TriggerNext();
		return true;
	}

	FString Name;
	if (!TryReadInputValue("Name", Name))
	{
		UE_LOG(LogUBF, Warning, TEXT("[FindRenderer] No Name input found"));
		TriggerNext();
		return true;
	}

	FMeshRenderer* OutMesh = nullptr;

	for (auto MeshRenderer : Mesh)
	{
		UE_LOG(LogUBF, Verbose, TEXT("[FindRenderer] Checking MeshRender name {%s} with filter {%s}"), *MeshRenderer->GetName(), *Name);
		if (MeshRenderer->GetName().Contains(Name))
		{
			OutMesh = MeshRenderer;
		}
	}

	if (!OutMesh)
	{
		UE_LOG(LogUBF, Warning, TEXT("[FindRenderer] No valid mesh was found from %d inputs with filter %s"), Mesh.Num(), *Name);
	}

	WriteOutput("Renderer", FDynamicHandle::ForeignHandled(new FMeshRenderer(OutMesh ? OutMesh->GetMesh() : nullptr)));
	return true;
}
