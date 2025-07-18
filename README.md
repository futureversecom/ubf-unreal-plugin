# Universal Blueprint Framework - Unreal Interpreter

**A UBF Interpreter for Unreal by [Futureverse](https://www.futureverse.com)**

This SDK provides a runtime interpreter for the **Universal Blueprint Framework (UBF)**, designed specifically for use in Unreal-based applications. It allows developers to execute UBF Blueprints inside Unreal levels using a flexible, engine-agnostic system. The Interpreter is an essential part of the broader UBF ecosystem.

> For more information about the Universal Blueprint Framework, its authoring tools, and the full ecosystem, please visit the [UBF Open Standard](https://ubfstandard.com/) and the [Futureverse Developer Documentation](https://docs.futureverse.com/build-an-asset/asset-creation).

## Installation

Requires unreal engine 5.3 and above c++ project.

1. Download latest release
2. Find or create the 'Plugins' folder in your unreal engine project
3. Extract the zip into the plugins folder.
4. Compile or Build your unreal engine project.
5. Open your project and enable the 'UBF' plugin in the plugins tab of the unreal editor.

## Overview

The Unreal UBF Interpreter converts Blueprint’s engine‑agnostic instructions — defined in compliance with the [UBF Open Standard](https://ubfstandard.com/) — into native API calls, commands, and data formats that Unreal can understand and execute. This Interpreter sits on the execution side of the UBF stack. It is completely decoupled from authoring (UBF Studio) and orchestration (Futureverse UBF Controller). For more on those, see [Creating UBF Content](https://docs.futureverse.com/build-an-asset/asset-creation/blueprint-creation/ubf-studio-and-ubf-projects) and the [Unreal Futureverse UBF Controller](https://github.com/futureversecom/ubf-unreal-futureverse-controller).

The Interpreter is intentionally narrow in responsibility: it takes a compiled UBF Blueprint as input and simply executes it within a Unreal level, without any awareness of the Blueprint’s intent or the broader system context. They are entirely blind to external concerns—such as equipment trees, blockchain concepts, or orchestration logic—which are handled elsewhere. This isolation makes the Interpreter highly generic, allowing it to stand alone and serve an open-ended range of use cases across various applications.
​
## Core Responsibilities

While the Interpreter operate within a narrow and clearly defined scope, its role in the UBF Framework is both foundational and critical. It fulfills several core responsibilities, including but not limited to:

* Deserializing UBF Artifacts: Ensuring that the data structures are well-formed, valid, and ready for use.
* Implementing Node Translations: Providing behavior for each UBF node, in accordance with the standard specification.
* Managing Execution Flow: Handling the logical progression from one node to the next within a Blueprint’s execution, ensuring that dependencies and sequence are respected.
* Managing Data Flow Between Nodes: Coordinating how data is passed and transformed from one node to another.

The Interpreter is not responsible for:

* Resolving or downloading referenced artifacts
* Deciding when or where a blueprint should execute
* Determining what inputs are fed into a blueprint
* Mapping blueprints or catalogs to specific assets or their variants

These concerns fall outside the Interpreter’s scope and can be handled by the [Unreal Futureverse UBF Controller](https://github.com/futureversecom/ubf-unreal-futureverse-controller).

## Using The Interpreter

Under UBF plugins content folder see Levels/LocalBlueprintDemo to see the interpreter in action.

This is driven by UI/W_SimpleBlueprintTestUI which simply registers catalogs selected from file explorer and lets you run any blueprint from a loaded catalog.

<img width="666" height="187" alt="image" src="https://github.com/user-attachments/assets/4a716646-c0c0-4a53-8b8d-6e35ea64379e" />

<img width="776" height="379" alt="image" src="https://github.com/user-attachments/assets/4dc35b8d-054f-4941-861f-7bc39b10eb53" />



