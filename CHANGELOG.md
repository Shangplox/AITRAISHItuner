# Changelog

All notable changes to AITRAISHItuner are documented here.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).  
Version scheme: `MAJOR.MINOR.PATCH`.

---

## [Unreleased]

### Added

- Complete CMake build system with JUCE 8 via FetchContent
- All APVTS parameter IDs (permanent, versioned)
- MIDI-triggered monophonic sample engine with double-buffer lock-free loading
- WAV loading on background thread (`SampleLoader`)
- Simple resampling pitch shift (±24 semitones, ±24 semitones MIDI offset)
- Soft-clip tanh distortion engine with 1x/4x oversampling
- Post-distortion tone LP filter (biquad, 500 Hz → 20 kHz)
- Wet/dry mix, input gain, output gain
- Correct PDC latency reporting (`getLatencySamples()`)
- Peak/RMS metering via lock-free atomics
- Waveform display with drag-and-drop WAV import
- All UI panels: Sample, Distortion, Output, Header
- Preset system: JSON load/save, factory presets (Default, 808 Crunch, Heavy Clip)
- State save/load (FL Studio project persistence via `getStateInformation` / `setStateInformation`)
- Catch2 DSP test suite
- Full documentation: BUILDING.md, ARCHITECTURE.md, DSP.md, PRESET_FORMAT.md, etc.

---

## [0.1.0] — TBD

First public MVP release.
