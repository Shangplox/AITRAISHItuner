#pragma once
#include <JuceHeader.h>
#include "UI/PluginLookAndFeel.h"
#include "UI/WaveformDisplay.h"
#include "UI/MeterDisplay.h"
#include "UI/SamplePanel.h"
#include "UI/DistortionPanel.h"
#include "UI/OutputPanel.h"
#include "UI/HeaderBar.h"

class AITRAISHItunerProcessor;

class AITRAISHItunerEditor : public juce::AudioProcessorEditor
{
public:
    explicit AITRAISHItunerEditor(AITRAISHItunerProcessor& p);
    ~AITRAISHItunerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AITRAISHItunerProcessor& m_processor;

    AIT::PluginLookAndFeel  m_laf;
    AIT::HeaderBar          m_header;
    AIT::WaveformDisplay    m_waveform;
    AIT::MeterDisplay       m_meter;
    AIT::SamplePanel        m_samplePanel;
    AIT::DistortionPanel    m_distPanel;
    AIT::OutputPanel        m_outputPanel;

    void onFileLoaded(const juce::File& file);
    void onPresetLoad(const juce::File& presetFile);
    void onPresetSave();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AITRAISHItunerEditor)
};
