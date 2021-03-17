#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExportLibrary.generated.h"


USTRUCT()
struct FExpotMeshVertex
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector Normal;

	UPROPERTY()
	FVector2D Uv;
};

USTRUCT()
struct FExportData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FString> Format;

	UPROPERTY()
	int32 VertexNum;

	UPROPERTY()
	TArray<FExpotMeshVertex> Vertices;

	UPROPERTY()
	int32 IndexNum;

	UPROPERTY()
	TArray<int32> Indices;
};


UCLASS()
class UExportLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	static bool ExportStaticMesh(FString& AssetPath, FString& SavePath, bool JsonOrBinary = true);

	UFUNCTION(BlueprintPure)
	static bool ExportCameraInfo(FString& SavePath);

};
