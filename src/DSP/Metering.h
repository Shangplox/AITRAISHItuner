#pragma once
#include <JuceHeader.h>
#include <atomic>
#include "Utils/MathUtils.h"

namespace AIT {

// Lock-free peak and RMS metering. Audio thread writes, UI reads via atomics.
// All values stored as linear (not dB). UI converts on read.
class Metering
{
public:
    // Called from audio thread inside processBlock.
    void process(const juce::AudioBuffer<float>& buffer) noexcept
    {
        const int numSamples = buffer.getNumSamples();
        if (numSamples == 0)
            return;

        // Peak: absolute max per channel
        const float rawL = buffer.getNumChannels() > 0
            ? buffer.getMagnitude(0, 0, numSamples) : 0.f;
        const float rawR = buffer.getNumChannels() > 1
            ? buffer.getMagnitude(1, 0, numSamples) : rawL;

        // Smooth peak decay (hold ~300ms, then decay)
        // Simple exponential hold: no fancy hold timer in MVP
        const float decayCoeff = 0.999f;  // ~44 samples at 44.1 kHz to decay ~0.1 dB
        m_peakL.store(std::max(rawL, m_peakL.load(std::memory_order_relaxed) * decayCoeff),
                      std::memory_order_relaxed);
        m_peakR.store(std::max(rawR, m_peakR.load(std::memory_order_relaxed) * decayCoeff),
                      std::memory_order_relaxed);

        // RMS: accumulate sum-of-squares per block, one-pole smoothed
        auto calcRms = [&](int ch) -> float {
            if (ch >= buffer.getNumChannels()) return 0.f;
            const auto* data = buffer.getReadPointer(ch);
            float sum = 0.f;
            for (int i = 0; i < numSamples; ++i)
                sum += data[i] * data[i];
            return std::sqrt(sum / static_cast<float>(numSamples));
        };

        const float rmsAlpha = 0.05f;  // smoothing coefficient
        const float newRmsL = calcRms(0);
        const float newRmsR = calcRms(1);
        m_rmsL.store(rmsAlpha * newRmsL + (1.f - rmsAlpha) * m_rmsL.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
        m_rmsR.store(rmsAlpha * newRmsR + (1.f - rmsAlpha) * m_rmsR.load(std::memory_order_relaxed),
                     std::memory_order_relaxed);
    }

    // UI thread reads — no blocking, no allocation.
    float getPeakL() const noexcept { return m_peakL.load(std::memory_order_relaxed); }
    float getPeakR() const noexcept { return m_peakR.load(std::memory_order_relaxed); }
    float getRmsL()  const noexcept { return m_rmsL.load(std::memory_order_relaxed); }
    float getRmsR()  const noexcept { return m_rmsR.load(std::memory_order_relaxed); }

    float getPeakLdB() const noexcept { return linearTodB(getPeakL()); }
    float getPeakRdB() const noexcept { return linearTodB(getPeakR()); }
    float getRmsLdB()  const noexcept { return linearTodB(getRmsL()); }
    float getRmsRdB()  const noexcept { return linearTodB(getRmsR()); }

    void reset() noexcept
    {
        m_peakL.store(0.f); m_peakR.store(0.f);
        m_rmsL.store(0.f);  m_rmsR.store(0.f);
    }

private:
    std::atomic<float> m_peakL{0.f};
    std::atomic<float> m_peakR{0.f};
    std::atomic<float> m_rmsL{0.f};
    std::atomic<float> m_rmsR{0.f};
};

} // namespace AIT
