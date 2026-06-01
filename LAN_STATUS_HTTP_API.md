# RYCOM 局域网电脑状态 HTTP JSON 接口

本文档给第三方设备开发者使用。RYCOM 启动状态服务后，同一个 WiFi 或同一个局域网内的设备可以通过 HTTP 读取运行 RYCOM 电脑的状态信息。

## 启动方式

1. 打开 RYCOM。
2. 在窗口底部状态栏找到 `--助手源代码V2.6.4--` 右侧的 `启动状态` 按钮。
3. 点击 `启动状态`。
4. 按钮右侧文本框会显示访问地址，例如：

```text
http://192.168.1.20:18765/status
```

5. 第三方设备连接到同一个 WiFi 后，访问这个地址即可获取 JSON 数据。

启动后按钮文字会变成 `停止状态`。再次点击可停止 HTTP 服务。

## 接口信息

默认端口：

```text
18765
```

支持的 HTTP 接口：

```text
GET /status
GET /
```

返回格式：

```text
application/json; charset=utf-8
```

接口为只读接口，不需要用户名、密码或 token。请只在可信局域网中使用。

## 返回 JSON 示例

```json
{
  "app": "RYCOM",
  "timestamp": "2026-06-01T10:20:30+08:00",
  "host": {
    "name": "DESKTOP-PC",
    "os": "Windows 11",
    "kernelType": "winnt",
    "kernelVersion": "10.0.26100",
    "localIp": "192.168.1.20"
  },
  "cpu": {
    "usagePercent": 23.5,
    "temperatureCelsius": null,
    "logicalProcessors": 16
  },
  "memory": {
    "totalBytes": 17179869184,
    "usedBytes": 8589934592,
    "availableBytes": 8589934592,
    "usagePercent": 50.0
  },
  "gpu": {
    "name": null,
    "usagePercent": null,
    "temperatureCelsius": null,
    "note": "GPU status is not available without a vendor sensor API"
  }
}
```

## 字段说明

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `app` | string | 固定为 `RYCOM` |
| `timestamp` | string | 数据更新时间，ISO 8601 格式 |
| `host.name` | string | 电脑主机名 |
| `host.os` | string | 操作系统名称 |
| `host.kernelType` | string | 系统内核类型 |
| `host.kernelVersion` | string | 系统内核版本 |
| `host.localIp` | string | RYCOM 自动选择的局域网 IPv4 地址 |
| `cpu.usagePercent` | number/null | CPU 使用率，单位 `%` |
| `cpu.temperatureCelsius` | number/null | CPU 温度，单位摄氏度；无法稳定获取时为 `null` |
| `cpu.logicalProcessors` | number | 逻辑处理器数量 |
| `memory.totalBytes` | number/null | 物理内存总量，单位字节 |
| `memory.usedBytes` | number/null | 已用物理内存，单位字节 |
| `memory.availableBytes` | number/null | 可用物理内存，单位字节 |
| `memory.usagePercent` | number/null | 内存使用率，单位 `%` |
| `gpu.name` | string/null | GPU 名称；首版不依赖厂商 SDK，可能为 `null` |
| `gpu.usagePercent` | number/null | GPU 使用率；无法稳定获取时为 `null` |
| `gpu.temperatureCelsius` | number/null | GPU 温度；无法稳定获取时为 `null` |
| `gpu.note` | string | GPU 字段不可用时的说明 |

`null` 表示当前系统或当前版本无法稳定获取该字段，不表示接口错误。

## 请求示例

电脑本机测试：

```powershell
Invoke-RestMethod http://127.0.0.1:18765/status
```

局域网设备测试：

```bash
curl http://192.168.1.20:18765/status
```

ESP32 Arduino 示例：

```cpp
#include <HTTPClient.h>
#include <WiFi.h>

const char *ssid = "YOUR_WIFI";
const char *password = "YOUR_PASSWORD";
const char *statusUrl = "http://192.168.1.20:18765/status";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  HTTPClient http;
  http.begin(statusUrl);
  int code = http.GET();
  if (code == 200) {
    String json = http.getString();
    Serial.println(json);
  }
  http.end();
  delay(1000);
}
```

## 常见问题

### 第三方设备无法访问

检查以下项目：

- RYCOM 状态栏是否已经显示 `http://局域网IP:18765/status`。
- 第三方设备和电脑是否连接到同一个 WiFi 或同一个局域网。
- Windows 防火墙是否允许 RYCOM 访问专用网络。
- 路由器是否开启了 AP 隔离或访客网络隔离。
- 电脑 IP 是否发生变化，变化后以 RYCOM 状态栏显示的新地址为准。

### CPU 使用率第一次返回 null

CPU 使用率需要前后两次采样计算差值。服务刚启动后的第一次请求可能返回 `null`，等待约 1 秒后再次请求即可。

### CPU/GPU 温度为什么是 null

不同操作系统、主板和显卡厂商的传感器接口差异很大。当前版本优先保证跨平台稳定和不引入额外厂商 SDK，因此温度和 GPU 信息在无法稳定获取时返回 `null`。

### 端口被占用

如果启动失败，RYCOM 会弹出提示。请检查电脑上是否有其他程序占用了 `18765` 端口。
