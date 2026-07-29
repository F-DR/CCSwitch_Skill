#include "BloodPaintBPLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"

bool UBloodPaintBPLibrary::GetHitUV(const FHitResult& HitResult, FVector2D& OutUV, float& OutTriangleArea3D, float& OutTriangleArea2D)
{
	USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(HitResult.Component.Get());
	if (!Mesh || !Mesh->GetSkeletalMeshRenderData())
	{
		return false;
	}

	FSkeletalMeshRenderData* RenderData = Mesh->GetSkeletalMeshRenderData();
	FSkeletalMeshLODRenderData& LODData = RenderData->LODRenderData[0];
	FRawStaticIndexBuffer16or32Interface* Indices = LODData.MultiSizeIndexContainer.GetIndexBuffer();

	const int32 FaceIndex = HitResult.FaceIndex;
	if (FaceIndex * 3 + 2 >= Indices->Num() || FaceIndex * 3 < 0)
	{
		return false;
	}

	const int32 Index0 = Indices->Get(FaceIndex * 3 + 0);
	const int32 Index1 = Indices->Get(FaceIndex * 3 + 1);
	const int32 Index2 = Indices->Get(FaceIndex * 3 + 2);

	const FVector3f Pos0 = Mesh->GetSkinnedVertexPosition(LODData, LODData.SkinWeightVertexBuffer, Index0);
	const FVector3f Pos1 = Mesh->GetSkinnedVertexPosition(LODData, LODData.SkinWeightVertexBuffer, Index1);
	const FVector3f Pos2 = Mesh->GetSkinnedVertexPosition(LODData, LODData.SkinWeightVertexBuffer, Index2);

	const FVector2f UV0 = Mesh->GetVertexUV(Index0, 0);
	const FVector2f UV1 = Mesh->GetVertexUV(Index1, 0);
	const FVector2f UV2 = Mesh->GetVertexUV(Index2, 0);

	const FVector LocalHitPos = Mesh->GetComponentTransform().InverseTransformPosition(HitResult.Location);

	const FVector BaryCoords = FMath::ComputeBaryCentric2D(LocalHitPos, (FVector)Pos0, (FVector)Pos1, (FVector)Pos2);

	OutUV.X = (BaryCoords.X * UV0.X) + (BaryCoords.Y * UV1.X) + (BaryCoords.Z * UV2.X);
	OutUV.Y = (BaryCoords.X * UV0.Y) + (BaryCoords.Y * UV1.Y) + (BaryCoords.Z * UV2.Y);

	const FVector3f Side1_3D = Pos1 - Pos0;
	const FVector3f Side2_3D = Pos2 - Pos0;
	const FVector3f CrossProduct_3D = FVector3f::CrossProduct(Side1_3D, Side2_3D);
	OutTriangleArea3D = 0.5f * CrossProduct_3D.Size();

	const FVector2f Side1_2D = UV1 - UV0;
	const FVector2f Side2_2D = UV2 - UV0;
	const float CrossProduct_2D = Side1_2D.X * Side2_2D.Y - Side1_2D.Y * Side2_2D.X;
	OutTriangleArea2D = FMath::Abs(CrossProduct_2D) * 0.5f;

	return true;
}

float UBloodPaintBPLibrary::CalculateUVStretch(float TriangleArea3D, float TriangleArea2D)
{
	if (TriangleArea3D <= KINDA_SMALL_NUMBER || TriangleArea2D <= KINDA_SMALL_NUMBER)
	{
		return 1.0f;
	}

	const float Stretch = FMath::Sqrt(TriangleArea2D) / FMath::Sqrt(TriangleArea3D);
	return Stretch;
}

bool UBloodPaintBPLibrary::GetAdjustedHitUV(const FHitResult& HitResult, FVector2D& OutUV, float& OutStretch)
{
	float Area3D = 0.0f;
	float Area2D = 0.0f;

	if (!GetHitUV(HitResult, OutUV, Area3D, Area2D))
	{
		OutStretch = 1.0f;
		return false;
	}

	OutStretch = CalculateUVStretch(Area3D, Area2D);
	return true;
}

void UBloodPaintBPLibrary::DrawBloodDecalAtUV(UTextureRenderTarget2D* RenderTarget, UTexture2D* BloodTexture,
	FVector2D UVPosition, float DecalSize, float UVStretch, float Rotation, float Opacity)
{
	if (!RenderTarget || !BloodTexture)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("BloodPaint: Draw at UV(%.3f, %.3f) Size=%.2f Stretch=%.3f Rot=%.1f Opacity=%.2f"),
		UVPosition.X, UVPosition.Y, DecalSize, UVStretch, Rotation, Opacity);
}
