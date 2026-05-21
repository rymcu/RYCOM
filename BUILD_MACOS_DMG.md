# RYCOM macOS DMG 打包说明

本文档用于在 macOS 上从 GitHub 源码编译 RYCOM，并生成可分发的 `.dmg` 文件。

最终输出文件：

```text
build-codex/RYCOM-2.6.4.dmg
```

注意：`.dmg` 需要 macOS 的 `hdiutil`，Qt 依赖收集需要 `macdeployqt`。Windows 环境不能生成一个真实可运行的 macOS `.dmg`。

## 1. 工具要求

目标 Mac 需要安装：

- Qt for macOS，推荐 Qt `6.9.2`
- Xcode Command Line Tools
- Qt Serial Port 模块

常见 Qt 路径：

```text
$HOME/Qt/6.9.2/macos/bin
$HOME/Qt/6.9.2/clang_64/bin
/Applications/Qt/6.9.2/macos/bin
/Applications/Qt/6.9.2/clang_64/bin
```

先检查工具：

```bash
xcode-select -p
hdiutil help >/dev/null
$HOME/Qt/6.9.2/macos/bin/qmake -v
$HOME/Qt/6.9.2/macos/bin/macdeployqt -h
```

如果 Qt 安装在其他目录，后面通过 `QT_BIN` 指定。

## 2. 生成 DMG

从仓库根目录执行：

```bash
chmod +x scripts/package_macos_dmg.sh
QT_BIN="$HOME/Qt/6.9.2/macos/bin" ./scripts/package_macos_dmg.sh
```

如果 Qt 的 `qmake` 已经在 `PATH` 中，也可以直接执行：

```bash
./scripts/package_macos_dmg.sh
```

脚本会执行以下步骤：

- 清理并创建 `build-codex/macos/`
- 使用 `qmake` 编译 `sources/RYCOM.pro`
- 生成 `build-codex/macos/RYCOM.app`
- 写入 `CFBundleShortVersionString=2.6.4`
- 复制 `sources/rymculogo.icns`
- 执行 `macdeployqt RYCOM.app -dmg`
- 将输出统一改名为 `build-codex/RYCOM-2.6.4.dmg`

## 3. 可选签名

如果本机有开发者签名证书，可以指定：

```bash
CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAMID)" \
QT_BIN="$HOME/Qt/6.9.2/macos/bin" \
./scripts/package_macos_dmg.sh
```

没有签名证书时也可以生成 `.dmg`，但在其他 Mac 上首次打开可能会被 Gatekeeper 提示阻止，需要用户手动允许。

## 4. 验证

确认输出存在：

```bash
ls -lh build-codex/RYCOM-2.6.4.dmg
```

挂载检查：

```bash
hdiutil attach build-codex/RYCOM-2.6.4.dmg
open /Volumes/RYCOM/RYCOM.app
hdiutil detach /Volumes/RYCOM
```

如果挂载后的卷名不是 `RYCOM`，用 `hdiutil info` 查看实际挂载点后再 `detach`。

## 5. 给 Codex 的执行要求

Codex 在 macOS 机器上处理本工程时，应遵守：

- 使用 `scripts/package_macos_dmg.sh` 生成 `.dmg`。
- 固定输出到 `build-codex/RYCOM-2.6.4.dmg`。
- 不要使用仓库里的旧 `sources/RYCOM.app` 作为发布产物。
- 必须通过 `macdeployqt` 收齐 Qt Framework 和插件。
- `build-codex/` 是生成目录，不提交到 Git。
