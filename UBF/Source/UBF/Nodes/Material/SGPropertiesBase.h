// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CustomNode.h"

namespace UBF
{
    struct FShaderPropertyValue;
	
	class FSGPropertiesBase : public FCustomNode
	{
    public:
        virtual bool ExecuteSync() const override;

    protected:
        virtual void AddProperties(TMap<FString, FShaderPropertyValue>& Properties) const = 0;
        virtual FName GetBaseShaderName() const = 0;

        void AddColor(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddFloat(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddInt(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddBool(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName, const FString& PropertyName) const;
        void AddTexture(TMap<FString, FShaderPropertyValue>& PropertyMap, const FString& ResourceName,const FString& PropertyName, bool bIsNormal = false) const;
	};
}

