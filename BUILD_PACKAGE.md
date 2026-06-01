# RYCOM Windows 源码编译与安装包生成说明

本文档用于在另一台 Windows 电脑上，从 GitHub 下载 RYCOM 源码后，让 Codex 或开发者按同一套流程编译，并生成可正常安装运行的安装包。

最终输出文件必须是：

```text
build-codex/RYCOM-2.6.5-Setup.exe
```

安装包内容必须来自 `windeployqt` 收集后的运行目录，不能直接把 `release` 编译目录打包。这样可以保证安装后的 `RYCOM.exe` 在脱离编译目录后仍能找到 Qt DLL、MinGW DLL 和 Qt 插件。

## 1. 工具要求

目标机器需要准备以下工具：

| 工具 | 推荐版本/路径 | 说明 |
| --- | --- | --- |
| Qt MinGW Kit | `D:\Qt\6.9.2\mingw_64` | 提供 `qmake.exe`、`windeployqt.exe`、Qt DLL |
| MinGW | `D:\Qt\Tools\mingw1310_64` | 提供 `mingw32-make.exe` 和运行时 DLL |
| NSIS | `C:\Program Files (x86)\NSIS` | 提供 `makensis.exe` |
| CMake/Ninja | Qt 自带即可 | 仅在 QtSerialPort 缺失时需要 |

如果本机安装路径不同，可以替换下面命令中的路径，但目录结构和输出目录不要改。

先在仓库根目录执行路径检查：

```powershell
$QtDir = 'D:\Qt\6.9.2\mingw_64'
$MingwDir = 'D:\Qt\Tools\mingw1310_64'
$NsisExe = 'C:\Program Files (x86)\NSIS\makensis.exe'

Test-Path "$QtDir\bin\qmake.exe"
Test-Path "$QtDir\bin\windeployqt.exe"
Test-Path "$MingwDir\bin\mingw32-make.exe"
Test-Path $NsisExe
```

四项都应输出 `True`。如果不是，先修正 `$QtDir`、`$MingwDir`、`$NsisExe`。

## 2. 检查 QtSerialPort

RYCOM 依赖 Qt Serial Port 模块。先检查模块是否存在：

```powershell
Test-Path "$QtDir\include\QtSerialPort\QSerialPort"
Test-Path "$QtDir\mkspecs\modules\qt_lib_serialport.pri"
```

如果任意一项不是 `True`，`qmake` 会报：

```text
Project ERROR: Unknown module(s) in QT: serialport
```

优先使用 Qt MaintenanceTool 安装同版本、同编译器套件的 Qt Serial Port。若无法通过 MaintenanceTool 安装，可用源码安装：

```powershell
New-Item -ItemType Directory -Force -Path _deps | Out-Null
git clone --branch v6.9.2 --depth 1 https://code.qt.io/qt/qtserialport.git _deps\qtserialport
New-Item -ItemType Directory -Force -Path _deps\qtserialport-build | Out-Null

$env:PATH="$QtDir\bin;$MingwDir\bin;D:\Qt\Tools\CMake_64\bin;D:\Qt\Tools\Ninja;$env:PATH"
Push-Location _deps\qtserialport-build
& "$QtDir\bin\qt-cmake.bat" ..\qtserialport -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$QtDir.Replace('\','/')
& 'D:\Qt\Tools\Ninja\ninja.exe'
& 'D:\Qt\Tools\Ninja\ninja.exe' install
Pop-Location
```

安装后重新执行本节开头的两个 `Test-Path`。

## 3. 编译工程

所有临时文件和产物都放在 `build-codex/`，该目录已加入 `.gitignore`。

```powershell
$QtDir = 'D:\Qt\6.9.2\mingw_64'
$MingwDir = 'D:\Qt\Tools\mingw1310_64'
$env:PATH="$QtDir\bin;$MingwDir\bin;$env:PATH"

New-Item -ItemType Directory -Force -Path build-codex | Out-Null

Push-Location build-codex
& "$QtDir\bin\qmake.exe" ..\sources\RYCOM.pro
& "$MingwDir\bin\mingw32-make.exe" -j4
Pop-Location
```

编译成功后应得到：

```text
build-codex/release/RYCOM.exe
```

如果链接时报：

```text
cannot open output file release\RYCOM.exe: Permission denied
```

关闭正在运行的 `build-codex\release\RYCOM.exe`，再重新执行 `mingw32-make`。不要关闭用户已经安装在 `C:\Program Files (x86)\RYMCU\RYCOM\RYCOM.exe` 的正式版本，避免影响正在使用的软件。

## 4. 生成干净运行目录

不要直接打包 `build-codex/release`，因为里面有目标文件和中间文件。先创建干净目录，只放实际运行需要的文件：

```powershell
$QtDir = 'D:\Qt\6.9.2\mingw_64'
$MingwDir = 'D:\Qt\Tools\mingw1310_64'
$env:PATH="$QtDir\bin;$MingwDir\bin;$env:PATH"

$release = Resolve-Path -LiteralPath 'build-codex\release'
$packageRoot = Join-Path (Resolve-Path -LiteralPath 'build-codex') 'package'
$appDir = Join-Path $packageRoot 'RYCOM-2.6.5'

if (Test-Path -LiteralPath $appDir) {
    Remove-Item -LiteralPath $appDir -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $appDir | Out-Null
Copy-Item -LiteralPath (Join-Path $release 'RYCOM.exe') -Destination $appDir -Force
```

