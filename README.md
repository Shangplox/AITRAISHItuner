# AITRAISHItuner

A professional, open-source VST3 instrument plugin for Windows. Load a sample, tune it, saturate it, and route it — without leaving FL Studio.

> **Status:** MVP — v0.1.0 in development. Not yet released.

---

## What it does

- **Sample Engine** — Load WAV files, trigger with MIDI, tune with ±24 semitones (simple resampling), control start/end/loop/one-shot
- **Distortion** — Soft-clip tanh saturation with 4x anti-aliased oversampling, tone control, wet/dry mix
- **Metering** — Peak and RMS meters displayed in real time
- **Preset system** — Save and load JSON presets; factory presets included
- **FL Studio ready** — IS_SYNTH instrument, PDC-correct latency reporting, MIDI step sequencer + piano roll compatible

---

## Supported formats

| Format | Status |
|---|---|
| VST3 | Supported (MVP) |
| CLAP | Phase 2 |
| AU / AAX / VST2 | Not planned |

## Supported platforms

| Platform | Status |
|---|---|
| Windows 10 / 11 | Supported |
| macOS | Not planned |
| Linux | Not planned |

---

## Build requirements

- Visual Studio 2022 (C++ Desktop workload)
- CMake 3.22+
- Git

JUCE 8 is downloaded automatically. See [docs/BUILDING.md](docs/BUILDING.md) for step-by-step instructions.

## Quick build

```bash
git clone https://github.com/YOUR_ORG/AITRAISHItuner.git
cd AITRAISHItuner
cmake --preset windows-vs2022-release
cmake --build build/release --config Release
```

The built VST3 appears at:
```
build/release/AITRAISHItuner_artefacts/Release/VST3/AITRAISHItuner.vst3
```

---

## Install in FL Studio

1. Copy `AITRAISHItuner.vst3` to `C:\Program Files\Common Files\VST3\AITRAISHItuner\`
2. In FL Studio: `Options → Manage plugins → Verify plugins`
3. The plugin appears in the Instrument list

---

## Feature roadmap

| Phase | Features |
|---|---|
| 0 (done) | Architecture, CMake, environment setup |
| 1 (MVP) | VST3 loads, sample playback, soft-clip distortion, presets |
| 2 | CLAP format, phase-vocoder pitch shifting, reverse sample, additional distortion modes |
| 3 | FFT spectrum analyzer, BPM/pitch detection |
| 4 | WASAPI companion recorder app |
| 5 | Commercial polish, installer, full UI |

---

## Licensing

This project is open-source under the **GNU Affero General Public License v3.0 (AGPL-3.0)**.

This license is required because AITRAISHItuner uses JUCE 8 under its open-source tier.

**What this means for you:**

- Free to use, modify, and distribute for open-source projects
- If you ship a modified version to other users, you must also publish your source code under AGPL-3.0
- If you want to include this code in a **closed-source or commercial product**, you need:
  1. A commercial JUCE license from [juce.com](https://juce.com)
  2. Written permission for any project-specific code additions

See [LICENSE](LICENSE) for the full AGPL-3.0 text.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). All contributions require:
- Passing all existing tests (`ctest --preset windows-vs2022-release`)
- No real-time audio thread violations
- Matching the code style in the existing codebase

---

## Known limitations (MVP)

- Pitch shifting is simple resampling — changes tempo of loops as a side effect
- Only soft-clip distortion in MVP (Tube, Tape, Bitcrush, Wavefold are Phase 2)
- No FFT spectrum analyzer in MVP
- No installer — manual copy to VST3 directory required
- Windows only
