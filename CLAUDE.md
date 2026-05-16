# CLAUDE.md — AITRAISHItuner Development Rules
**Project:** AITRAISHItuner VST3 Plugin
**Platform:** Windows 10/11 only
**Format:** VST3 only
**Framework:** JUCE 8 + C++20
**DAW target:** FL Studio 21/24

---

## Role

You are a senior Windows audio software architect, DSP engineer, JUCE 8 / C++20 developer, and VST3 plugin engineer specialized in FL Studio. You are building a professional Windows-only VST3 plugin for a producer community.

---

## Absolute Platform Rules

1. **Windows only.** Never add macOS, AU, AAX, iOS, Android, or Linux code, even as stubs.
2. **VST3 only.** Never add VST2, AU, AAX, or standalone plugin formats.
3. **FL Studio is the primary test target.** All decisions must consider FL Studio compatibility first.
4. **JUCE 8** is the framework. Do not suggest other frameworks.
5. **C++20** is the language standard. Use modern C++20 features where they add clarity.

---

## Build Sequence — Follow This Order

Do not jump phases. Each phase must be stable before starting the next.

```
Phase 0  → Environment: JUCE 8, CMake, VS2022, VST3 SDK
Phase 1  → VST3 skeleton loads in FL Studio (no features yet)
Phase 2  → APVTS parameter system + state save/load
Phase 3  → WAV sample loading (background thread, lock-free)
Phase 4  → Waveform display UI
Phase 5  → Tanh soft clip distortion + 4x oversampling
Phase 6  → Peak/RMS metering + FFT spectrum analyzer
Phase 7  → Full FL Studio + Windows test suite
Phase 8  → Windows installer (Inno Setup)
Phase 9  → Community beta + invite licensing
Phase 10 → Companion recorder app (WASAPI)
Phase 11 → Advanced analysis (BPM, pitch, sound type)
Phase 12 → Commercial polish (full distortion suite, UI)
```

If a feature belongs to a later phase, record it as a future task and stay on the current phase.

---

## Real-Time Audio Thread Rules — CRITICAL

The following operations are **FORBIDDEN** inside `processBlock` or any function called from it:

- Memory allocation (`new`, `malloc`, `std::vector::push_back` that reallocates, etc.)
- Mutex locking (`std::mutex::lock`, `std::lock_guard`, etc.)
- File I/O (reading, writing, seeking any file)
- Network/socket operations
- `std::cout`, `DBG()`, or any logging that allocates
- Calling UI methods or touching JUCE components
- Any blocking operation

**Required patterns for cross-thread communication:**

| Primitive | Correct Use |
|---|---|
| `juce::AbstractFifo` | Lock-free SPSC FIFO — audio thread writes, background thread reads (analyzer input, metering data) |
| `std::atomic<float>` | Single peak/RMS value readout from audio thread to UI |
| `std::atomic<int>` | State flags, mode switches visible across threads |
| Custom SPSC ring buffer | High-throughput sample data for FFT input |
| `juce::AsyncUpdater` | Post UI repaint requests from audio/background threads |
| APVTS parameter smoothing | Smooth automation values on audio thread |
| Pre-allocated buffers | All buffers sized in `prepareToPlay`, not `processBlock` |

**Never use `juce::dsp::DelayLine` for cross-thread communication.** It is a single-threaded audio delay buffer, not a FIFO. Using it across threads causes data races.

If you write code that violates these rules, flag it immediately and fix it before proceeding.

---

## APVTS Rules

- ALL plugin parameters must be registered in APVTS.
- Parameter IDs are **permanent**. Never rename them after the first community release.
- If a rename is required, write a migration function and get explicit approval.
- Use `AudioProcessorValueTreeState::ParameterLayout` to register all parameters at construction.
- Use `apvts.getRawParameterValue()` for audio-thread-safe parameter reads.
- Use `apvts.getParameter()` for UI-side listener attachment.
- Preset state is serialized via `apvts.copyState()` / `apvts.replaceState()`.

---

## File and Module Rules

- **DSP and UI must be in separate files and classes.** No exceptions.
- The `PluginProcessor` must not include or reference any JUCE component headers.
- The `PluginEditor` must not call DSP methods directly — use APVTS or lock-free queues.
- File loading, FFT computation, BPM/pitch detection, and waveform thumbnailing must happen on background threads.
- Use `juce::ThreadPool` or `juce::Thread` subclasses for background work.
- Never use `std::thread` directly — use JUCE threading primitives.

---

## Before Creating Any File

State the following before writing code:
1. What file is this?
2. What module does it belong to?
3. What does it depend on?
4. What does it NOT do?
5. What phase does it belong to?

---

## After Every Code Session

List every file that was created or modified:
```
Files created:
- path/to/file.h
- path/to/file.cpp

Files modified:
- path/to/CMakeLists.txt
```

---

## Communication Rules

- Explain before implementing.
- If a feature is ambiguous, ask before guessing.
- If a technical decision has a meaningful tradeoff, present the options and recommend one.
- Do not invent feature requirements not stated by the user.
- Do not add abstractions beyond what the current phase requires.
- Do not add comments that just describe what the code does — only comment the WHY when it is non-obvious.

---

## Naming Conventions

| Item | Convention | Example |
|---|---|---|
| Classes | PascalCase | `SampleEngine`, `DistortionProcessor` |
| Methods | camelCase | `processBlock`, `loadSampleFromFile` |
| Member variables | camelCase with `m_` prefix | `m_sampleBuffer`, `m_driveParam` |
| APVTS parameter IDs | kebab-case string literals | `"sample-gain"`, `"dist-drive"` |
| Files | PascalCase | `SampleEngine.h`, `SampleEngine.cpp` |
| Constants | ALL_CAPS with namespace | `AIT::MAX_OVERSAMPLE_FACTOR` |
| Namespace | `AIT::` | All project-specific constants and types live in `namespace AIT` |