再用 `windeployqt` 收集 Qt 依赖：

```powershell
& "$QtDir\bin\windeployqt.exe" --release --no-compiler-runtime --no-translations (Join-Path $appDir 'RYCOM.exe')
```

最后补齐 MinGW 运行时 DLL：

```powershell
foreach ($dll in @('libgcc_s_seh-1.dll','libstdc++-6.dll','libwinpthread-1.dll')) {
    $src = Join-Path $release $dll
    if (Test-Path -LiteralPath $src) {
        Copy-Item -LiteralPath $src -Destination $appDir -Force
    } else {
        $src = Join-Path "$MingwDir\bin" $dll
        Copy-Item -LiteralPath $src -Destination $appDir -Force
    }
}
```

运行目录应包含：

```text
build-codex/package/RYCOM-2.6.5/RYCOM.exe
build-codex/package/RYCOM-2.6.5/Qt6Core.dll
build-codex/package/RYCOM-2.6.5/Qt6Gui.dll
build-codex/package/RYCOM-2.6.5/Qt6Widgets.dll
build-codex/package/RYCOM-2.6.5/Qt6SerialPort.dll
build-codex/package/RYCOM-2.6.5/platforms/qwindows.dll
```

`windeployqt` 如果提示缺少 `dxcompiler.dll`、`dxil.dll`，当前 RYCOM 不使用 Qt Quick/Direct3D Shader 功能，通常不影响本程序运行。

## 5. 使用 NSIS 生成安装包

仓库已经提供 NSIS 脚本：

```text
installer/RYCOM-2.6.5.nsi
```

该脚本固定读取：

```text
build-codex/package/RYCOM-2.6.5
```

并输出：

```text
build-codex/RYCOM-2.6.5-Setup.exe
```

在仓库根目录执行：

```powershell
$NsisExe = 'C:\Program Files (x86)\NSIS\makensis.exe'
& $NsisExe 'installer\RYCOM-2.6.5.nsi'
```

## 6. 验证输出

确认安装包存在：

```powershell
Get-Item build-codex\RYCOM-2.6.5-Setup.exe
```

可选：先运行部署后的目录，检查是否缺 DLL：

```powershell
$exe = (Resolve-Path -LiteralPath 'build-codex\package\RYCOM-2.6.5\RYCOM.exe').Path
$p = Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe) -PassThru
Start-Sleep -Seconds 3
Get-Process -Id $p.Id | Select-Object Id, ProcessName, MainWindowTitle, Path
Stop-Process -Id $p.Id -Force
```

再运行安装包安装到默认目录，安装后启动：

```text
C:\Program Files (x86)\RYMCU\RYCOM\RYCOM.exe
```

如果安装后的程序能启动且不提示缺少 DLL，说明打包正确。

## 7. 给 Codex 的执行要求

Codex 在新机器上处理本工程时，应遵守以下规则：

- 固定使用 `build-codex/` 作为构建和打包输出目录，不把生成物写入源码目录。
- 编译前先确认 Qt、MinGW、NSIS 三类工具路径，路径不同则只替换变量，不修改工程结构。
- `qmake` 失败并提示缺少 `serialport` 时，先安装或编译同版本 QtSerialPort，再继续构建。
- 安装包必须基于 `build-codex/package/RYCOM-2.6.5`，不能直接打包 `build-codex/release`。
- 必须先运行 `windeployqt`，再执行 NSIS。
- 最终必须确认 `build-codex/RYCOM-2.6.5-Setup.exe` 存在。
- `build-codex/` 是生成目录，不提交到 Git。

## 8. 一次性命令汇总

工具路径正确、QtSerialPort 已存在时，可直接从仓库根目录执行：

```powershell
$QtDir = 'D:\Qt\6.9.2\mingw_64'
$MingwDir = 'D:\Qt\Tools\mingw1310_64'
$NsisExe = 'C:\Program Files (x86)\NSIS\makensis.exe'
$env:PATH="$QtDir\bin;$MingwDir\bin;$env:PATH"

New-Item -ItemType Directory -Force -Path build-codex | Out-Null

Push-Location build-codex
& "$QtDir\bin\qmake.exe" ..\sources\RYCOM.pro
& "$MingwDir\bin\mingw32-make.exe" -j4
Pop-Location

$release = Resolve-Path -LiteralPath 'build-codex\release'
$packageRoot = Join-Path (Resolve-Path -LiteralPath 'build-codex') 'package'
$appDir = Join-Path $packageRoot 'RYCOM-2.6.5'

if (Test-Path -LiteralPath $appDir) {
    Remove-Item -LiteralPath $appDir -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $appDir | Out-Null
Copy-Item -LiteralPath (Join-Path $release 'RYCOM.exe') -Destination $appDir -Force
& "$QtDir\bin\windeployqt.exe" --release --no-compiler-runtime --no-translations (Join-Path $appDir 'RYCOM.exe')

foreach ($dll in @('libgcc_s_seh-1.dll','libstdc++-6.dll','libwinpthread-1.dll')) {
    $src = Join-Path $release $dll
    if (Test-Path -LiteralPath $src) {
        Copy-Item -LiteralPath $src -Destination $appDir -Force
    } else {
        $src = Join-Path "$MingwDir\bin" $dll
        Copy-Item -LiteralPath $src -Destination $appDir -Force
    }
}

& $NsisExe 'installer\RYCOM-2.6.5.nsi'
Get-Item build-codex\RYCOM-2.6.5-Setup.exe
```
