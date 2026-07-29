#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BloodPaintBPLibrary.generated.h"

UCLASS()
class BLOODPAINT_API UBloodPaintBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** 
	 * 从骨骼网格体的命中结果计算UV坐标和三角形面积（用于拉伸校正）。
	 * 需要骨骼网格体启用 PerPolyCollision。
	 */
	UFUNCTION(BlueprintCallable, Category = "BloodPaint|UV")
	static bool GetHitUV(const FHitResult& HitResult, FVector2D& OutUV, float& OutTriangleArea3D, float& OutTriangleArea2D);

	/** 
	 * 计算UV拉伸比 = sqrt(UV面积) / sqrt(3D面积)。
	 * 值越大说明该区域UV越"拉伸"，绘制贴花时应缩小以补偿。
	 */
	UFUNCTION(BlueprintPure, Category = "BloodPaint|UV")
	static float CalculateUVStretch(float TriangleArea3D, float TriangleArea2D);

	/** 
	 * 便捷函数：从命中结果直接获取经过拉伸校正的UV坐标和拉伸比。
	 */
	UFUNCTION(BlueprintCallable, Category = "BloodPaint|UV")
	static bool GetAdjustedHitUV(const FHitResult& HitResult, FVector2D& OutUV, float& OutStretch);

	/** 
	 * 在渲染目标上绘制血液贴花（接口框架，日志+参数说明）。
	 * 实际绘制需在蓝图中配合材质实例 + Draw Material to Render Target 节点实现。
	 */
	UFUNCTION(BlueprintCallable, Category = "BloodPaint|Draw")
	static void DrawBloodDecalAtUV(UTextureRenderTarget2D* RenderTarget, UTexture2D* BloodTexture,
		FVector2D UVPosition, float DecalSize, float UVStretch, float Rotation, float Opacity);
};
