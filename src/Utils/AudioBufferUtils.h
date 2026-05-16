#pragma once
#include <JuceHeader.h>

namespace AIT {

// Apply a linear gain scalar to all samples in a buffer. Audio-thread safe.
inline void applyGain(juce::AudioBuffer<float>& buffer, float gain) noexcept
{
    buffer.applyGain(gain);
}

// Apply independent per-channel gains (for panning).
inline void applyPan(juce::AudioBuffer<float>& buffer, float gainL, float gainR) noexcept
{
    if (buffer.getNumChannels() >= 1)
        buffer.applyGain(0, 0, buffer.getNumSamples(), gainL);
    if (buffer.getNumChannels() >= 2)
        buffer.applyGain(1, 0, buffer.getNumSamples(), gainR);
}

// Hard-clip to [-1, 1] as a safety limiter. Not used in the DSP signal path
// (distortion is handled by DistortionEngine), but useful as a final guard.
inline void hardClipSafety(juce::AudioBuffer<float>& buffer) noexcept
{
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] = juce::jlimit(-1.f, 1.f, data[i]);
    }
}

// Flush denormals. Call via juce::ScopedNoDenormals at the top of processBlock.
// Kept here as a reminder — do NOT call manually inside processBlock.
// Usage: juce::ScopedNoDenormals noDenormals;

} // namespace AIT
