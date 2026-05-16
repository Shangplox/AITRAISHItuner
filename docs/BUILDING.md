# Building AITRAISHItuner

## Requirements

| Tool | Version | Source |
|---|---|---|
| Visual Studio 2022 | Community or higher | visualstudio.microsoft.com |
| C++ Desktop workload | Required | VS2022 installer → "Desktop development with C++" |
| CMake | 3.22 or later | cmake.org — add to PATH during install |
| Git | Any recent | git-scm.com |

JUCE 8 is downloaded automatically via CMake FetchContent. You do not need to install it manually.

---

## Clone the repo

```
git clone https://github.com/YOUR_ORG/AITRAISHItuner.git
cd AITRAISHItuner
```

---

## Configure (first time — downloads JUCE, takes 1–3 minutes)

```
cmake --preset windows-vs2022-release
```

For a debug build:

```
cmake --preset windows-vs2022-debug
```

---

## Build

```
cmake --build build/release --config Release
```

The VST3 bundle is created at:

```
build/release/AITRAISHItuner_artefacts/Release/VST3/AITRAISHItuner.vst3
```

---

## Install into FL Studio

### Option A — Manual copy

1. Copy `AITRAISHItuner.vst3` to:
   ```
   C:\Program Files\Common Files\VST3\AITRAISHItuner\
   ```
2. Launch FL Studio.
3. Go to `Options → Manage plugins → Verify plugins`.
4. The plugin should appear under **Instruments**.

### Option B — CMake install target (requires admin)

Run PowerShell as Administrator:

```powershell
cmake --install build/release --prefix "C:/Program Files/Common Files/VST3"
```

---

## Run tests

```
ctest --preset windows-vs2022-release --output-on-failure
```

Or with verbose output:

```
cd build/release
ctest -C Release -V
```

---

## Troubleshooting

**JUCE download fails:**
Check your internet connection. JUCE is fetched from GitHub. If you are behind a proxy, set `HTTPS_PROXY` environment variable.

**Plugin does not appear in FL Studio scan:**
Ensure the `.vst3` directory is in `C:\Program Files\Common Files\VST3\`. FL Studio scans subdirectories. Delete the FL Studio plugin cache (`%AppData%\Image-Line\FL Studio\Plugins\...`) and rescan.

**Build fails on `std::span` or C++20 features:**
Ensure Visual Studio 2022 is up to date. Older VS2019 or VS2017 will not work — the project requires MSVC 2022.

**Tests fail to compile with missing JUCE headers:**
The test target uses a subset of JUCE modules. If you see missing header errors in tests, verify that `juce::juce_audio_basics` and `juce::juce_dsp` are linked in `tests/CMakeLists.txt`.
