#pragma once
#include <JuceHeader.h>

namespace AIT {

// Wraps juce::dsp::Oversampling for 1x and 4x modes.
// Oversample mode 1 = 1x (passthrough, zero latency).
// Oversample mode 2 = 4x (anti-aliased, introduces latency).
//
// The plugin's dist-oversample parameter stores 1 or 2.
// Call setMode() when the parameter changes (not automatable — only on non-audio threads).
// processUp/processDown are called on the audio thread.
class OversamplingWrapper
{
public:
    OversamplingWrapper();

    // Call during prepareToPlay. numChannels must match the bus layout.
    void prepare(int numChannels, double sampleRate, int maxBlockSize);

    // Change between 1x (mode=1) and 4x (mode=2).
    // Only call this from a non-audio thread (prepareToPlay, parameter change callback).
    void setMode(int mode);

    int  getCurrentMode() const noexcept { return m_currentMode; }

    // Returns extra latency in samples introduced by current mode.
    // Report this from PluginProcessor::getLatencySamples().
    int  getLatencyInSamples() const noexcept;

    // Upsample the block. Returns a reference to the upsampled block.
    // Call before distortion processing.
    juce::dsp::AudioBlock<float> processUp(juce::dsp::AudioBlock<float>& inputBlock) noexcept;

    // Downsample back to base rate after distortion processing.
    void processDown(juce::dsp::AudioBlock<float>& outputBlock) noexcept;

    void reset();

private:
    // Two oversampler instances: index 0 = 1x (no-op), index 1 = 4x.
    // We always use 1x as a passthrough (no actual oversampler needed for 1x).
    std::unique_ptr<juce::dsp::Oversampling<float>> m_oversampler4x;

    int m_currentMode{2};  // default to 4x matching parameter default
    bool m_prepared{false};
};

} // namespace AIT
