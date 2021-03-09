// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ExportMeshStyle.h"

class FExportMeshCommands : public TCommands<FExportMeshCommands>
{
public:

	FExportMeshCommands()
		: TCommands<FExportMeshCommands>(TEXT("ExportMesh"), NSLOCTEXT("Contexts", "ExportMesh", "ExportMesh Plugin"), NAME_None, FExportMeshStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
