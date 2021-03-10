#include "ExportLibrary.h"
#include "ExportMesh.h"
#include "RawMesh.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include <fstream>
#include "Misc/FileHelper.h"
#include "JsonObjectConverter.h"

#include "GameFrameWork/PlayerController.h"

DECLARE_LOG_CATEGORY_CLASS(LogExportMesh, Log, All)

UExportLibrary::UExportLibrary(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}


TSharedRef<FJsonObject> SaveRawMeshAsJsonObj(FRawMesh& RawMesh)
{
	TSharedRef<FJsonObject> JObj = MakeShareable(new FJsonObject());

	int32 VertexNum = RawMesh.VertexPositions.Num();
	JObj->SetStringField(FString(TEXT("VertexNum")), FString::FromInt(VertexNum));

	TArray<TSharedPtr<FJsonValue>> JvArray;
	for (int32 pIndex = 0; pIndex < RawMesh.VertexPositions.Num(); pIndex++)
	{
		JvArray.Add(MakeShareable(new FJsonValueNumber(RawMesh.VertexPositions[pIndex].X)));
		JvArray.Add(MakeShareable(new FJsonValueNumber(RawMesh.VertexPositions[pIndex].Y)));
		JvArray.Add(MakeShareable(new FJsonValueNumber(RawMesh.VertexPositions[pIndex].Z)));

	}
	FString  FileName = FString(TEXT("Vertices"));
	JObj->SetArrayField(FileName, JvArray);

	int32 IndexNum = RawMesh.WedgeIndices.Num();
	JObj->SetStringField(FString(TEXT("IndexNum")), FString::FromInt(IndexNum));
	TArray<TSharedPtr<FJsonValue>> JIArray;
	for (int32 IIndex = 0; IIndex < RawMesh.WedgeIndices.Num(); IIndex++)
	{
		JIArray.Add(MakeShareable(new FJsonValueString(FString::FromInt(RawMesh.WedgeIndices[IIndex]))));
	}
	JObj->SetArrayField(FString(TEXT("Index")), JIArray);

	return JObj;
}

FExportData* SaveMeshLODResourceAsExportData(FStaticMeshLODResources& LODResource)
{
	FExportData* ExportData = new FExportData();

	ExportData->Format.Add(FString(TEXT("Position")));

	int32 VertexNum = LODResource.GetNumVertices();
	ExportData->VertexNum = VertexNum;
	for (int32 VIndex = 0; VIndex < VertexNum; ++VIndex)
	{
		FVector Pos = LODResource.VertexBuffers.PositionVertexBuffer.VertexPosition(VIndex);
		FExpotMeshVertex Vertex;
		Vertex.Position = Pos;
		ExportData->Vertices.Add(Vertex);
	}

	int32 IndexNum = LODResource.IndexBuffer.GetNumIndices();
	ExportData->IndexNum = IndexNum;
	for (int32 IIndex = 0; IIndex < IndexNum; ++IIndex)
	{
		ExportData->Indices.Add(LODResource.IndexBuffer.GetIndex(IIndex));
	}

	return ExportData;
}

void ExportMeshDataToBinary(const FExportData* JsonObj, FString SavePath)
{
	//export to binary
	std::ofstream foutBinary(TCHAR_TO_ANSI(*SavePath), std::ofstream::binary);
	int32 VertexNum = JsonObj->VertexNum;

	foutBinary << VertexNum;
	for (size_t VIndex = 0; VIndex < VertexNum; VIndex++)
	{
		foutBinary << JsonObj->Vertices[VIndex].Position.X;
		foutBinary << JsonObj->Vertices[VIndex].Position.Y;
		foutBinary << JsonObj->Vertices[VIndex].Position.Z;
	}

	int32 IndexNum = JsonObj->IndexNum;
	foutBinary << IndexNum;
	{
		for (size_t IIndex = 0; IIndex < IndexNum; IIndex++)
			foutBinary << JsonObj->Indices[IIndex];
	}

	foutBinary.close();
}

bool UExportLibrary::ExportStaticMesh(FString& AssetPath, FString& SavePath, bool JsonOrBinary)
{
	UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *AssetPath);

	if (!LoadedMesh->RenderData)
	{
		return false;
	}

	if (!LoadedMesh->RenderData->LODResources.Num())
	{
		return false;
	}

	FStaticMeshLODResources& LodResource = LoadedMesh->RenderData->LODResources[0];
	FExportData* JsonObj = SaveMeshLODResourceAsExportData(LodResource);

	//true:export for json ; false: export binary .m3d
	if (JsonOrBinary)
	{
		FString JsonContent;
		if (FJsonObjectConverter::UStructToJsonObjectString(*JsonObj, JsonContent))
		{
			FFileHelper::SaveStringToFile(JsonContent, *SavePath);
		}
	}
	else
	{
		ExportMeshDataToBinary(JsonObj, SavePath);
	}


	UE_LOG(LogExportMesh, Display, TEXT("*** export success!"));

	return true;
}

bool UExportLibrary::ExportCameraInfo(FString& SavePath)
{
	if (!GEditor)
	{
		UE_LOG(LogExportMesh, Warning, TEXT("*** can not export camera info (not in editor mode)!"));
		return false;
	}

	FViewport* Viewport = GEditor->GetActiveViewport();
	FViewportClient* ViewportClient = Viewport->GetClient();
	
	FEditorViewportClient* EditorViewClient = static_cast<FEditorViewportClient*>(ViewportClient);
	
	if (EditorViewClient)
	{
		UE_LOG(LogExportMesh, Display, TEXT("*** Get EditorViewClient success!"));
	}

	FVector CamLoc = EditorViewClient->GetViewLocation();
	FRotator CamRot = EditorViewClient->GetViewRotation();
	FVector CamTarget = EditorViewClient->GetLookAtLocation();
	float FOv = EditorViewClient->ViewFOV;
	float AspectRatio = EditorViewClient->AspectRatio;

	TSharedRef<FJsonObject> JsonObj = MakeShareable(new FJsonObject());

	//set properties
	JsonObj->SetStringField(FString(TEXT("CameraLoc")), CamLoc.ToString());
	JsonObj->SetStringField(FString(TEXT("CameraRot")), CamRot.ToString());
	JsonObj->SetStringField(FString(TEXT("CameraLookAt")), CamTarget.ToString());
	JsonObj->SetStringField(FString(TEXT("FOv")), FString::SanitizeFloat(FOv));
	JsonObj->SetStringField(FString(TEXT("AspectRatio")), FString::SanitizeFloat(AspectRatio));


	FString JsonPath;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonPath);
	bool Success = FJsonSerializer::Serialize(JsonObj, Writer);
	if (Success)
	{
		std::ofstream fout(TCHAR_TO_ANSI(*SavePath));
		fout.write(TCHAR_TO_ANSI(*JsonPath), JsonPath.Len());
		fout.close();
	}
	
	return true;
}
