# Halcon 大恒相机扩展包 (Halcon_DaHeng)

本项目是 MVTec HALCON 的大恒(Galaxy/DaHeng)工业相机扩展包，通过封装大恒 SDK (GxIAPI) 提供 HALCON 原生的相机操作算子。

## 功能特性

- 支持大恒全系列工业相机 (GigE/USB3.0)
- HALCON 原生算子接口，与 HALCON 开发环境无缝集成
- 支持回调采集模式，高性能图像获取
- 支持相机参数读写 (Int/Float/Enum/Bool/Command)

## 安装说明

### 环境要求

- Windows 10/11 x64
- MVTec HALCON 20.11 或更高版本
- Visual Studio 2019/2022 (用于编译)
- 大恒相机 SDK (GxIAPI)

### 安装步骤

1. **安装大恒相机 SDK**
   - 从官网下载 Galaxy SDK
   - 安装并确保相机能被 GalaxyView 识别

2. **编译扩展包**
   ```powershell
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

3. **安装到 HALCON**
   - 将 `bin/` 目录下的所有文件复制到 HALCON 扩展包目录：
     ```
     C:\Users\<用户名>\AppData\Roaming\MVTec\HALCON-24.11-Progress-Steady\extensions\Halcon_DaHeng\
     ```
   - 或者设置环境变量 `HALCONEXTENSIONS` 指向项目 `bin` 目录

4. **验证安装**
   - 打开 HDevelop
   - 输入 `DHGXInitLib()` 不报错即表示安装成功

## 算子说明

### 库管理

| 算子 | 说明 |
|------|------|
| `DHGXInitLib` | 初始化大恒 SDK |
| `DHGXCloseLib` | 关闭 SDK 释放资源 |
| `DHGXGetLastError` | 获取最后一次错误信息 |

### 相机连接

| 算子 | 说明 |
|------|------|
| `DHGXOPenCameraByID` | 通过用户 ID 打开相机，返回相机句柄 |

参数说明：
- `Name`: 相机用户定义名称 (在 GalaxyView 中设置)
- `TimeSharingFlicker`: 分时频闪数 (用于多光源场景)
- `CameraType`: 设备类型 (1=GigE, 2=USB3)
- `QueueHandle`: HALCON 消息队列句柄 (用于接收图像)
- `CameraHandle`: 返回的相机句柄

### 参数设置 (Set)

| 算子 | 参数类型 | 说明 |
|------|----------|------|
| `DHGXSetIntValue` | 整数 | 设置整型参数 (如 Width, Height, OffsetX 等) |
| `DHGXSetFloatValue` | 浮点数 | 设置浮点参数 (如 ExposureTime, Gain 等) |
| `DHGXSetEnumValue` | 枚举 | 设置枚举参数 (如 PixelFormat, TriggerMode 等) |
| `DHGXSetBoolValue` | 布尔 | 设置布尔参数 (如 ReverseX, BlackLevelClampEnable 等) |
| `DHGXSetCommandValue` | 命令 | 执行命令 (如 TriggerSoftware, TimestampLatch 等) |

### 参数获取 (Get)

| 算子 | 返回值类型 | 说明 |
|------|------------|------|
| `DHGXGetIntValue` | 整数 | 获取整型参数当前值 |
| `DHGXGetFloatValue` | 浮点数 | 获取浮点参数当前值 |
| `DHGXGetEnumValue` | 整数 | 获取枚举参数当前值 |
| `DHGXGetBoolValue` | 整数(0/1) | 获取布尔参数当前值 |

## 使用示例

### 基础示例：打开相机并采集

```hdevelop
* 初始化 SDK
DHGXInitLib ()

* 创建消息队列用于接收图像
create_message_queue_queue (QueueHandle)

* 打开相机 (相机名称为 "Camera1")
DHGXOPenCameraByID ('Camera1', 1, 1, QueueHandle, CameraHandle)

* 设置连续采集模式
DHGXSetEnumValue (CameraHandle, 'TriggerMode', 0)

* 开始采集
set_framegrabber_param (CameraHandle, 'start_async', 1)

* 获取图像
read_message_queue (QueueHandle, ['image','nFrameNum'], 5000, Message)
get_message_obj (Image, Message, 'image')
get_message_tuple (Message, 'nFrameNum', FrameNum)

* 显示
dev_display (Image)
```

### 参数读写示例

```hdevelop
* 设置曝光时间 (微秒)
DHGXSetFloatValue (CameraHandle, 'ExposureTime', 5000.0)

