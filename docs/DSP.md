# DSP Reference

## Signal Flow

```
MIDI Note On (note N, velocity V)
        │
        ▼  SampleEngine
  semitones = (N - 60) + sample-pitch
  ratio     = 2^(semitones / 12)
  position  += ratio  per output sample
  sample    = linear_interp(buffer, position) × gain × velocity × pan
        │
        ▼  DistortionEngine
  × inputGain
  → upsample (1x or 4x)
        │ (oversampled domain)
  y = tanh(x × k) / tanh(k)   where k = 1 + drive × 63
  → ToneFilter LP biquad
        │
  → downsample
  → wet/dry mix
  × outputGain
        │
        ▼  Output Section
  × outputGain × pan
  → metering (peak/RMS atomics)
  → DAW buffer
```

---

## Pitch Shifting (Sample Engine)

AITRAISHItuner uses **simple resampling** for pitch shifting. This changes playback speed proportionally to the pitch change.

### Formula

```
ratio = 2^(semitones / 12)
```

At every output sample, the read head advances by `ratio` fractional positions through the source buffer.

### Linear Interpolation

```cpp
int   posInt = (int)m_playPos;
float frac   = m_playPos - posInt;
float s      = buf[posInt] * (1.0 - frac) + buf[posInt + 1] * frac;
```

Linear interpolation is the minimum viable anti-aliasing for resampling. It is sufficient for transient-based samples (808s, kicks, one-shots). For sustained tones at large pitch offsets, higher-order interpolation (Hermite, sinc) would sound better — Phase 2 improvement.

### Limitation

At high pitch-up ratios (large positive semitones), the sample plays faster. This shortens perceived duration but does **not** change tempo. This is intentional and correct for most producer use cases. Time-stretching (WSOLA, phase vocoder) is Phase 2.

### MIDI pitch mapping

```
Root note: MIDI 60 (C3)
semitones = (midiNoteNumber - 60) + sample-pitch parameter
```

Playing MIDI note 72 (C4) with sample-pitch = 0 plays the sample one octave up.

---

## Soft-Clip Distortion

### Transfer function

```
k = 1 + drive × 63      (maps drive [0,1] to input scale [1, 64])
y = tanh(x × k) / tanh(k)
```

### Properties

- At `drive = 0`: k = 1, `tanh(x) / tanh(1) ≈ x` (approximately linear)
- At `drive = 1`: k = 64, heavy saturation
- **Level compensation**: `tanh(k) / tanh(k) = 1.0` for input x = ±1.0, so full-scale input → full-scale output at any drive setting
- **Odd symmetry**: `f(-x) = -f(x)` (generates only odd harmonics: 3rd, 5th, 7th, ...)
- **Bounded**: output always within [-1, 1] regardless of input

### Oversampling

Distortion (any nonlinear process) creates harmonic content above the Nyquist frequency. Without oversampling, these fold back into the audible range as aliasing — digital harshness.

4x oversampling raises the effective Nyquist from 22.05 kHz (at 44.1 kHz host rate) to ~88 kHz, then a polyphase IIR filter removes content above the original Nyquist during downsampling.

`juce::dsp::Oversampling<float>` with `filterHalfBandPolyphaseIIR` mode is used.

---

## Tone Filter

A 2nd-order Butterworth low-pass filter applied after distortion.

### Cutoff frequency mapping

```
fc = 500 × (20000/500)^tone   Hz
   = 500 × 40^tone             Hz
```

| Tone | fc    | Character |
|------|-------|-----------|
| 0.0  | 500 Hz | Dark, muddy |
| 0.25 | ~2 kHz | Warm |
| 0.5  | ~8 kHz | Neutral |
| 0.75 | ~14 kHz | Bright |
| 1.0  | 20 kHz | Near-bypass |

### Implementation

JUCE's `juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, fc)` computes bilinear-transform Butterworth coefficients. The `ProcessorDuplicator` applies the same filter to both stereo channels independently.

Coefficients are recomputed only when the tone parameter changes by more than 0.001 (cached to save CPU).

---

## Parameter Smoothing

All continuous parameters use `juce::SmoothedValue<float>` (linear smoothing) to prevent zipper noise. Smoothing happens on the audio thread at block rate.

| Parameter | Ramp time |
|---|---|
| Sample gain, pitch, pan | 10 ms |
| Start, end | 5 ms |
| Drive, tone, mix | 20 ms |
| Output gain, pan | 10 ms |

### Denormal protection

`juce::ScopedNoDenormals noDenormals;` is placed at the top of `processBlock`. This sets the CPU's FTZ (flush-to-zero) and DAZ (denormals-are-zero) bits for the duration of the block. Denormals in IIR filter state registers can cause CPU usage to spike 10–100x.

---

## Metering

Peak and RMS values are computed on the audio thread and stored in `std::atomic<float>` members.

**Peak**: per-block absolute maximum. Decays with a coefficient of 0.999 per block (approximately -0.1 dB per block at 44.1 kHz, 512 samples/block).

**RMS**: one-pole IIR smoothed: `rms_new = 0.05 × rms_block + 0.95 × rms_prev`. This gives approximately 100 ms time constant.

The UI reads these atomics via a 15 Hz timer with no blocking and no allocation.
