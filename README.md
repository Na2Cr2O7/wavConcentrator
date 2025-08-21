确保你的wav文件有采样率信息。
# wavConcentrator

![License](https://img.shields.io/badge/license-GPLv3-blue.svg)

`wavConcentrator` 是一个命令行工具，用于将多个 WAV 音频文件按顺序拼接（串联）成一个立体声 WAV 文件。支持不同采样率的自动重采样、单声道/立体声混合输入，并提供基本的音频处理功能。

> 🎯 适用于语音播报拼接、有声书合成、音频批量处理等场景。

---

## 📋 功能特性

- ✅ 支持从文本列表加载多个 WAV 文件路径
- ✅ 自动统一采样率（线性插值重采样）
- ✅ 单声道文件自动复制为立体声
- ✅ 多通道音频取前两个通道
- ✅ 跨平台支持（Windows / Linux / macOS）
- ✅ 中文路径与控制台输出兼容（Windows）
- ✅ 内存使用实时显示
- ✅ 命令行友好，易于集成到脚本中

---

## 🛠 编译方法

### 依赖项

- C++17 编译器（g++ 7+ / clang 6+ / MSVC）
- [AudioFile.h](https://github.com/adamstark/AudioFile)（已包含或自行下载）

### 编译步骤
visual Studio 2022
---

## 🚀 使用方法

### 1. 准备文件列表

创建一个文本文件（如 `list.txt`），每行一个 WAV 文件路径：

```text
audio1.wav
audio2.wav
subfolder/audio3.wav
```

> 支持相对路径或绝对路径，自动去除首尾空格。

### 2. 运行程序

```bash
# 基本用法
./wavConcentrator -f list.txt -o output.wav

# 指定输出文件名
./wavConcentrator -f list.txt -o my_audiobook.wav
```

### 参数说明

| 参数 | 说明 |
|------|------|
| `-f <file.txt>` | 指定输入文件列表（必需） |
| `-o <output.wav>` | 指定输出 WAV 文件名（可选，默认 `output.wav`） |

---

## 🧪 示例

```bash
./wavConcentrator -f sentences.txt -o announcement.wav
```

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！  

---

## 📎 依赖库

- [AudioFile.h](https://github.com/adamstark/AudioFile) by Adam Stark  
  A simple, cross-platform, single-file, C++ library for reading and writing audio files.

