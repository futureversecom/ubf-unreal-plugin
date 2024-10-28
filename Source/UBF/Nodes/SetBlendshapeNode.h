// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomNode.h"

namespace UBF
{
	class UBF_API FSetBlendshapeNode final : public FCustomNode
	{
	protected:
		virtual void ExecuteAsync() const override;
	};
}

