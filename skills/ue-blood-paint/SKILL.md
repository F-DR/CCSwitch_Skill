---
name: ue-blood-paint
description: "UE5 BloodPaint插件编译技能——在UE 5.7项目中编译血液绘制插件的完整流程。使用当用户需要：(1)编译BloodPaint插件，(2)在UE 5.7项目中编译自定义C++插件，(3)了解RunUAT命令行编译流程。引擎路径：D:\UEEpic\UE_5.7。"
---

# UE Blood Paint - 编译技能

编译 BloodPaint 插件（在骨骼网格体角色身体上绘制血液贴花的 UE5 C++ 插件）的流程说明。

## 插件目录结构

BloodPaint 插件应位于 UE 项目的 `Plugins/BloodPaint/` 目录下：

```
项目目录/
└── Plugins/
    └── BloodPaint/
        ├── BloodPaint.uplugin
        ├── Content/
        ├── Resources/
        └── Source/BloodPaint/
            ├── BloodPaint.Build.cs
            ├── Public/BloodPaintBPLibrary.h
            └── Private/BloodPaintBPLibrary.cpp
```

## 编译方式

### 方式一：命令行编译

```powershell
# 引擎路径：D:\UEEpic\UE_5.7
# 项目路径：由用户指定
D:\UEEpic\UE_5.7\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin `
    -Plugin="<项目路径>\Plugins\BloodPaint\BloodPaint.uplugin" `
    -Package="<项目路径>\Plugins\BloodPaint\Binaries" `
    -TargetPlatforms=Win64
```

### 方式二：IDE编译

在 Visual Studio / Rider 中打开项目 `.sln`，编译 `Editor` Target（Development Editor配置），插件会自动编译。

## 在 .uproject 中启用

在项目的 `.uproject` 文件的 `Plugins` 数组中添加：

```json
{
    "Name": "BloodPaint",
    "Enabled": true
}
```

## 引擎信息

- 引擎版本：UE 5.7
- 引擎安装路径：`D:\UEEpic\UE_5.7`
- 引擎模块依赖：Core, CoreUObject, Engine

## 蓝图函数库 API

编译成功后，蓝图中可调用以下4个函数：

| 函数 | 类型 | 说明 |
|---|---|---|
| `GetHitUV` | BlueprintCallable | 从骨骼网格体的FHitResult计算命中UV（需PerPolyCollision） |
| `CalculateUVStretch` | BlueprintPure | 拉伸比 = sqrt(UV面积) / sqrt(3D面积)，校正贴花大小 |
| `GetAdjustedHitUV` | BlueprintCallable | 直接获取校正后的UV |
| `DrawBloodDecalAtUV` | BlueprintCallable | 在RenderTarget上绘制血液贴花（配合材质实例使用） |

## 核心技术要点

- **PerPolyCollision**：骨骼网格体必须启用此选项，否则 FaceIndex 无效
- **骨骼动画兼容**：使用 `GetSkinnedVertexPosition` 而非CPU蒙皮
- **UV拉伸校正**：通过面积比确保各UV密度区域的贴花大小一致
