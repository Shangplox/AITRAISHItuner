#include "OutputPanel.h"
#include "Theme.h"
#include "Plugin/Parameters.h"

namespace AIT {

OutputPanel::OutputPanel(juce::AudioProcessorValueTreeState& apvts)
    : m_gain("Out Gain", "Master output gain (-24 to +6 dB)")
    , m_pan ("Out Pan",  "Master output pan (-1=L, 0=C, +1=R)")
{
    m_titleLabel.setText("OUTPUT", juce::dontSendNotification);
    m_titleLabel.setFont(juce::Font(Theme::FONT_SMALL, juce::Font::bold));
    m_titleLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));
    m_titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_titleLabel);
    addAndMakeVisible(m_gain);
    addAndMakeVisible(m_pan);

    namespace P = AIT::Params;
    m_attachments.push_back(std::make_unique<SliderAttach>(apvts, P::OUTPUT_GAIN, m_gain.getSlider()));
    m_attachments.push_back(std::make_unique<SliderAttach>(apvts, P::OUTPUT_PAN,  m_pan.getSlider()));
}

void OutputPanel::paint(juce::Graphics& g)
{
    g.setColour(Theme::colour(Theme::BACKGROUND_PANEL));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::PANEL_RADIUS);
}

void OutputPanel::resized()
{
    auto area = getLocalBounds().reduced(Theme::PADDING);
    m_titleLabel.setBounds(area.removeFromTop(16));
    area.removeFromTop(4);

    const int knobW = area.getWidth() / 2;
    auto row = area.removeFromTop(Theme::KNOB_SIZE + 16);
    m_gain.setBounds(row.removeFromLeft(knobW));
    m_pan.setBounds(row);
}

} // namespace AIT
