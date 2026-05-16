#pragma once
#include <JuceHeader.h>

namespace AIT {

class Presets;

// Top bar: plugin name, preset selector, load/save buttons.
class HeaderBar : public juce::Component
{
public:
    // Callback: user wants to load a preset file.
    std::function<void(const juce::File&)> onPresetLoad;
    // Callback: user wants to save current state as a preset.
    std::function<void()> onPresetSave;
    // Callback: user opened an audio file via the Open button.
    std::function<void(const juce::File&)> onFileOpen;

    HeaderBar(Presets& presets, const juce::String& currentSamplePath);

    void paint(juce::Graphics&) override;
    void resized() override;

    void refreshPresetList();

private:
    Presets& m_presets;

    juce::Label    m_pluginNameLabel;
    juce::ComboBox m_presetBox;
    juce::TextButton m_saveBtn;
    juce::TextButton m_loadBtn;

    juce::Array<juce::File> m_presetFiles;
    std::unique_ptr<juce::FileChooser> m_fileChooser;

    void buildPresetList();
    void onPresetSelected(int comboId);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderBar)
};

} // namespace AIT
