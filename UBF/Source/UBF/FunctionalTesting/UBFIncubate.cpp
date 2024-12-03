#pragma once

#include "CoreMinimal.h"
#include "DebugLog.h"
#include "Graph.h"
#include "Registry.h"
#include "ExecutionContext.h"
#include "GraphProvider.h"
#include "SubGraphResolver.h"
#include "UBFActor.h"
#include "UBFAPISubsystem.h"
#include "Misc/AutomationTest.h"
#include "UBFLog.h"

static constexpr int TestFlags = EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(UBFIncubate, "UBF.Incubate", TestFlags)

const char* Graph1Json = R"json(
{
  "tempId": "",
  "nodes": [
    {
      "id": "0NbRYs49aOZXZAKIQre_i",
      "type": "Entry",
      "inputs": [],
      "outputs": [
        {
          "name": "Exec",
          "type": "exec"
        }
      ],
      "position": {
        "x": -242,
        "y": 60
      }
    },
    {
      "id": "qzE4liS7oLk1k7eX_DEl3",
      "type": "DebugLog",
      "inputs": [
        {
          "name": "Exec",
          "type": "exec"
        },
        {
          "name": "Message",
          "type": "string",
          "value": ""
        },
        {
          "name": "Foobar",
          "type": "boolean",
          "value": false
        }
      ],
      "outputs": [
        {
          "name": "Exec",
          "type": "exec"
        }
      ],
      "position": {
        "x": 688,
        "y": -9
      }
    },
    {
      "id": "uDoazvu6SdTen9fvt3lu4",
      "type": "Append",
      "inputs": [
        {
          "name": "A",
          "type": "string",
          "value": ""
        },
        {
          "name": "B",
          "type": "string",
          "value": ", World"
        }
      ],
      "outputs": [
        {
          "name": "String",
          "type": "string"
        }
      ],
      "position": {
        "x": 267,
        "y": 364
      }
    },
    {
      "id": "R0TeotJFnRv5M5JAbfh7s",
      "type": "StringLiteral",
      "inputs": [
        {
          "name": "Value",
          "type": "string",
          "value": "Hello"
        }
      ],
      "outputs": [
        {
          "name": "String",
          "type": "string"
        }
      ],
      "position": {
        "x": -161,
        "y": 249
      }
    },
    {
      "id": "aKXk3OMas_XMCXC2xQRjd",
      "type": "FindSceneNodes",
      "inputs": [
        {
          "name": "Root",
          "type": "Transform",
          "value": null
        },
        {
          "name": "Filter",
          "type": "string",
          "value": ""
        }
      ],
      "outputs": [
        {
          "name": "Nodes",
          "type": "Array<Transform>"
        }
      ],
      "position": {
        "x": 713,
        "y": 292
      }
    },
    {
      "id": "tRHnmItwQ7uay2LMWYtBf",
      "type": "ToString",
      "inputs": [
        {
          "name": "Value",
          "type": "unknown",
          "value": null
        }
      ],
      "outputs": [
        {
          "name": "String",
          "type": "string"
        }
      ],
      "position": {
        "x": 1118,
        "y": 153
      }
    },
    {
      "id": "jW7ZLO5i3fbqqcabJEkGx",
      "type": "DebugLog",
      "inputs": [
        {
          "name": "Exec",
          "type": "exec"
        },
        {
          "name": "Message",
          "type": "string",
          "value": ""
        },
        {
          "name": "Foobar",
          "type": "boolean",
          "value": false
        }
      ],
      "outputs": [
        {
          "name": "Exec",
          "type": "exec"
        }
      ],
      "position": {
        "x": 1502,
        "y": -66
      }
    }
  ],
  "edges": [
    {
      "sourceID": "0NbRYs49aOZXZAKIQre_i",
      "sourceKey": "Exec",
      "targetID": "qzE4liS7oLk1k7eX_DEl3",
      "targetKey": "Exec"
    },
    {
      "sourceID": "R0TeotJFnRv5M5JAbfh7s",
      "sourceKey": "String",
      "targetID": "uDoazvu6SdTen9fvt3lu4",
      "targetKey": "A"
    },
    {
      "sourceID": "uDoazvu6SdTen9fvt3lu4",
      "sourceKey": "String",
      "targetID": "qzE4liS7oLk1k7eX_DEl3",
      "targetKey": "Message"
    },
    {
      "sourceID": "aKXk3OMas_XMCXC2xQRjd",
      "sourceKey": "Nodes",
      "targetID": "tRHnmItwQ7uay2LMWYtBf",
      "targetKey": "Value"
    },
    {
      "sourceID": "qzE4liS7oLk1k7eX_DEl3",
      "sourceKey": "Exec",
      "targetID": "jW7ZLO5i3fbqqcabJEkGx",
      "targetKey": "Exec"
    },
    {
      "sourceID": "tRHnmItwQ7uay2LMWYtBf",
      "sourceKey": "String",
      "targetID": "jW7ZLO5i3fbqqcabJEkGx",
      "targetKey": "Message"
    }
  ],
  "bindings": [],
  "resources": []
}
)json";

inline bool UBFIncubate::RunTest(const FString& Parameters)
{
  UWorld* World(UWorld::CreateWorld(EWorldType::Game, false));

  UE_LOG(LogUBF, Log, TEXT("Creating Registry..."));

  const UBF::FRegistryHandle Registry(UBF::FRegistryHandle::Default());

  UE_LOG(LogUBF, Log, TEXT("Loading Graph..."));

  UBF::FGraphHandle Graph;
  if (!UBF::FGraphHandle::Load(
    Registry,
    FString(Graph1Json),
    Graph
  ))
  {
    return false;
  }

  UE_LOG(LogUBF, Log, TEXT("Executing Graph..."));
  const FOnComplete OnComplete;
  auto OnCompleteFunc = [OnComplete]
  {
    OnComplete.ExecuteIfBound();
    UE_LOG(LogUBF, Log, TEXT("Completed Executing Graph"));
  };
  const auto Root = World->SpawnActor<AUBFActor>();
  FDefaultGraphProvider* GraphProvider = new FDefaultGraphProvider();
  GraphProvider->RegisterGraph("", Graph);
  ISubGraphResolver* SubGraphResolver = new FDefaultSubGraphResolver();
  UBF::FExecutionContextHandle Context;
  TMap<FString, UBF::FDynamicHandle> Inputs;
  Graph.Execute("", Root->GetRootComponent(), GraphProvider, SubGraphResolver, Inputs, OnCompleteFunc, Context);

  return true;
}
