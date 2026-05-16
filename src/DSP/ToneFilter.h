#pragma once
#include <JuceHeader.h>

namespace AIT {

// Post-distortion tone control — a low-pass filter whose cutoff frequency
// is mapped from the tone parameter [0, 1].
// tone=0.0 → dark:  fc = 500 Hz
// tone=0.5 → neutral: fc ≈ 8 kHz
// tone=1.0 → bright: fc = 20 kHz (near bypass for most content)
//
// Uses a 2nd-order Butterworth LP via juce::dsp::IIR (stereo duplicate).
// Must be called from audio thread only.
class ToneFilter
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block, float tone) noexcept;
    void reset();

private:
    using IIRCoeffs = juce::dsp::IIR::Coefficients<float>;
    using IIRFilter = juce::dsp::IIR::Filter<float>;
    using StereoFilter = juce::dsp::ProcessorDuplicator<IIRFilter, IIRCoeffs>;

    StereoFilter m_filter;
    double m_sampleRate{44100.0};
    float  m_lastTone{-1.f};  // cached to avoid recomputing coefficients every block
};

} // namespace AIT