* 获取当前曝光时间
DHGXGetFloatValue (CameraHandle, 'ExposureTime', CurrentExposure)

* 设置增益
gain := 10.0
DHGXSetFloatValue (CameraHandle, 'Gain', gain)

* 获取当前增益
DHGXGetFloatValue (CameraHandle, 'Gain', CurrentGain)

* 设置像素格式 (Mono8=17301505, Mono12=17825795)
DHGXSetEnumValue (CameraHandle, 'PixelFormat', 17301505)

* 获取当前像素格式
DHGXGetEnumValue (CameraHandle, 'PixelFormat', CurrentFormat)

* 设置 ROI
DHGXSetIntValue (CameraHandle, 'OffsetX', 100)
DHGXSetIntValue (CameraHandle, 'OffsetY', 100)
DHGXSetIntValue (CameraHandle, 'Width', 640)
DHGXSetIntValue (CameraHandle, 'Height', 480)

* 获取图像宽度
DHGXGetIntValue (CameraHandle, 'Width', ImageWidth)

* 获取图像高度
DHGXGetIntValue (CameraHandle, 'Height', ImageHeight)
```

### 触发模式示例

```hdevelop
* 设置为软触发模式
DHGXSetEnumValue (CameraHandle, 'TriggerMode', 1)        * On
DHGXSetEnumValue (CameraHandle, 'TriggerSource', 0)      * Software

* 发送软触发命令
DHGXSetCommandValue (CameraHandle, 'TriggerSoftware')

* 读取图像
read_message_queue (QueueHandle, ['image'], 5000, Message)
get_message_obj (Image, Message, 'image')
```

### 分时频闪示例

```hdevelop
* 打开相机，设置分时频闪数为 4
* 相机会将 4 张图像拼接成一张输出
DHGXOPenCameraByID ('Camera1', 4, 1, QueueHandle, CameraHandle)

* 设置 Strobe 输出
DHGXSetEnumValue (CameraHandle, 'StrobeSwitch', 1)  * On

* 获取的图像将包含 4 个子图像，宽度为单张的 4 倍
read_message_queue (QueueHandle, ['image'], 5000, Message)
get_message_obj (Image4Channel, Message, 'image')

* 分割图像
get_image_size (Image4Channel, Width4, Height)
Width := Width4 / 4
crop_part (Image4Channel, Image1, 0, 0, Height, Width)
crop_part (Image4Channel, Image2, 0, Width, Height, Width)
crop_part (Image4Channel, Image3, 0, Width*2, Height, Width)
crop_part (Image4Channel, Image4, 0, Width*3, Height, Width)
```

## 常用参数名称对照表

| 功能 | 参数名称 | 类型 | 说明 |
|------|----------|------|------|
| 曝光 | `ExposureTime` | Float | 曝光时间 (μs) |
| 增益 | `Gain` | Float | 模拟增益 (dB) |
| 宽度 | `Width` | Int | 图像宽度 |
| 高度 | `Height` | Int | 图像高度 |
| 水平偏移 | `OffsetX` | Int | ROI 水平起始 |
| 垂直偏移 | `OffsetY` | Int | ROI 垂直起始 |
| 像素格式 | `PixelFormat` | Enum | Mono8/Mono12 等 |
| 触发模式 | `TriggerMode` | Enum | Off/On |
| 触发源 | `TriggerSource` | Enum | Software/Line1 等 |
| 软触发 | `TriggerSoftware` | Command | 执行软触发 |
| 水平翻转 | `ReverseX` | Bool | 启用水平镜像 |
| 垂直翻转 | `ReverseY` | Bool | 启用垂直镜像 |

## 常见问题

### Q: 打开相机失败？
- 检查相机是否被其他程序占用 (如 GalaxyView)
- 确认相机名称正确，区分大小写
- 检查网络连接 (GigE 相机)

### Q: 获取图像超时？
- 检查相机是否正确触发
- 增加 `read_message_queue` 的超时时间
- 确认 `TriggerMode` 设置正确

### Q: 参数设置无效？
- 某些参数需要在采集停止时设置
- 检查参数范围是否在允许值内
- 使用 `DHGXGetLastError` 获取详细错误信息

## 许可证

MIT License

## 技术支持

- 大恒 SDK 文档：`doc/` 目录
- HALCON 扩展包编程手册：`extension_package_programmers_manual.pdf`
