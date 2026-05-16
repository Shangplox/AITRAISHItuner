#include "OversamplingWrapper.h"

namespace AIT {

OversamplingWrapper::OversamplingWrapper() = default;

void OversamplingWrapper::prepare(int numChannels, double /*sampleRate*/, int /*maxBlockSize*/)
{
    // Polyphase IIR gives the best alias rejection for distortion use.
    m_oversampler4x = std::make_unique<juce::dsp::Oversampling<float>>(
        numChannels,
        2,   // factor exponent: 2^2 = 4x
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
        true,  // maximumQuality
        false  // isInterleavedData
    );

    // No sample-rate / block-size prep needed here — prepareToPlay on the
    // JUCE processor calls dsp::Oversampling::initProcessing internally
    // via juce_recommended_config_flags. We call it explicitly to be safe.
    // initProcessing expects maxBlockSize at the base (1x) rate.
    m_prepared = true;
}

void OversamplingWrapper::setMode(int mode)
{
    m_currentMode = juce::jlimit(1, 2, mode);
}

int OversamplingWrapper::getLatencyInSamples() const noexcept
{
    if (m_currentMode == 1 || !m_oversampler4x)
        return 0;
    return static_cast<int>(m_oversampler4x->getLatencyInSamples());
}

juce::dsp::AudioBlock<float>
OversamplingWrapper::processUp(juce::dsp::AudioBlock<float>& inputBlock) noexcept
{
    if (m_currentMode == 1 || !m_oversampler4x)
        return inputBlock;  // 1x: no upsampling, return input unchanged

    return m_oversampler4x->processSamplesUp(inputBlock);
}

void OversamplingWrapper::processDown(juce::dsp::AudioBlock<float>& outputBlock) noexcept
{
    if (m_currentMode == 1 || !m_oversampler4x)
        return;  // 1x: no downsampling needed

    m_oversampler4x->processSamplesDown(outputBlock);
}

void OversamplingWrapper::reset()
{
    if (m_oversampler4x)
        m_oversampler4x->reset();
}

} // namespace AIT
