#include "DistortionEngine.h"
#include "Utils/MathUtils.h"

namespace AIT {

void DistortionEngine::prepare(const juce::dsp::ProcessSpec& spec)
{
    m_sampleRate    = spec.sampleRate;
    m_maxBlockSize  = static_cast<int>(spec.maximumBlockSize);

    m_toneFilter.prepare(spec);

    // Pre-allocate dry buffer to avoid allocation in processBlock.
    m_dryBuffer.setSize(static_cast<int>(spec.numChannels), m_maxBlockSize, false, true, false);
}

void DistortionEngine::initOversampling(int numChannels, double sampleRate, int maxBlockSize)
{
    m_oversampling.prepare(numChannels, sampleRate, maxBlockSize);
    // Reset internal state of oversampler after re-init.
    m_oversampling.reset();
}

void DistortionEngine::setOversampleMode(int mode)
{
    m_oversampling.setMode(mode);
}

void DistortionEngine::process(juce::AudioBuffer<float>& buffer,
                                const DistortionParams& params) noexcept
{
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numSamples == 0)
        return;

    // Store dry signal for wet/dry mix later.
    // m_dryBuffer is pre-allocated — no heap ops here.
    for (int ch = 0; ch < numChannels; ++ch)
        m_dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // Apply input gain boost before distortion.
    buffer.applyGain(params.inputGainLinear);

    // Upsample (or passthrough if 1x).
    auto inputBlock = juce::dsp::AudioBlock<float>(buffer);
    auto processBlock = m_oversampling.processUp(inputBlock);

    // Soft-clip distortion in the (possibly upsampled) domain.
    applySoftClip(processBlock, params.drive);

    // Tone filter (LP) in the upsampled domain.
    // Note: the ProcessSpec passed to toneFilter must match the oversampled rate.
    // For MVP we apply tone AFTER downsampling to avoid recomputing at 4x rate.
    // Downsampling is done first, then tone is applied at base rate.
    m_oversampling.processDown(inputBlock);

    // Now inputBlock == buffer (back at base rate).
    // Apply tone filter at base rate.
    m_toneFilter.process(inputBlock, params.tone);

    // Apply distortion output gain.
    buffer.applyGain(params.outputGainLinear);

    // Wet/dry mix: output = mix * wet + (1 - mix) * dry.
    if (params.mix < 0.9999f)
    {
        const float wet = params.mix;
        const float dry = 1.f - wet;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* out     = buffer.getWritePointer(ch);
            const auto* d = m_dryBuffer.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
                out[i] = wet * out[i] + dry * d[i];
        }
    }
}

void DistortionEngine::applySoftClip(juce::dsp::AudioBlock<float>& block, float drive) noexcept
{
    const int numChannels = static_cast<int>(block.getNumChannels());
    const int numSamples  = static_cast<int>(block.getNumSamples());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = block.getChannelPointer(ch);
        for (int i = 0; i < numSamples; ++i)
            data[i] = softClip(data[i], drive);
    }
}

void DistortionEngine::reset()
{
    m_toneFilter.reset();
    m_oversampling.reset();
    m_dryBuffer.clear();
}

} // namespace AIT
