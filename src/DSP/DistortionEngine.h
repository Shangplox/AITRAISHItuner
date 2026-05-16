#pragma once
#include <JuceHeader.h>
#include "OversamplingWrapper.h"
#include "ToneFilter.h"

namespace AIT {

// Parameters passed into DistortionEngine::process() each block.
struct DistortionParams
{
    float drive{0.f};             // 0–1
    float tone{0.5f};             // 0–1
    float mix{1.f};               // 0–1 wet/dry
    float inputGainLinear{1.f};   // linear gain before distortion
    float outputGainLinear{1.f};  // linear gain after distortion
    int   oversampleMode{2};      // 1=1x, 2=4x (not changed per-block in MVP)
};

// Distortion engine: input gain → upsample → soft-clip → tone → downsample → mix → output gain.
// No JUCE UI headers included — DSP only.
// Audio thread only.
class DistortionEngine
{
public:
    DistortionEngine() = default;
    ~DistortionEngine() = default;

    // Call from prepareToPlay. Must be called before process().
    void prepare(const juce::dsp::ProcessSpec& spec);

    // Call when dist-oversample parameter changes (non-audio thread, e.g. prepareToPlay).
    void setOversampleMode(int mode);

    // Process a buffer in-place. Output replaces input.
    void process(juce::AudioBuffer<float>& buffer, const DistortionParams& params) noexcept;

    // Returns current oversampling latency in samples.
    int getLatencyInSamples() const noexcept { return m_oversampling.getLatencyInSamples(); }

    void reset();

    // Called from prepareToPlay so the oversampler is initialised with correct block size.
    void initOversampling(int numChannels, double sampleRate, int maxBlockSize);

private:
    OversamplingWrapper m_oversampling;
    ToneFilter          m_toneFilter;

    // Scratch buffer for dry signal (wet/dry mix without allocation on audio thread).
    juce::AudioBuffer<float> m_dryBuffer;

    double m_sampleRate{44100.0};
    int    m_maxBlockSize{512};

    // Applies the soft-clip transfer function sample-by-sample to an AudioBlock.
    void applySoftClip(juce::dsp::AudioBlock<float>& block, float drive) noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionEngine)
};

} // namespace AIT
