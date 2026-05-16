#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include "DSP/SampleEngine.h"
#include "DSP/SampleLoader.h"
#include "DSP/DistortionEngine.h"
#include "DSP/Metering.h"
#include "State/StateManager.h"
#include "Plugin/Presets.h"
#include "Utils/Smoothing.h"
#include "Utils/MathUtils.h"

// PluginProcessor is the thin plugin-format wrapper.
// It owns all DSP objects and wires APVTS parameter changes to them.
// No DSP logic lives in this file — it delegates entirely to the DSP layer.
class AITRAISHItunerProcessor : public juce::AudioProcessor
{
public:
    AITRAISHItunerProcessor();
    ~AITRAISHItunerProcessor() override;

    // ── AudioProcessor overrides ───────────────────────────────────────────────
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi()  const override { return true;  }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms()                               override { return 1; }
    int getCurrentProgram()                            override { return 0; }
    void setCurrentProgram(int)                        override {}
    const juce::String getProgramName(int)             override { return {}; }
    void changeProgramName(int, const juce::String&)   override {}

    void getStateInformation(juce::MemoryBlock& destData)          override;
    void setStateInformation(const void* data, int sizeInBytes)    override;

    // ── Accessors for PluginEditor ─────────────────────────────────────────────
    juce::AudioProcessorValueTreeState& getAPVTS()        { return m_apvts; }
    AIT::Metering&                      getMetering()     { return m_metering; }
    AIT::SampleEngine&                  getSampleEngine() { return m_sampleEngine; }
    AIT::Presets&                       getPresets()      { return m_presets; }

    // Triggers a background-thread WAV load. Safe to call from the UI thread.
    void loadSampleFile(const juce::File& file);

    juce::String getCurrentSamplePath() const;

private:
    // ── APVTS ──────────────────────────────────────────────────────────────────
    juce::AudioProcessorValueTreeState m_apvts;

    // Cached raw atomic parameter pointers (fetched once in prepareToPlay).
    std::atomic<float>* m_pSampleGain    {nullptr};
    std::atomic<float>* m_pSamplePitch   {nullptr};
    std::atomic<float>* m_pSampleStart   {nullptr};
    std::atomic<float>* m_pSampleEnd     {nullptr};
    std::atomic<float>* m_pSamplePan     {nullptr};
    std::atomic<float>* m_pSampleLoop    {nullptr};
    std::atomic<float>* m_pSampleOneShot {nullptr};
    std::atomic<float>* m_pDistDrive     {nullptr};
    std::atomic<float>* m_pDistTone      {nullptr};
    std::atomic<float>* m_pDistMix       {nullptr};
    std::atomic<float>* m_pDistOutput    {nullptr};
    std::atomic<float>* m_pDistInputGain {nullptr};
    std::atomic<float>* m_pDistOversample{nullptr};
    std::atomic<float>* m_pOutputGain    {nullptr};
    std::atomic<float>* m_pOutputPan     {nullptr};

    // Per-parameter smoothers (block-rate, avoids zipper noise).
    AIT::ParamSmoother<float> m_gainSmooth;
    AIT::ParamSmoother<float> m_pitchSmooth;
    AIT::ParamSmoother<float> m_startSmooth;
    AIT::ParamSmoother<float> m_endSmooth;
    AIT::ParamSmoother<float> m_panSmooth;
    AIT::ParamSmoother<float> m_driveSmooth;
    AIT::ParamSmoother<float> m_toneSmooth;
    AIT::ParamSmoother<float> m_distMixSmooth;
    AIT::ParamSmoother<float> m_distOutSmooth;
    AIT::ParamSmoother<float> m_distInGainSmooth;
    AIT::ParamSmoother<float> m_outGainSmooth;
    AIT::ParamSmoother<float> m_outPanSmooth;

    // ── DSP ────────────────────────────────────────────────────────────────────
    AIT::SampleEngine    m_sampleEngine;
    AIT::SampleLoader    m_sampleLoader;
    AIT::DistortionEngine m_distEngine;
    AIT::Metering        m_metering;

    // ── State / Presets ────────────────────────────────────────────────────────
    AIT::StateManager    m_stateManager;
    AIT::Presets         m_presets;
    juce::String         m_currentSamplePath;

    int m_reportedLatency{0};

    void updateLatency();
    void cacheParameterPointers();
    void initSmoothers(double sampleRate);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AITRAISHItunerProcessor)
};
