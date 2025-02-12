#include "FindRenderer.h"

#include "DataTypes/MeshRenderer.h"

bool UBF::FFindRenderer::ExecuteSync() const
{
	UBF_LOG(Verbose, TEXT("[FindRenderer] Executing Node"));
	
	TArray<FMeshRenderer*> Mesh;
	if (!TryReadInputArray("Array", Mesh))
	{
		UBF_LOG(Warning, TEXT("[FindRenderer] No Array input found"));
		TriggerNext();
		return true;
	}

	FString Name;
	if (!TryReadInputValue("Name", Name))
	{
		UBF_LOG(Warning, TEXT("[FindRenderer] No Name input found"));
		TriggerNext();
		return true;
	}

	FMeshRenderer* OutMesh = nullptr;

	for (auto MeshRenderer : Mesh)
	{
		UBF_LOG(Verbose, TEXT("[FindRenderer] Checking MeshRender name {%s} with filter {%s}"), *MeshRenderer->GetName(), *Name);
		if (MeshRenderer->GetName().Contains(Name))
		{
			OutMesh = MeshRenderer;
		}
	}

	if (!OutMesh)
	{
		UBF_LOG(Warning, TEXT("[FindRenderer] No valid mesh was found from %d inputs with filter %s"), Mesh.Num(), *Name);
	}

	WriteOutput("Renderer", FDynamicHandle::ForeignHandled(new FMeshRenderer(OutMesh ? OutMesh->GetMesh() : nullptr)));
	return true;
}
