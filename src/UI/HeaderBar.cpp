#include "HeaderBar.h"
#include "Theme.h"
#include "Plugin/Presets.h"

namespace AIT {

HeaderBar::HeaderBar(Presets& presets, const juce::String& /*currentSamplePath*/)
    : m_presets(presets)
{
    m_pluginNameLabel.setText("AITRAISHItuner", juce::dontSendNotification);
    m_pluginNameLabel.setFont(juce::Font(Theme::FONT_TITLE, juce::Font::bold));
    m_pluginNameLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_PRIMARY));
    m_pluginNameLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_pluginNameLabel);

    m_presetBox.setTextWhenNothingSelected("-- Preset --");
    m_presetBox.setTooltip("Load a factory or user preset");
    m_presetBox.onChange = [this] {
        const int id = m_presetBox.getSelectedId();
        if (id > 0 && id <= m_presetFiles.size())
            onPresetSelected(id);
    };
    addAndMakeVisible(m_presetBox);

    m_saveBtn.setButtonText(juce::String::fromUTF8("\xf0\x9f\x92\xbe Save"));
    m_saveBtn.setTooltip("Save current settings as a user preset");
    m_saveBtn.onClick = [this] { if (onPresetSave) onPresetSave(); };
    addAndMakeVisible(m_saveBtn);

    m_loadBtn.setButtonText(juce::String::fromUTF8("\xf0\x9f\x93\x82 Open"));
    m_loadBtn.setTooltip("Open a WAV file for sample playback");
    m_loadBtn.onClick = [this] {
        m_fileChooser = std::make_unique<juce::FileChooser>(
            "Open audio file", juce::File{}, "*.wav;*.aif;*.aiff;*.flac;*.mp3");
        m_fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc) {
                const auto file = fc.getResult();
                if (file.existsAsFile() && onFileOpen)
                    onFileOpen(file);
            });
    };
    addAndMakeVisible(m_loadBtn);

    buildPresetList();
}

void HeaderBar::paint(juce::Graphics& g)
{
    g.setColour(Theme::colour(Theme::BACKGROUND_MID));
    g.fillRect(getLocalBounds());

    // Bottom separator line.
    g.setColour(Theme::colour(Theme::SEPARATOR));
    g.drawLine(0.f, static_cast<float>(getHeight() - 1),
               static_cast<float>(getWidth()), static_cast<float>(getHeight() - 1), 1.f);
}

void HeaderBar::resized()
{
    auto area = getLocalBounds().reduced(Theme::PADDING, 0);
    m_pluginNameLabel.setBounds(area.removeFromLeft(160));
    m_loadBtn.setBounds(area.removeFromRight(70));
    area.removeFromRight(4);
    m_saveBtn.setBounds(area.removeFromRight(70));
    area.removeFromRight(8);
    m_presetBox.setBounds(area.withSizeKeepingCentre(std::min(area.getWidth(), 200), 22));
}

void HeaderBar::buildPresetList()
{
    m_presetBox.clear(juce::dontSendNotification);
    m_presetFiles.clear();

    auto addGroup = [&](const juce::String& header, const juce::Array<juce::File>& files) {
        if (!files.isEmpty())
        {
            m_presetBox.addSectionHeading(header);
            for (const auto& f : files)
            {
                const int id = m_presetFiles.size() + 1;
                m_presetBox.addItem(f.getFileNameWithoutExtension(), id);
                m_presetFiles.add(f);
            }
        }
    };

    addGroup("Factory Presets", Presets::getFactoryPresets());
    addGroup("User Presets",    Presets::getUserPresets());
}

void HeaderBar::refreshPresetList()
{
    buildPresetList();
}

void HeaderBar::onPresetSelected(int comboId)
{
    const int idx = comboId - 1;
    if (idx >= 0 && idx < m_presetFiles.size() && onPresetLoad)
        onPresetLoad(m_presetFiles[idx]);
}

} // namespace AIT
