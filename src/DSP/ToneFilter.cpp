#include "ToneFilter.h"

namespace AIT {

void ToneFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    m_sampleRate = spec.sampleRate;
    m_lastTone   = -1.f;  // force coefficient update on first process call
    m_filter.prepare(spec);
}

void ToneFilter::process(juce::dsp::AudioBlock<float>& block, float tone) noexcept
{
    // Recompute LP coefficients only when tone changes (saves CPU).
    if (std::abs(tone - m_lastTone) > 0.001f)
    {
        m_lastTone = tone;

        // Exponential mapping: tone 0→1 maps cutoff 500 Hz → 20 000 Hz.
        // log(20000/500) / log(2) ≈ 5.32 octaves of range.
        const double minFreq =  500.0;
        const double maxFreq = 20000.0;
        const double fc = minFreq * std::pow(maxFreq / minFreq, static_cast<double>(tone));
        const double clampedFc = std::min(fc, m_sampleRate * 0.49);  // stay below Nyquist

        *m_filter.state = *IIRCoeffs::makeLowPass(m_sampleRate, clampedFc);
    }

    juce::dsp::ProcessContextReplacing<float> ctx(block);
    m_filter.process(ctx);
}

void ToneFilter::reset()
{
    m_filter.reset();
    m_lastTone = -1.f;
}

} // namespace AIT
