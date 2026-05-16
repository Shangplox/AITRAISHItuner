# Roadmap

## Phase 1 — MVP (current)

- [x] CMake build system, JUCE 8 integration
- [x] Full parameter system (APVTS, all IDs)
- [x] MIDI-triggered sample playback
- [x] WAV loading on background thread
- [x] Pitch, gain, pan, start/end, loop, one-shot
- [x] Soft-clip tanh distortion
- [x] 4x oversampling with correct PDC latency reporting
- [x] Waveform display + drag-and-drop
- [x] Peak/RMS metering
- [x] Preset save/load (JSON)
- [x] State persistence (FL Studio project save/load)
- [x] DSP test suite

## Phase 2 — Extended DSP + CLAP

- [ ] Sample reverse (pre-reversal buffer pattern)
- [ ] Fade in / fade out envelopes
- [ ] CLAP format via `clap-juce-extensions`
- [ ] Tube, diode, tape, bitcrush, wavefold distortion modes
- [ ] 8x oversampling option
- [ ] WSOLA / phase-vocoder pitch shifting (tempo-preserving)
- [ ] Hermite interpolation for higher-quality resampling

## Phase 3 — Analysis

- [ ] FFT spectrum analyzer display (AbstractFifo → background FFT → AsyncUpdater)
- [ ] BPM detection (offline, on loaded sample)
- [ ] Root note / pitch detection (YIN algorithm)
- [ ] Sound type classification (808, kick, snare, hi-hat — ML or heuristic)

## Phase 4 — Companion App

- [ ] Separate Windows `.exe` application (not part of the VST3)
- [ ] WASAPI audio capture (microphone + system loopback)
- [ ] Record to WAV, write to `%AppData%\AITRAISHItuner\WatchFolder\`
- [ ] Plugin polls watch folder and auto-imports new files

## Phase 5 — Polish + Release

- [ ] Windows installer (Inno Setup)
- [ ] LUFS metering
- [ ] Advanced preset browser UI
- [ ] Full UI visual polish
- [ ] Windows Defender / antivirus pre-submission scan
- [ ] Community beta release

---

## Not planned

- macOS / AU / AAX / Linux support
- VST2 format
- PACE iLok
- AI/ML model inference inside the VST3
- Time-stretching before Phase 2
- Direct system audio capture inside the VST3 plugin
