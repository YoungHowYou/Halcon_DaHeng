# Halcon_DaHeng — Linux 构建与运行指南

本文档记录在 Linux (x86_64) 平台上构建并运行本 HALCON 扩展包的完整流程，
以及与 Windows 版本的差异点。Windows 流程保持不变，参见仓库根目录原有说明。

---

## 1. 前置依赖

| 组件 | 版本 / 路径 | 说明 |
|------|-------------|------|
| HALCON | 24.11 Progress Steady | 需设置环境变量 `HALCONROOT`、`HALCONEXAMPLES`、`HALCONARCH=x64-linux`，并将 `$HALCONROOT/lib/x64-linux` 加入 `LD_LIBRARY_PATH` |
| 大恒 Galaxy Linux SDK | 2.4.x 及以上 (`Galaxy_Linux-x86_Gige-U3_*.run`) | 安装后系统会有 `/usr/lib/libgxiapi.so` 与 `liblog4cplus_gx.so`，并安装 `GxGVTL.cti` / `GxU3VTL.cti` |
| GCC | 9+ (实测 GCC 13.3) | 需 C++17 |
| CMake | ≥ 3.20 | 关闭 `HALCON_DOTNET` 时无需 4.1.1 |

> 安装 Galaxy SDK 前请先执行 `SetUSBStack.sh`、`SetSocketBufferSize.sh`，否则 USB3/GigE 相机会因缓冲区不足导致丢帧。

确认环境：

```bash
echo $HALCONROOT $HALCONARCH
ldconfig -p | grep gxiapi
which cmake && cmake --version
```

---

## 2. 仓库内 Linux 专属资源

为了让 Linux 与 Windows 共存，本仓库已新增以下文件夹/文件（不会影响 Windows 构建）：

```
3rd/daheng/
├── inc/             # Windows 版 SDK 头文件（保持不变）
├── inc_linux/       # 新增：Linux 版 SDK 头文件
└── lib/
    ├── x64/         # Windows .lib (GxIAPI.lib / DxImageProc.lib)
    ├── x86/         # Windows 32 位 .lib
    └── x86_64/      # 新增：Linux .so (libgxiapi.so)
```

> ⚠️ Windows 与 Linux 的头文件并非完全一致。例如 `GX_FRAME_CALLBACK_PARAM` 在
> Linux 头文件中没有 `pChunkDataHandle` 字段。若直接复用 Windows 头编译会
> 报 `enum GX_AUTO_CONNECTION_STATUS_ENTRY` 等错误，因此必须使用 `inc_linux/`。

如需升级 SDK：

```bash
cp /path/to/Galaxy_camera/inc/*.h         3rd/daheng/inc_linux/
cp /path/to/Galaxy_camera/lib/x86_64/libgxiapi.so 3rd/daheng/lib/x86_64/
```

---

## 3. 构建步骤

```bash
cd Halcon_Extension/Halcon_DaHeng
mkdir -p build-linux && cd build-linux
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

构建产物（输出到仓库根 `bin/`）：

| 文件 | 说明 |
|------|------|
| `libHalcon_DaHeng.so`     | 扩展包主库 |
| `libHalcon_DaHengc.so`    | C 语言接口 |
| `libHalcon_DaHengcpp.so`  | C++ 语言接口 |

可用 `ldd bin/libHalcon_DaHeng.so` 验证已正确链接 `libhalconcpp` 与 `libgxiapi`。

> 选项：`-DHALCON_XL=ON` 启用 HALCON XL；Linux 上 `HALCON_DOTNET` 默认关闭，
> 不要在 Linux 上开启它（依赖 Visual Studio 的 CSharp 生成器）。

---

## 4. 安装到 HALCON

HALCON 通过 `HALCON_AddExtensionPackage` 自动生成 `cmake_install` 目标：

```bash
cmake --install . --prefix $HALCONROOT/extension_packages/Halcon_DaHeng
```

或者将 `bin/`、`def/`、`help/`、`doc/` 拷到自定义扩展包目录后，把该目录加入
`HALCONEXTENSIONS` 环境变量（多个目录用 `:` 分隔）。

启动 HDevelop / 自有程序前确保：

```bash
export LD_LIBRARY_PATH=$HALCONROOT/lib/x64-linux:$PWD/bin:$LD_LIBRARY_PATH
export HALCONEXTENSIONS=/path/to/Halcon_DaHeng:$HALCONEXTENSIONS
```

---

## 5. 运行时排错

| 现象 | 原因 / 处理 |
|------|-------------|
| `error while loading shared libraries: libgxiapi.so` | 未安装 Galaxy SDK，或 `LD_LIBRARY_PATH` 未包含 `/usr/lib` 与 SDK 安装目录 |
| 找不到 `GxGVTL.cti` / `GxU3VTL.cti` | 设置 `GENICAM_GENTL64_PATH=/usr/lib`（SDK 安装脚本通常已写入 `/etc/profile.d/`）|
| 相机能搜到但取流失败 | 检查 `SetSocketBufferSize.sh`（GigE）或 USB3 USBFS 缓冲区，参考 `Galaxy_camera/config/galaxy-limits.conf` |
| HDevelop 中 `DHGX*` 算子缺失 | `HALCONEXTENSIONS` 没指向本扩展包目录，或扩展包目录里缺少 `def/` `help/` |
| `permission denied` 访问 USB 相机 | `cp Galaxy_camera/config/99-galaxy-u3v.rules /etc/udev/rules.d/ && udevadm control --reload && udevadm trigger`，并把当前用户加入 `plugdev` 组 |

---

## 6. 与 Windows 版本的差异速查

| 项目 | Windows | Linux |
|------|---------|-------|
| 大恒 SDK 头文件 | `3rd/daheng/inc/` | `3rd/daheng/inc_linux/` |
| 大恒 SDK 库 | `GxIAPI.lib` + `DxImageProc.lib` | 仅 `libgxiapi.so`（源码未使用 DxImageProc） |
| HALCON 库后缀 | `halconcpp.lib` | `libhalconcpp.so` |
| 导出宏 `EXPORTS_API` | `__declspec(dllexport)` | `__attribute__((visibility("default")))` |
| `strcpy_s` / `__int64` | 由 MSVC 提供 | 在 `Halcon_DaHeng.cpp` 顶部按平台补写兼容实现 |
| `HGetPPar` 第三参类型 | 隐式接受 `Hcpar**` | 严格要求 `const Hcpar**`，使用 `const_cast` 适配 `HTuple(Hcpar*, ...)` |
| `.NET` 支持 | 默认 ON | 默认 OFF（`-DHALCON_DOTNET=OFF`，CMake 最低 3.20） |
| 输出目录 | `bin/*.dll` | `bin/lib*.so` |

---

## 7. 后续待办

- [ ] 验证 GigE / USB3 相机的实际取流回调路径（`OnFrameCallbackFun` 中的 `EnqueueMessage` 流程）。
- [ ] 在 Linux 上跑 `examples/` 下的 HDevelop 例程，确认 `DHGXOPenCameraByID` 句柄交互正常。
- [ ] 视需要把 `libgxiapi.so` 改为 `find_library(GXIAPI gxiapi)`，让外部环境（如 `/usr/lib`）的 SDK 也可被自动发现。
- [ ] 如需要 Bayer 转换等功能，再补 Linux 版 `DxImageProc`（大恒 Linux SDK 一般通过 `libdximageproc.so` 单独提供，本仓库目前未集成）。