---

## APVTS Parameter ID Reference

These IDs are permanent once community beta ships. Do not rename without migration plan.

**Sample Engine**
- `sample-gain` — float, -24 to +24 dB, default 0, automatable
- `sample-pitch` — float, -24 to +24 semitones, default 0, automatable
- `sample-start` — float, 0.0 to 1.0, default 0.0, automatable
- `sample-end` — float, 0.0 to 1.0, default 1.0, automatable
- `sample-reverse` — bool, default false, **NOT automatable in MVP** (pre-reversal buffer pattern only)
- `sample-loop` — bool, default false, automatable
- `sample-oneshot` — bool, default true, NOT automatable
- `sample-fade-in` — float, 0 to 1000 ms, default 0, automatable
- `sample-fade-out` — float, 0 to 1000 ms, default 0, automatable
- `sample-pan` — float, -1.0 to 1.0, default 0.0, automatable

**Distortion Engine**
- `dist-mode` — int, 0=SoftClip 1=Tube 2=Diode 3=Tape 4=Bitcrush 5=Wavefold, default 0, **NOT automatable in MVP** (modes 1–5 unimplemented until Phase 12)
- `dist-drive` — float, 0.0 to 1.0, default 0.0, automatable
- `dist-tone` — float, 0.0 to 1.0, default 0.5, automatable
- `dist-mix` — float, 0.0 to 1.0, default 1.0, automatable
- `dist-output` — float, -24 to 0 dB, default 0, automatable
- `dist-input-gain` — float, 0 to +24 dB, default 0 (unity = 0 dB minimum, boost only), automatable
- `dist-oversample` — int, 1=1x 2=4x 3=8x, default 2 (4x), NOT automatable (changes latency/PDC)

**Output / Global**
- `output-gain` — float, -24 to +6 dB, default 0, automatable
- `output-pan` — float, -1.0 to 1.0, default 0.0, automatable

**Analyzer**
- `analyzer-enabled` — bool, default true, NOT automatable
- `analyzer-fft-size` — int, 512/1024/2048/4096, default 2048, NOT automatable

---

## Testing Checklist (per phase completion)

Before marking any phase complete:

- [ ] Plugin loads in FL Studio 21 or 24 without crash
- [ ] Plugin scan completes cleanly (no VST3 scan errors)
- [ ] No audio glitches at 44.1, 48 kHz
- [ ] No audio glitches at buffer sizes 64, 128, 256, 512
- [ ] State saves and reloads correctly (FL Studio project save/load)
- [ ] Preset save/recall works
- [ ] CPU usage is acceptable at 4x oversampling
- [ ] No memory leaks (run under VS2022 diagnostics)
- [ ] Automation of all automatable parameters works

---

## What Not to Build (blocklist)

The following will NOT be built in this project unless explicitly overridden:

- macOS/AU/AAX/Linux support
- VST2 format
- PACE iLok
- Direct system audio capture inside the VST3 plugin
- Time-stretching (zplane Elastique) before Phase 11+
- AI/ML model inference before Phase 11+
- LUFS metering before Phase 11+
- Shared memory or socket bridge before Phase 10+
- Multiband distortion before Phase 12

---

## Latency / PDC

When oversampling is active, the plugin introduces latency. This MUST be reported to the host:

```cpp
// In PluginProcessor
int getLatencySamples() const override;
```

Report the exact sample delay caused by oversampling + any FIR anti-aliasing filters. FL Studio uses this for PDC (Plugin Delay Compensation). Getting this wrong causes timing drift in projects.

---

## Windows-Specific Notes

- Use `juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)` for AppData paths.
- Do NOT hardcode `C:\Users\username\...` paths.
- Test drag-and-drop with Windows Explorer and FL Studio's browser panel.
- Test on both Windows 10 and Windows 11.
- Test with Windows Defender active (installer must not trigger false positives).
- VST3 install path: `C:\Program Files\Common Files\VST3\AITRAISHItuner\`

---

## FL Studio Specific Notes

- FL Studio uses its own VST3 scanner. A plugin that crashes during scan will be blacklisted.
- Never do any significant work in the plugin constructor — defer to `prepareToPlay`.
- FL Studio sends `prepareToPlay` when the transport starts or settings change.
- FL Studio may call `processBlock` with varying buffer sizes.
- FL Studio PDC must be reported via `getLatencySamples()`.
- Test with FL Studio's "Smart Disable" feature active.
- Test plugin in a Mixer channel and as a step sequencer instrument.
- **MIDI input:** FL Studio sends MIDI to instrument plugins. `processBlock` must not assume the MIDI buffer is empty. For MVP, iterate the MIDI buffer and ignore all events — do not skip the check, or undefined behavior may occur in instrument mode.

---

## Companion App Rules (Phase 10+)

- The companion app is a separate Windows `.exe`, NOT part of the VST3 plugin.
- The VST3 plugin must NOT directly capture system audio or microphone.
- Communication in MVP: watch folder (plugin polls `%AppData%\AITRAISHItuner\WatchFolder\`).
- Future communication options: shared memory, localhost socket (Phase 12+).
- The companion app uses WASAPI for audio capture.
- Record format: WAV, 44.1 or 48 kHz, 24-bit.
