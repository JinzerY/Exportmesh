// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportMeshCommands.h"

#define LOCTEXT_NAMESPACE "FExportMeshModule"

void FExportMeshCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ExportMesh", "Execute ExportMesh action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
