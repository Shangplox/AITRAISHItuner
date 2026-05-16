#include "SamplePanel.h"
#include "Theme.h"
#include "Plugin/Parameters.h"

namespace AIT {

SamplePanel::SamplePanel(juce::AudioProcessorValueTreeState& apvts)
    : m_gain    ("Gain",    "Sample gain in dB (-24 to +24)")
    , m_pitch   ("Pitch",   "Pitch offset in semitones (-24 to +24)")
    , m_pan     ("Pan",     "Sample stereo pan (-1=L, 0=C, +1=R)")
    , m_start   ("Start",   "Playback start position (0-1)")
    , m_end     ("End",     "Playback end position (0-1)")
{
    m_titleLabel.setText("SAMPLE", juce::dontSendNotification);
    m_titleLabel.setFont(juce::Font(Theme::FONT_SMALL, juce::Font::bold));
    m_titleLabel.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));
    m_titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(m_titleLabel);

    addAndMakeVisible(m_gain);
    addAndMakeVisible(m_pitch);
    addAndMakeVisible(m_pan);
    addAndMakeVisible(m_start);
    addAndMakeVisible(m_end);

    m_loopBtn.setButtonText("LOOP");
    m_loopBtn.setTooltip("Toggle sample looping");
    addAndMakeVisible(m_loopBtn);

    m_oneShotBtn.setButtonText("1-SHOT");
    m_oneShotBtn.setTooltip("One-shot mode: ignore note-off, play full sample");
    m_oneShotBtn.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(m_oneShotBtn);

    // Wire sliders to APVTS.
    namespace P = AIT::Params;
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_GAIN,  m_gain.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_PITCH, m_pitch.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_PAN,   m_pan.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_START, m_start.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_END,   m_end.getSlider()));

    m_buttonAttachments.push_back(std::make_unique<ButtonAttach>(apvts, P::SAMPLE_LOOP,    m_loopBtn));
    m_buttonAttachments.push_back(std::make_unique<ButtonAttach>(apvts, P::SAMPLE_ONESHOT, m_oneShotBtn));
}

void SamplePanel::paint(juce::Graphics& g)
{
    g.setColour(Theme::colour(Theme::BACKGROUND_PANEL));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::PANEL_RADIUS);
}

void SamplePanel::resized()
{
    auto area = getLocalBounds().reduced(Theme::PADDING);
    m_titleLabel.setBounds(area.removeFromTop(16));
    area.removeFromTop(4);

    // Top row: gain, pitch, pan (3 knobs).
    auto topRow = area.removeFromTop(Theme::KNOB_SIZE + 16);
    const int knobW = topRow.getWidth() / 3;
    m_gain.setBounds(topRow.removeFromLeft(knobW));
    m_pitch.setBounds(topRow.removeFromLeft(knobW));
    m_pan.setBounds(topRow);

    area.removeFromTop(Theme::PADDING);

    // Bottom row: start, end, then buttons.
    auto bottomRow = area.removeFromTop(Theme::KNOB_SIZE + 16);
    const int halfKnobW = bottomRow.getWidth() / 2;
    m_start.setBounds(bottomRow.removeFromLeft(halfKnobW));
    m_end.setBounds(bottomRow.removeFromLeft(halfKnobW));

    area.removeFromTop(Theme::PADDING);

    // Toggle buttons in a row.
    const int btnH = 22;
    const int btnW = area.getWidth() / 2 - 4;
    auto btnRow = area.removeFromTop(btnH);
    m_loopBtn.setBounds(btnRow.removeFromLeft(btnW));
    btnRow.removeFromLeft(8);
    m_oneShotBtn.setBounds(btnRow.removeFromLeft(btnW));
}

} // namespace AIT
