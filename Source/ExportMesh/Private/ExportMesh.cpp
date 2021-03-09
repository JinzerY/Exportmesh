// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportMesh.h"
#include "ExportMeshStyle.h"
#include "ExportMeshCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "ExportLibrary.h"

static const FName ExportMeshTabName("ExportMesh");

#define LOCTEXT_NAMESPACE "FExportMeshModule"

void FExportMeshModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FExportMeshStyle::Initialize();
	FExportMeshStyle::ReloadTextures();

	FExportMeshCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FExportMeshCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FExportMeshModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FExportMeshModule::RegisterMenus));
}

void FExportMeshModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FExportMeshStyle::Shutdown();

	FExportMeshCommands::Unregister();
}

void FExportMeshModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	//FText DialogText = FText::Format(
	//						LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
	//						FText::FromString(TEXT("FExportMeshModule::PluginButtonClicked()")),
	//						FText::FromString(TEXT("ExportMesh.cpp"))
	//				   );
	//FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	//StaticMesh'/Engine/EngineMeshes/SM_MatPreviewMesh_01.SM_MatPreviewMesh_01'


	//StaticMesh'/Engine/EngineMeshes/Cube.Cube'
	FString AssetPath = FString(TEXT("/Engine/EngineMeshes/Cube.Cube"));
	UStaticMesh* StaticMeshLoaded = LoadObject<UStaticMesh>(nullptr, *AssetPath);
	UExportLibrary::ExportStaticMesh(StaticMeshLoaded);
	
	UExportLibrary::ExportCameraInfo();
	
}



void FExportMeshModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FExportMeshCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FExportMeshCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExportMeshModule, ExportMesh)