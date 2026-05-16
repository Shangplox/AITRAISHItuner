# Architecture

## Thread Model

The plugin runs on three threads. **Violating these boundaries will crash FL Studio.**

```
┌──────────────────────────────────────────────────────────────────┐
│  AUDIO THREAD  (processBlock)                                    │
│                                                                  │
│  MIDI buffer → SampleEngine → DistortionEngine → Output          │
│                                    │                             │
│  Peak/RMS → std::atomic<float> ──────────────┐                  │
│  (future) analyzer data → AbstractFifo ──────┤                  │
└──────────────────────────────────────────────┼──────────────────┘
                                               │ safe atomic read
┌──────────────────────────────────────────────┼──────────────────┐
│  MESSAGE THREAD  (UI / JUCE event loop)      │                  │
│                                              │                  │
│  WaveformDisplay   MeterDisplay   HeaderBar  │                  │
│  SamplePanel       DistortionPanel           │                  │
│  ← reads atomics via juce::Timer callbacks ──┘                  │
└──────────────────────────────────────────────────────────────────┘
                    ▲
                    │ APVTS (thread-safe parameter reads/writes)
┌───────────────────┴──────────────────────────────────────────────┐
│  BACKGROUND THREAD  (SampleLoader : juce::Thread)                │
│                                                                  │
│  File I/O → decode WAV → write to inactive buffer               │
│  → callAsync → message thread → SampleEngine::commitPendingBuffer│
└──────────────────────────────────────────────────────────────────┘
```

### What's forbidden on the audio thread

- `new`, `delete`, `malloc`, `free` — no heap allocation
- `std::mutex::lock`, `std::lock_guard` — no blocking locks
- File I/O of any kind
- `std::cout`, `DBG()`, any logging that allocates
- Calling any JUCE Component method
- Any blocking operation

### Cross-thread primitives used

| Primitive | Direction | Purpose |
|---|---|---|
| `std::atomic<float>` | Audio → UI | Peak/RMS metering |
| `std::atomic<int>` | Message → Audio | Sample buffer index (double-buffer swap) |
| `std::atomic<bool>` | Message → Audio | `hasSample` flag |
| `APVTS::getRawParameterValue()` | UI → Audio | All parameter values |
| `juce::MessageManager::callAsync` | Background → Message | Deliver loaded sample |
| `juce::SmoothedValue<float>` | Audio thread only | Per-parameter zipper-noise prevention |

---

## Module Map

```
src/
├── Plugin/
│   ├── PluginProcessor   — AudioProcessor wrapper, owns all DSP objects
│   ├── PluginEditor      — AudioProcessorEditor, owns all UI panels
│   ├── Parameters        — All APVTS IDs (constexpr) + createParameterLayout()
│   └── Presets           — JSON preset load/save, factory preset discovery
│
├── DSP/                  — No JUCE UI headers in this directory
│   ├── SampleEngine      — Monophonic MIDI-triggered sample playback
│   ├── SampleLoader      — Background WAV loading (juce::Thread subclass)
│   ├── DistortionEngine  — Soft-clip + oversampling + tone + mix chain
│   ├── OversamplingWrapper — Wraps juce::dsp::Oversampling (1x / 4x)
│   ├── ToneFilter        — Post-distortion biquad LP tone control
│   └── Metering          — Lock-free peak/RMS via std::atomic (header-only)
│
├── UI/                   — JUCE component layer, no DSP logic
│   ├── Theme             — All colors, fonts, spacing (constexpr, header-only)
│   ├── PluginLookAndFeel — Custom JUCE LookAndFeel subclass
│   ├── KnobControl       — Rotary slider + label + tooltip
│   ├── WaveformDisplay   — Read-only waveform + drag-and-drop target
│   ├── MeterDisplay      — Peak/RMS meter bars (reads Metering atomics)
│   ├── SamplePanel       — Sample engine controls
│   ├── DistortionPanel   — Distortion engine controls
│   ├── OutputPanel       — Output gain and pan
│   └── HeaderBar         — Plugin name, preset selector, save button
│
├── State/
│   ├── StateManager      — getStateInformation / setStateInformation logic
│   └── Serialization     — Binary (XML/APVTS) and JSON preset serialization
│
└── Utils/
    ├── MathUtils         — dBToLinear, softClip, semitoneRatio, constantPowerPan
    ├── AudioBufferUtils  — applyGain, applyPan helpers
    ├── Smoothing         — Alias for juce::SmoothedValue
    └── Logging           — AIT_LOG / AIT_LOG_ERR (debug-only macros)
```

---

## Double-Buffer Sample Swap

`SampleEngine` uses two `juce::AudioBuffer<float>` members to allow lock-free sample loading:

```
m_buffers[0]  ◄── audio thread reads this when m_activeIndex == 0
m_buffers[1]  ◄── audio thread reads this when m_activeIndex == 1

When SampleLoader finishes:
  1. Write new audio data into m_buffers[1 - m_activeIndex]  (inactive buffer)
  2. std::atomic::store(new_index, memory_order_release)     (atomic swap)
  3. Audio thread sees new index on next block, reads new buffer

No locks. No blocking. No race condition.
```

The release/acquire memory ordering ensures all writes to the inactive buffer are visible to the audio thread after it reads the new index.

---

## PDC (Plugin Delay Compensation)

When 4x oversampling is active, `juce::dsp::Oversampling` introduces latency. This is reported to the host via:

```cpp
int AITRAISHItunerProcessor::getLatencySamples() const override
{
    return m_reportedLatency;  // set in prepareToPlay from OversamplingWrapper
}
```

FL Studio reads `getLatencySamples()` and delays other tracks to compensate. **If this value is wrong, all tracks in the project will drift out of alignment.**

To verify: place a click on a reference track. Insert the plugin with 4x oversampling. The click in the plugin output should align with the reference after FL Studio applies PDC.

---

## Parameter Stability Contract

Parameter IDs in `Parameters.h` are **permanent** after first public release.

- Never rename a parameter ID.
- Never remove a parameter ID.
- Add new parameters with a higher version hint (the second argument to `juce::ParameterID`).
- If a rename is unavoidable, write a migration function in `StateManager::setState` that maps old IDs to new IDs before calling `m_apvts.replaceState`.

Breaking this contract corrupts saved FL Studio projects for all users.
