---
name: ue-blood-paint
description: "UE5 BloodPaint插件——在骨骼网格体角色身体上动态绘制血液贴花的完整C++插件开发与编译。使用当用户需要：(1)创建或重建BloodPaint插件，(2)编译UE5自定义插件，(3)了解骨骼网格体UV命中计算、蒙皮顶点位置获取、UV拉伸校正等血液绘制技术。引擎路径：D:\UEEpic\UE_5.7，项目路径：D:\UEPJ\UEcpp。"
---

# UE Blood Paint

在角色身体上绘制血液贴花的UE5插件。核心原理来自社区方案：
通过 GetSkinnedVertexPosition 获取蒙皮顶点位置 -> 重心坐标插值计算命中点UV -> 面积比校正UV拉伸。

## 插件源码位置

插件源码以项目模板形式存储在 ssets/BloodPaint/ 目录下。

## 使用流程

### 1. 将插件复制到项目

将 ssets/BloodPaint/ 整个文件夹复制到目标UE项目的 Plugins/ 目录下。

### 2. 在 .uproject 中启用插件

在项目的 .uproject 文件的 Plugins 数组中添加：

`json
{
    "Name": "BloodPaint",
    "Enabled": true
}
`

### 3. 编译（命令行）

`powershell
cd D:\UEPJ\UEcpp
D:\UEEpic\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="D:\UEPJ\UEcpp\Plugins\BloodPaint\BloodPaint.uplugin" -Package="D:\UEPJ\UEcpp\Plugins\BloodPaint\Binaries" -TargetPlatforms=Win64
`

或者在IDE中直接编译项目。

### 4. 编译（IDE）

在 Visual Studio / Rider 中打开 .sln，编译 UEcppEditor Target（Development Editor配置），插件会自动编译。

## 蓝图函数库 API

UBloodPaintBPLibrary 提供4个可在蓝图中调用的静态函数：

| 函数 | 类型 | 说明 |
|---|---|---|
| GetHitUV | BlueprintCallable | 从骨骼网格体的FHitResult计算命中UV。需要启用PerPolyCollision。输出UV坐标 + 3D三角形面积 + UV三角形面积 |
| CalculateUVStretch | BlueprintPure | 拉伸比 = sqrt(UV面积) / sqrt(3D面积)。用于校正不同UV密度区域的贴花大小 |
| GetAdjustedHitUV | BlueprintCallable | GetHitUV + CalculateUVStretch 一步到位 |
| DrawBloodDecalAtUV | BlueprintCallable | 在RenderTarget上绘制血液贴花。实际绘制需配合材质实例+Draw Material to Render Target节点 |

## 关键技术点

- **PerPolyCollision**: 骨骼网格体必须启用此选项，否则FaceIndex无效
- **骨骼动画兼容**: 使用 GetSkinnedVertexPosition 而非CPU蒙皮，性能远优于手动计算
- **UV拉伸校正**: 通过 sqrt(UV面积)/sqrt(3D面积) 计算拉伸比，确保脸部(高UV密度)和手臂(低UV密度)的贴花大小一致
- **引擎版本**: UE 5.7（引擎路径：D:\UEEpic\UE_5.7）

## 相关文件参考

- [BloodPaint.uplugin](assets/BloodPaint/BloodPaint.uplugin) - 插件描述文件
- [BloodPaintBPLibrary.h](assets/BloodPaint/Source/BloodPaint/Public/BloodPaintBPLibrary.h) - 蓝图函数库头文件
- [BloodPaintBPLibrary.cpp](assets/BloodPaint/Source/BloodPaint/Private/BloodPaintBPLibrary.cpp) - 核心实现（UV计算+拉伸校正）
- [BloodPaint.Build.cs](assets/BloodPaint/Source/BloodPaint/BloodPaint.Build.cs) - 模块构建配置
