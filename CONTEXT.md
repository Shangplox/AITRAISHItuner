# AITRAISHItuner — Project Context
**Last updated:** 2026-05-16
**Status:** Phase 0 — Blueprint audited and corrected. Environment setup required before Phase 1.

---

## What This Project Is

A professional Windows-only VST3 audio plugin for a producer community. Combines sample manipulation, distortion/saturation, frequency analysis, pitch detection, and BPM detection in one tool for FL Studio producers.

**Plugin name:** AITRAISHItuner
**Format:** VST3 only
**OS:** Windows 10/11 only
**DAW:** FL Studio 21/24 (priority), other Windows VST3 hosts (secondary)
**Framework:** JUCE 8 + C++20
**Distribution:** Community beta → commercial

---

## Current Phase

**Phase 0 — Research and Setup**
- [x] Blueprint defined
- [x] CLAUDE.md and CONTEXT.md created
- [x] Architecture audited — issues found and fixed (see Audit section below)
- [ ] Install Visual Studio 2022 Community (C++ Desktop workload)
- [ ] Install CMake 3.24+ (add to PATH)
- [ ] Install JUCE 8 (clone to C:\JUCE from github.com/juce-framework/JUCE)
- [ ] Verify JUCE example (AudioPluginHost) builds on this machine
- [ ] Initialize git repository in c:\Project\AITRAISHItuner
- [ ] Create source folder structure
- [ ] Install Inno Setup 6 (can wait until Phase 8)

## Audit Fixes Applied (2026-05-16)

- **Fixed:** Incorrect `juce::dsp::DelayLine` listed as cross-thread primitive in CLAUDE.md — replaced with correct table
- **Fixed:** Added missing `sample-oneshot` parameter to CLAUDE.md
- **Fixed:** Added missing `analyzer-enabled` and `analyzer-fft-size` parameters to CLAUDE.md
- **Fixed:** Defined namespace as `AIT::` (was `AITuner::` — inconsistent with project name)
- **Fixed:** Added MIDI input handling note to CLAUDE.md FL Studio section
- **Fixed:** Removed `WatchFolderMonitor` from PluginProcessor architecture (it is Phase 10)
- **Fixed:** Clarified `AnalyzerModule` (audio thread, ring buffer writer) vs `BackgroundAnalyzer` (background, FFT computation) roles
- **Fixed:** Corrected licensing MVP to invite code only (RSA keys are Phase 12)
- **Documented:** `sample-reverse` is NOT automatable in MVP (pre-reversal buffer pattern)
- **Documented:** `dist-mode` is NOT automatable in MVP (modes 1–5 unimplemented)
- **Documented:** `dist-oversample` is NOT automatable (changes PDC)

---

## Tech Stack

| Concern | Choice |
|---|---|
| Framework | JUCE 8 |
| Language | C++20 |
| Plugin format | VST3 |
| Build system | CMake |
| Compiler | MSVC (VS2022) |
| Parameters | AudioProcessorValueTreeState (APVTS) |
| Oversampling | juce::dsp::Oversampling |
| FFT | juce::dsp::FFT |
| Audio capture (companion) | WASAPI |
| Installer | Inno Setup 6 |
| Licensing (MVP) | Invite code only (static list) |
| Licensing (Phase 12) | RSA-signed offline license key |

---

## Windows Paths

| Purpose | Path |
|---|---|
| VST3 plugin | `C:\Program Files\Common Files\VST3\AITRAISHItuner\` |
| Presets | `%AppData%\AITRAISHItuner\Presets\` |
| Settings | `%AppData%\AITRAISHItuner\Settings\` |
| Watch folder | `%AppData%\AITRAISHItuner\WatchFolder\` |
| Companion app | `C:\Program Files\AITRAISHItuner Recorder\` |
| Logs | `%AppData%\AITRAISHItuner\Logs\` |

---

## Feature Scope

### MVP (Phases 1–8)
- VST3 loads in FL Studio
- WAV drag-drop import
- Waveform display
- Sample playback (gain, pitch, start/end trim, reverse, loop, one-shot)
- Tanh soft clip distortion (drive, mix, output, 4x oversampling)
- Peak/RMS metering
- Preset save/recall via APVTS
- Windows installer or manual install guide
- FL Studio compatibility testing

### Phase 9 (Community Beta)
- Extended testing across community PCs
- Basic licensing/invite system
- Bug fixes from beta feedback

### Phase 10 (Companion Recorder App)
- Windows desktop app (WASAPI)
- Mic capture + system audio loopback
- Record to WAV → watch folder
- Plugin auto-imports from watch folder

### Phase 11 (Advanced Analysis)
- Sound type detection (808, kick, snare, hi-hat, vocal, lead)
- BPM detection
- Pitch/root note detection
- Transient detection
- Frequency suggestions per sound type

### Phase 12 (Commercial Polish)
- Additional distortion modes (tube, diode, tape, bitcrush, wavefold)
- 8x oversampling option
- LUFS metering
- Advanced preset browser
- Commercial licensing system
- Full UI polish

### NOT in this project (ever)
- macOS support
- AU format
- AAX format
- iOS/Android
- Linux
- PACE iLok (unless commercially required much later)
- Direct system audio capture inside the VST3 plugin

---

## Architecture Overview

```
PluginProcessor (AudioProcessor)                  [Audio thread]
├── APVTS (parameter, state, preset management)   [Thread-safe]
├── SampleEngine (playback, pitch, reverse, loop) [Audio thread DSP]
├── DistortionEngine (waveshapers, oversampling)  [Audio thread DSP]
├── MeteringModule (peak/RMS, atomic output)      [Audio thread writes, UI reads atomic]
├── AnalyzerModule (ring buffer writer only)       [Audio thread — WRITES to ring buffer only, no FFT computation]
└── FileLoadingService (background thread)        [Background — lock-free handoff via AtomicAudioBuffer]

