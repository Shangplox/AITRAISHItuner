#include "Parameters.h"

namespace AIT {

using APVTS = juce::AudioProcessorValueTreeState;
using FloatParam = juce::AudioParameterFloat;
using BoolParam  = juce::AudioParameterBool;
using IntParam   = juce::AudioParameterInt;
using PID        = juce::ParameterID;
using NRange     = juce::NormalisableRange<float>;

// Version hint: bump this when parameters are added, to distinguish preset versions.
// Keep at 1 until a breaking parameter change requires migration.
static constexpr int PARAM_VERSION = 1;

APVTS::ParameterLayout AIT::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ── Sample Engine ─────────────────────────────────────────────────────────
    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_GAIN, PARAM_VERSION}, "Sample Gain",
        NRange(-24.f, 24.f, 0.01f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("dB")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_PITCH, PARAM_VERSION}, "Pitch",
        NRange(-24.f, 24.f, 0.01f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("st")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_START, PARAM_VERSION}, "Start",
        NRange(0.f, 1.f, 0.001f), 0.f));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_END, PARAM_VERSION}, "End",
        NRange(0.f, 1.f, 0.001f), 1.f));

    params.push_back(std::make_unique<BoolParam>(
        PID{Params::SAMPLE_REVERSE, PARAM_VERSION}, "Reverse", false));

    params.push_back(std::make_unique<BoolParam>(
        PID{Params::SAMPLE_LOOP, PARAM_VERSION}, "Loop", false));

    params.push_back(std::make_unique<BoolParam>(
        PID{Params::SAMPLE_ONESHOT, PARAM_VERSION}, "One-Shot", true));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_FADE_IN, PARAM_VERSION}, "Fade In",
        NRange(0.f, 1000.f, 1.f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("ms")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_FADE_OUT, PARAM_VERSION}, "Fade Out",
        NRange(0.f, 1000.f, 1.f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("ms")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::SAMPLE_PAN, PARAM_VERSION}, "Sample Pan",
        NRange(-1.f, 1.f, 0.01f), 0.f));

    // ── Distortion Engine ─────────────────────────────────────────────────────
    params.push_back(std::make_unique<IntParam>(
        PID{Params::DIST_MODE, PARAM_VERSION}, "Dist Mode", 0, 5, 0));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::DIST_DRIVE, PARAM_VERSION}, "Drive",
        NRange(0.f, 1.f, 0.001f), 0.f));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::DIST_TONE, PARAM_VERSION}, "Tone",
        NRange(0.f, 1.f, 0.001f), 0.5f));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::DIST_MIX, PARAM_VERSION}, "Mix",
        NRange(0.f, 1.f, 0.001f), 1.f));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::DIST_OUTPUT, PARAM_VERSION}, "Dist Output",
        NRange(-24.f, 0.f, 0.01f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("dB")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::DIST_INPUT_GAIN, PARAM_VERSION}, "Input Gain",
        NRange(0.f, 24.f, 0.01f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("dB")));

    params.push_back(std::make_unique<IntParam>(
        PID{Params::DIST_OVERSAMPLE, PARAM_VERSION}, "Oversample", 1, 2, 2));

    // ── Output / Global ───────────────────────────────────────────────────────
    params.push_back(std::make_unique<FloatParam>(
        PID{Params::OUTPUT_GAIN, PARAM_VERSION}, "Output Gain",
        NRange(-24.f, 6.f, 0.01f), 0.f,
        juce::AudioParameterFloatAttributes{}.withLabel("dB")));

    params.push_back(std::make_unique<FloatParam>(
        PID{Params::OUTPUT_PAN, PARAM_VERSION}, "Output Pan",
        NRange(-1.f, 1.f, 0.01f), 0.f));

    // ── Analyzer ──────────────────────────────────────────────────────────────
    params.push_back(std::make_unique<BoolParam>(
        PID{Params::ANALYZER_ENABLED, PARAM_VERSION}, "Analyzer", true));

    params.push_back(std::make_unique<IntParam>(
        PID{Params::ANALYZER_FFT_SIZE, PARAM_VERSION}, "FFT Size", 512, 4096, 2048));

    return { params.begin(), params.end() };
}

} // namespace AIT
