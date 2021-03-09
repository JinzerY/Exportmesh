#include "ExportMesh.h"
#include "ExportLibrary.h"

#include "RawMesh.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

#include <fstream>


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

bool UExportLibrary::ExportStaticMesh(UStaticMesh* StaticMesh)
{
	if (StaticMesh->GetSourceModels().Num() == 0)
	{
		return false;
	}

	FStaticMeshSourceModel& MeshModel = StaticMesh->GetSourceModel(0);

	FRawMesh RawMeshData;
	MeshModel.LoadRawMesh(RawMeshData);

	//export to json
	TSharedRef<FJsonObject> JsonObj = SaveRawMeshAsJsonObj(RawMeshData);
	FString JsonPath;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonPath);
	bool Success = FJsonSerializer::Serialize(JsonObj, Writer);

	if (Success)
	{
		std::ofstream fout("E:/Model/modelSave.txt");
		fout.write(TCHAR_TO_ANSI(*JsonPath), JsonPath.Len());
		fout.close();
	}

	//export to binary
	//std::ofstream foutBinary("E:/Model/modelSave.m3d", std::ofstream::binary);
	//size_t PositionSize = RawMeshData.VertexPositions.Num() * sizeof(FVector);
	//void* Data = RawMeshData.VertexPositions.GetData();

	//foutBinary << PositionSize;
	//for (size_t VIndex = 0; VIndex < PositionSize; VIndex++)
	//{
	//	foutBinary << RawMeshData.VertexPositions[VIndex].X;
	//	foutBinary << RawMeshData.VertexPositions[VIndex].Y;
	//	foutBinary << RawMeshData.VertexPositions[VIndex].Z;
	//}

	//size_t IndexNum = RawMeshData.WedgeIndices.Num();
	//foutBinary << IndexNum;
	//{
	//	for (size_t IIndex = 0; IIndex < IndexNum; IIndex++)
	//		foutBinary << RawMeshData.WedgeIndices[IIndex];
	//}

	//foutBinary.close();

	UE_LOG(LogExportMesh, Display, TEXT("*** export success!"));

	return true;
}

bool UExportLibrary::ExportCameraInfo()
{
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
		std::ofstream fout("E:/Model/CameraSave.txt");
		fout.write(TCHAR_TO_ANSI(*JsonPath), JsonPath.Len());
		fout.close();
	}
	
	return true;
}
