#include "DistortionPanel.h"
#include "Theme.h"
#include "Plugin/Parameters.h"

namespace AIT {

DistortionPanel::DistortionPanel(juce::AudioProcessorValueTreeState& apvts)
    : m_drive    ("Drive",   "Distortion drive amount (0=clean, 1=heavy)")
    , m_tone     ("Tone",    "Post-distortion tone (LP cutoff: 0=dark, 1=bright)")
    , m_mix      ("Mix",     "Wet/dry mix (0=dry, 1=100% wet)")
    , m_output   ("Output",  "Post-distortion output level in dB")
    , m_inputGain("Input",   "Input gain boost before distortion (0 to +24 dB)")
{
    m_titleLabel.setText("DISTORTION", juce::dontSendNotification);
    m_titleLabel.setFont(juce::Font(Theme::FONT_SMALL, juce::Font::bold));
    m_titleLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));
    m_titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_titleLabel);

    addAndMakeVisible(m_drive);
    addAndMakeVisible(m_tone);
    addAndMakeVisible(m_mix);
    addAndMakeVisible(m_output);
    addAndMakeVisible(m_inputGain);

    // Mode combo box.
    // MVP: only SoftClip is functional. Other labels show future modes.
    m_modeBox.addItem("SoftClip",  1);
    m_modeBox.addItem("Tube",      2);
    m_modeBox.addItem("Diode",     3);
    m_modeBox.addItem("Tape",      4);
    m_modeBox.addItem("Bitcrush",  5);
    m_modeBox.addItem("Wavefold",  6);
    m_modeBox.setTooltip("Distortion mode (modes 2-6 are Phase 2)");
    m_modeLabel.setText("Mode", juce::dontSendNotification);
    m_modeLabel.setFont(juce::Font(Theme::FONT_LABEL));
    m_modeLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));
    addAndMakeVisible(m_modeBox);
    addAndMakeVisible(m_modeLabel);

    // Oversample combo box.
    m_oversampleBox.addItem("1x",  1);
    m_oversampleBox.addItem("4x",  2);
    m_oversampleBox.setTooltip("Oversampling factor. Changing this updates PDC latency.");
    m_osLabel.setText("OS", juce::dontSendNotification);
    m_osLabel.setFont(juce::Font(Theme::FONT_LABEL));
    m_osLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));
    addAndMakeVisible(m_oversampleBox);
    addAndMakeVisible(m_osLabel);

    // Wire knobs.
    namespace P = AIT::Params;
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::DIST_DRIVE,      m_drive.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::DIST_TONE,       m_tone.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::DIST_MIX,        m_mix.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::DIST_OUTPUT,     m_output.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::DIST_INPUT_GAIN, m_inputGain.getSlider()));

    m_comboAttachments.push_back(std::make_unique<ComboAttach>(apvts, P::DIST_MODE,       m_modeBox));
    m_comboAttachments.push_back(std::make_unique<ComboAttach>(apvts, P::DIST_OVERSAMPLE, m_oversampleBox));
}

void DistortionPanel::paint(juce::Graphics& g)
{
    g.setColour(Theme::colour(Theme::BACKGROUND_PANEL));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::PANEL_RADIUS);
}

void DistortionPanel::resized()
{
    auto area = getLocalBounds().reduced(Theme::PADDING);
    m_titleLabel.setBounds(area.removeFromTop(16));
    area.removeFromTop(4);

    // Top row: drive, tone, mix, output, input (5 knobs — squish them).
    auto knobRow = area.removeFromTop(Theme::KNOB_SIZE + 16);
    const int knobW = knobRow.getWidth() / 5;
    m_drive.setBounds(knobRow.removeFromLeft(knobW));
    m_tone.setBounds(knobRow.removeFromLeft(knobW));
    m_mix.setBounds(knobRow.removeFromLeft(knobW));
    m_output.setBounds(knobRow.removeFromLeft(knobW));
    m_inputGain.setBounds(knobRow);

    area.removeFromTop(Theme::PADDING);

    // Mode + oversample dropdowns.
    auto comboRow = area.removeFromTop(22);
    const int labelW = 34;
    const int comboW = (comboRow.getWidth() / 2) - labelW - 4;

    m_modeLabel.setBounds(comboRow.removeFromLeft(labelW));
    m_modeBox.setBounds(comboRow.removeFromLeft(comboW));
    comboRow.removeFromLeft(8);
    m_osLabel.setBounds(comboRow.removeFromLeft(labelW));
    m_oversampleBox.setBounds(comboRow.removeFromLeft(comboW));
}

} // namespace AIT
