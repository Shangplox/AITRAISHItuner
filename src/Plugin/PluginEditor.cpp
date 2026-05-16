#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "UI/Theme.h"

using namespace AIT::Theme;

AITRAISHItunerEditor::AITRAISHItunerEditor(AITRAISHItunerProcessor& p)
    : AudioProcessorEditor(&p)
    , m_processor(p)
    , m_header   (p.getPresets(), p.getCurrentSamplePath())
    , m_waveform (p.getSampleEngine())
    , m_meter    (p.getMetering())
    , m_samplePanel  (p.getAPVTS())
    , m_distPanel    (p.getAPVTS())
    , m_outputPanel  (p.getAPVTS())
{
    setLookAndFeel(&m_laf);

    addAndMakeVisible(m_header);
    addAndMakeVisible(m_waveform);
    addAndMakeVisible(m_meter);
    addAndMakeVisible(m_samplePanel);
    addAndMakeVisible(m_distPanel);
    addAndMakeVisible(m_outputPanel);

    // Wire drag-and-drop and file open to the processor's sample loader.
    m_waveform.onFileDropped = [this](const juce::File& f) { onFileLoaded(f); };

    // Wire header bar callbacks.
    m_header.onPresetLoad = [this](const juce::File& f) { onPresetLoad(f); };
    m_header.onPresetSave = [this] { onPresetSave(); };
    m_header.onFileOpen   = [this](const juce::File& f) { onFileLoaded(f); };

    setSize(820, 520);
    setResizable(true, true);
    setResizeLimits(640, 400, 1600, 960);
}

AITRAISHItunerEditor::~AITRAISHItunerEditor()
{
    setLookAndFeel(nullptr);
}

void AITRAISHItunerEditor::paint(juce::Graphics& g)
{
    g.fillAll(colour(BACKGROUND_DARK));
}

void AITRAISHItunerEditor::resized()
{
    auto area = getLocalBounds();

    // Header bar across the top.
    m_header.setBounds(area.removeFromTop(HEADER_HEIGHT));
    area.removeFromTop(MARGIN);

    // Waveform display + meter side-by-side.
    auto waveformRow = area.removeFromTop(WAVEFORM_HEIGHT);
    m_meter.setBounds(waveformRow.removeFromRight(METER_WIDTH * 2 + MARGIN));
    waveformRow.removeFromRight(MARGIN);
    m_waveform.setBounds(waveformRow);

    area.removeFromTop(MARGIN);

    // Lower section: sample panel | distortion panel.
    const int bottomH = area.getHeight() - BOTTOM_BAR_H - MARGIN;
    auto controlsRow = area.removeFromTop(bottomH);

    const int sampleW  = static_cast<int>(controlsRow.getWidth() * 0.38f);
    const int distW    = static_cast<int>(controlsRow.getWidth() * 0.45f);

    m_samplePanel.setBounds(controlsRow.removeFromLeft(sampleW));
    controlsRow.removeFromLeft(MARGIN);
    m_distPanel.setBounds(controlsRow.removeFromLeft(distW));
    controlsRow.removeFromLeft(MARGIN);

    // Output panel takes remaining right space.
    m_outputPanel.setBounds(controlsRow);
}

void AITRAISHItunerEditor::onFileLoaded(const juce::File& file)
{
    m_processor.loadSampleFile(file);

    // Delay waveform refresh slightly to let the background loader finish.
    juce::Timer::callAfterDelay(300, [this] {
        m_waveform.refreshWaveform();
    });
}

void AITRAISHItunerEditor::onPresetLoad(const juce::File& presetFile)
{
    const auto samplePath = m_processor.getPresets().loadPreset(presetFile);
    if (samplePath.isNotEmpty())
    {
        const juce::File sampleFile(samplePath);
        if (sampleFile.existsAsFile())
            onFileLoaded(sampleFile);
    }
}

void AITRAISHItunerEditor::onPresetSave()
{
    auto* dialog = new juce::AlertWindow("Save Preset",
                                         "Enter a name for this preset:",
                                         juce::MessageBoxIconType::NoIcon);
    dialog->addTextEditor("presetName", "My Preset");
    dialog->addButton("Save",   1);
    dialog->addButton("Cancel", 0);
    dialog->enterModalState(true,
        juce::ModalCallbackFunction::create([this, dialog](int result) {
            if (result == 1)
            {
                const auto name = dialog->getTextEditorContents("presetName");
                if (name.isNotEmpty())
                {
                    m_processor.getPresets().saveUserPreset(
                        name, {}, "User", m_processor.getCurrentSamplePath());
                    m_header.refreshPresetList();
                }
            }
        }), true);
}
