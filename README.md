# RYCOM 串口调试助手

RYCOM 是一款开源的跨平台串口调试助手，使用 QT 设计，C++ 编写，支持 Windows、Linux 和 MacOS 操作系统。它支持 STM32 串口下载程序，并提供了丰富的功能和特性，适用于嵌入式开发和调试。

---

## ✨ 特性

### 基本功能
- 串口数据发送与接收
- 支持十六进制和文本数据的发送与显示
- 自动发送换行
- 数据接收时间显示（可选）
- 支持周期循环发送
- 发送与接收流量统计
- 支持中文发送与接收
- 文件发送功能
- 可保存接收数据
- 支持多行发送与多行循环发送

### 高级功能
- 支持 STM32 串口下载程序（支持 `.bin` 和 `.hex` 格式）
- 支持 ESP32 系列芯片串口下载程序
- 支持 Ymodem 文件传输协议（待开发）
- 支持MQTT通信（待开发）
- 支持网络调试（待开发）

---
## 📥 安装文件下载

RYCOM 提供了 Windows 和 MacOS 的安装包，您可以在 [Release 页面](https://gitee.com/rymcu/RYCOM/releases/tag/2.6.2) 下载对应操作系统的安装文件：

- **Windows**: 下载 `RYCOM 2.6.2-Setup.exe` 安装包。
- **MacOS**: 下载 `RYCOM 2.6.2.dmg` 安装包。

请根据您的操作系统选择合适的安装包，并按照提示完成安装。
## ⚡ 动机

RYCOM 的灵感来源于 [ronger](https://github.com/ronger) 的跨平台串口助手 [nebula-helper](https://github.com/ronger/nebula-helper)。我们希望通过 C++ 实现一个功能更强大、更易用的跨平台串口调试工具。

---

## � 报告缺陷

如果您在使用 RYCOM 时遇到以下问题，请通过 Issues 或社区[RYMCU](https://rymcu.com)反馈：
1. **功能性缺陷**：例如数据发送失败、数据显示乱码等。
2. **安全性漏洞**：例如串口打开失败、软件自动退出等。
3. **功能建议**：欢迎提出新功能或改进建议，我们将尽力实现。

---

## 🙏 鸣谢

- 感谢 [乐鑫科技 (Espressif)](https://www.espressif.com/) 提供的 ESP32 芯片支持及相关技术文档。
- 感谢 [QT 官方](https://www.qt.io/) 提供的跨平台开发框架，使 RYCOM 能够在多个操作系统上运行。

---

## 📄 许可证

RYCOM 是一个开源项目，遵循 [MIT 许可证](LICENSE)。欢迎贡献代码和提出建议！
