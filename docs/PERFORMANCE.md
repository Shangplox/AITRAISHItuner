# Performance

## CPU targets

| Configuration | Expected CPU | Measured on |
|---|---|---|
| 1x oversample, 512 samples, 44.1 kHz | < 1% | Target — not yet measured |
| 4x oversample, 512 samples, 44.1 kHz | 2–5% | Target — not yet measured |
| 4x oversample, 64 samples, 44.1 kHz | 5–10% | Target — higher because of callback overhead |

Measure CPU in FL Studio using the Performance Monitor (F11). These targets are guidelines — actual usage depends on the host machine.

## Buffer size behavior

The plugin is tested at buffer sizes 64, 128, 256, 512, and 1024 samples. All pre-allocation happens in `prepareToPlay`, not `processBlock`.

For very small buffers (32 samples), FL Studio rarely uses this in practice. The plugin should handle it, but it is not a primary test target.

## Denormal protection

`juce::ScopedNoDenormals` is active for the entire `processBlock`. This prevents denormal floating-point numbers from triggering the hardware's "denormal assist" mode, which can cause 100x CPU spikes in IIR filter chains.

## Memory usage

All sample data is held in memory. There is no streaming from disk during playback.

Maximum memory usage depends on sample size. A 10-second stereo 44.1 kHz WAV at 32-bit float uses approximately 3.4 MB.

Two copies of the buffer are maintained at all times for the double-buffer lock-free swap (approximately 2× sample size in RAM).

## Profiling

To profile in Debug mode:

1. Open the VS2022 solution in `build/debug/`.
2. Use the Performance Profiler (Alt+F2 → CPU Usage).
3. Profile while FL Studio plays a project with the plugin active.

Common bottlenecks in audio plugins:
- IIR filter coefficient recomputation every block (solved: only recompute when parameter changes)
- Memory allocation in `processBlock` (solved: all buffers pre-allocated in `prepareToPlay`)
- Cache misses on large sample buffers (mitigation: linear read-access pattern is cache-friendly)

## Oversampling latency

| Mode | Latency |
|---|---|
| 1x | 0 samples |
| 4x | ~32–64 samples (measured via `juce::dsp::Oversampling::getLatencyInSamples()`) |

This latency is reported to the host via `getLatencySamples()`. FL Studio applies PDC automatically.

**After changing the oversample mode**, the new latency is reported in the next `prepareToPlay` call. FL Studio triggers `prepareToPlay` when the transport is stopped and restarted, or when the buffer size changes.