BackgroundAnalyzer (juce::Thread)                 [Background thread — Phase 6+]
└── Reads AnalyzerModule ring buffer → runs FFT → AsyncUpdater → UI

NOTE: WatchFolderMonitor is a Phase 10 addition, not part of Phase 1-9 PluginProcessor.

PluginEditor (AudioProcessorEditor)               [Message thread]
├── WaveformPanel (sample display, drag-drop target)
├── SampleControlPanel (gain, pitch, start/end, reverse, loop)
├── DistortionPanel (mode, drive, tone, mix, output)
├── FrequencyHelperPanel (analyzer + production hints — Phase 11)
├── MeteringPanel (peak/RMS meters)
├── SpectrumPanel (FFT display — Phase 6)
├── PresetBrowser
└── SettingsPanel

CompanionApp (separate Windows .exe — Phase 10)
├── WASAPICapture (mic + loopback)
├── WAVWriter
├── WatchFolderWriter
└── SimpleUI (record/stop/send)
```

### AnalyzerModule vs BackgroundAnalyzer — Thread Roles

| Module | Location | Thread | Responsibility |
|---|---|---|---|
| `AnalyzerModule` | `Source/DSP/` | Audio thread | Receives audio samples from processBlock. Writes to lock-free ring buffer ONLY. No FFT. No computation. |
| `BackgroundAnalyzer` | `Source/Engine/` | Background thread | Reads from ring buffer. Runs juce::dsp::FFT. Builds spectrum data. Posts to UI via AsyncUpdater. |

These are two different classes with a clear data handoff boundary. Do not merge them.

---

## Thread Model

| Thread | Responsibility | Forbidden |
|---|---|---|
| Audio thread | processBlock, DSP, metering output | File I/O, memory alloc, mutexes, UI, sockets |
| Message thread | UI rendering, APVTS listeners, user events | Long blocking ops |
| Background thread(s) | File loading, FFT analysis, BPM/pitch detection, watch folder poll | Calling processBlock or UI directly |

**Cross-thread communication:** Lock-free queues (juce::AbstractFifo), std::atomic, JUCE::AsyncUpdater, APVTS parameter changes (thread-safe).

---

## Parameter System

All parameters use APVTS. Parameter IDs must never change after release without a migration plan. Preset state is serialized as XML via APVTS.

---

## Phase Roadmap Summary

| Phase | Focus | Key Deliverable |
|---|---|---|
| 0 | Setup | JUCE+CMake+VS2022 environment |
| 1 | VST3 skeleton | Plugin loads/scans in FL Studio |
| 2 | APVTS | Parameters, state save/load |
| 3 | WAV loading | Drag-drop import, background load |
| 4 | Waveform UI | Visual waveform display |
| 5 | Soft clip distortion | Tanh waveshaper + oversampling |
| 6 | Metering + analyzer | Peak/RMS + FFT spectrum |
| 7 | FL Studio test | Full test suite pass |
| 8 | Installer | Windows Inno Setup installer |
| 9 | Community beta | Beta release + invite system |
| 10 | Companion app | WASAPI recorder + watch folder |
| 11 | Advanced analysis | BPM, pitch, sound type detection |
| 12 | Commercial polish | Full distortion suite, UI polish |

---

## Key Risks

1. **Real-time thread safety** — any blocking op in processBlock will crash FL Studio
2. **Aliasing from distortion** — must use anti-aliasing + proper oversampling
3. **FL Studio PDC** — oversampling introduces latency; must report it correctly
4. **Sample loading crashes** — background loading with lock-free handoff is mandatory
5. **Waveform UI performance** — must thumbnail/downsample on background thread, not audio thread
6. **Windows installer edge cases** — UAC, antivirus, different VST3 paths on user PCs
7. **Community PC diversity** — test on low-spec and high-spec Windows machines
8. **APVTS ID breaking changes** — never rename parameter IDs after community beta ships

---

## Development Rules (Summary)

See CLAUDE.md for full rules. Key points:
- Never block the audio thread
- Windows + VST3 + FL Studio only
- APVTS for all parameters
- Modules in phase order — do not skip ahead
- Explain files before creating them
- List all touched files after every session
- DSP and UI in separate classes/files
- Never rename parameter IDs without migration plan
