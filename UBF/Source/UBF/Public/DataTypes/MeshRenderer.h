// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

namespace UBF
{
	/**
	 * Contains all data needed for the 'MeshRenderer' datatype in UBF
	 * Basically wrapper around MeshComponent that spawn mesh generates
	 */
	class FMeshRenderer
	{
	public:
		FMeshRenderer(UMeshComponent* MeshComponent) : MeshComponent(MeshComponent) {}

		USkeletalMeshComponent* GetSkeletalMesh() const {return Cast<USkeletalMeshComponent>(MeshComponent.Get());} 
		UStaticMeshComponent* GetStaticMesh() const {return Cast<UStaticMeshComponent>(MeshComponent.Get());} 
		UMeshComponent* GetMesh() const {return MeshComponent.Get();} 
		FString GetName() const {return GetMesh() ? GetMesh()->GetName() : FString("NullMeshRenderer");} 
	private:
		TWeakObjectPtr<UMeshComponent> MeshComponent;
	};
}

