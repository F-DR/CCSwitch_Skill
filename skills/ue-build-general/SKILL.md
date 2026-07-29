---
name: ue-build-general
description: "通用 UE 5.7 插件/项目编译技能。当用户需要：(1)编译UE项目中的C++插件，(2)处理编译错误（缺少头文件、API变更、.uplugin文件损坏），(3)了解UE5命令行编译流程。引擎路径：D:\UEEpic\UE_5.7。"
---

# UE Build General — 通用 UE 5.7 编译技能

编译 UE 5.7 项目中的任意插件或整个工程。包含常见编译问题排查。

## 环境

- **引擎**: `D:\UEEpic\UE_5.7`
- **引擎版本**: UE 5.7
- **Build.bat**: `D:\UEEpic\UE_5.7\Engine\Build\BatchFiles\Build.bat`
- **UBT DLL**: `D:\UEEpic\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll`

## 通用构建命令

### 1. 编译整个工程（编辑器 Target）

```powershell
$Engine = "D:\UEEpic\UE_5.7"
$Project = "D:\UEPJ\UEcpp\UEcpp.uproject"
& "$Engine\Engine\Build\BatchFiles\Build.bat" UEcppEditor Win64 Development -Project="$Project" -WaitMutex -FromMsBuild
```

### 2. 编译指定插件

UBT 会扫描 `Plugins/` 下所有 `.uplugin`，编译整个 Editor target 时自动包含所有启用的插件。若要只构建某插件，使用 `-Plugin` 参数（部分引擎版本支持）：

```powershell
& "$Engine\Engine\Build\BatchFiles\Build.bat" UEcppEditor Win64 Development -Project="$Project" -Plugin="D:\path\to\Plugin.uplugin"
```

### 3. 编译游戏客户端 Target

```powershell
& "$Engine\Engine\Build\BatchFiles\Build.bat" UEcpp Win64 Development -Project="$Project"
```

## Target 对照

| Target 名称 | 说明 |
|---|---|
| `{Project}Editor` | 编辑器（开发用） |
| `{Project}` | 游戏客户端 |
| `{Project}Server` | 专用服务器 |

## 常见编译错误及修复

### 1. `.uplugin` 文件编码错误

**症状**: `JsonReaderException: '0xFF' is an invalid start of a value`
**原因**: 文件是 UTF-16 LE 编码（BOM: `FF FE`），UE 的 JSON 解析器只支持 UTF-8。
**修复**: 用 PowerShell 转为 UTF-8 无 BOM：

```powershell
$content = Get-Content -LiteralPath "Plugin.uplugin" -Encoding Unicode -Raw
[System.IO.File]::WriteAllText("Plugin.uplugin", $content, [System.Text.UTF8Encoding]::new($false))
```

### 2. `FSkeletalMeshRenderData` 未定义

**症状**: `error C2027: use of undefined type 'FSkeletalMeshRenderData'`
**原因**: UE 5.7 将该类移到了内部头文件，需手动 include。
**修复**: 添加 `#include "Rendering/SkeletalMeshRenderData.h"`

### 3. `GetSkinnedVertexPosition` 参数不匹配

**症状**: `error C2661: 'GetSkinnedVertexPosition': no overloaded function takes 3 arguments`
**原因**: UE 5.7 中该方法改为静态方法，签名变为 4 个参数。
**修复**:

```cpp
// 旧写法（UE 5.4-）
const FVector3f Pos = Mesh->GetSkinnedVertexPosition(LODData, SkinWeightBuffer, Index);

// 新写法（UE 5.7+）
const FVector3f Pos = USkeletalMeshComponent::GetSkinnedVertexPosition(Mesh, Index, LODData, SkinWeightBuffer);
```

### 4. `GetVertexUV` 返回值类型不匹配

**症状**: `error C2440: 'initializing': cannot convert from 'FVector2D' to 'FVector2f'`
**原因**: UE 5.7 中 `GetVertexUV` 返回 `FVector2D`（double），且已不在 `USkeletalMeshComponent` 上。
**修复**:

```cpp
// 旧写法
FVector2f UV = Mesh->GetVertexUV(VertexIndex, 0);

// 新写法（UE 5.7+）
FVector2f UV = LODData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
```

### 5. 插件目录结构要求

```
Plugins/<PluginName>/
├── <PluginName>.uplugin        # 插件描述文件（JSON，UTF-8 无 BOM）
├── Source/<PluginName>/
│   ├── <PluginName>.Build.cs   # 模块依赖配置
│   ├── Public/                 # 公开头文件
│   └── Private/                # 私有实现
├── Content/                    # 蓝图/材质等资源
└── Resources/                  # 图标等
```

### 6. `.uproject` 中启用插件

```json
{
  "Plugins": [
    {
      "Name": "YourPluginName",
      "Enabled": true
    }
  ]
}
```

## 编译调试技巧

1. **查看详细日志**: 检查 `%LOCALAPPDATA%\UnrealBuildTool\Log.txt`
2. **编辑器未关闭**: 如果 UE 编辑器正在运行，编译可能因文件锁定失败。关闭编辑器或终止 `UnrealEditor.exe` 进程
3. **头文件路径**: UE 5.7 的引擎头文件在 `Engine/Source/Runtime/*/Public/` 或 `Engine/Source/Runtime/*/Classes/` 下
4. **Unity 编译**: UBT 默认使用 Unity 编译（合并多个 .cpp），单文件编译错误会被 Unity 文件排除（日志中 `Excluded from X unity file: file.cpp`）。修改后 UBT 会重新编译该文件

## 蓝图函数库 API 参考（BloodPaint 示例）

| 函数 | 说明 |
|---|---|
| `GetHitUV` | 从骨骼网格体 FHitResult 计算命中 UV（需 PerPolyCollision） |
| `CalculateUVStretch` | 拉伸比 = sqrt(UV面积) / sqrt(3D面积) |
| `GetAdjustedHitUV` | 直接获取校正后的 UV 和拉伸比 |
| `DrawBloodDecalAtUV` | 在 RenderTarget 上绘制血液贴花 |
