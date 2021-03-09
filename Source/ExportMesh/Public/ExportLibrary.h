#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExportLibrary.generated.h"

UCLASS()
class UExportLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static bool ExportStaticMesh(UStaticMesh* StaticMesh);

	UFUNCTION(BlueprintPure)
	static bool ExportCameraInfo();
};
