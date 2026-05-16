#include "SamplePanel.h"
#include "Theme.h"
#include "Plugin/Parameters.h"

namespace AIT {

SamplePanel::SamplePanel(juce::AudioProcessorValueTreeState& apvts)
    : m_gain    ("Gain",     "Sample gain in dB (-24 to +24)")
    , m_pitch   ("Pitch",    "Pitch offset in semitones (-24 to +24)")
    , m_pan     ("Pan",      "Sample stereo pan (-1=L, 0=C, +1=R)")
    , m_start   ("Start",    "Playback start position (0-1)")
    , m_end     ("End",      "Playback end position (0-1)")
    , m_fadeIn  ("Fade In",  "Fade-in duration in milliseconds (0-1000)")
    , m_fadeOut ("Fade Out", "Fade-out duration in milliseconds (0-1000)")
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
    addAndMakeVisible(m_fadeIn);
    addAndMakeVisible(m_fadeOut);

    m_loopBtn.setButtonText("LOOP");
    m_loopBtn.setTooltip("Toggle sample looping");
    addAndMakeVisible(m_loopBtn);

    m_oneShotBtn.setButtonText("1-SHOT");
    m_oneShotBtn.setTooltip("One-shot mode: ignore note-off, play full sample");
    m_oneShotBtn.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(m_oneShotBtn);

    m_reverseBtn.setButtonText("REV");
    m_reverseBtn.setTooltip("Reverse: play sample backwards within start/end bounds");
    addAndMakeVisible(m_reverseBtn);

    // Wire sliders to APVTS.
    namespace P = AIT::Params;
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_GAIN,     m_gain.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_PITCH,    m_pitch.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_PAN,      m_pan.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_START,    m_start.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_END,      m_end.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_FADE_IN,  m_fadeIn.getSlider()));
    m_sliderAttachments.push_back(std::make_unique<SliderAttach>(apvts, P::SAMPLE_FADE_OUT, m_fadeOut.getSlider()));

    m_buttonAttachments.push_back(std::make_unique<ButtonAttach>(apvts, P::SAMPLE_LOOP,    m_loopBtn));
    m_buttonAttachments.push_back(std::make_unique<ButtonAttach>(apvts, P::SAMPLE_ONESHOT, m_oneShotBtn));
    m_buttonAttachments.push_back(std::make_unique<ButtonAttach>(apvts, P::SAMPLE_REVERSE, m_reverseBtn));
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

    // Row 1: gain, pitch, pan (3 knobs).
    auto row1 = area.removeFromTop(Theme::KNOB_SIZE + 16);
    const int knobW3 = row1.getWidth() / 3;
    m_gain.setBounds(row1.removeFromLeft(knobW3));
    m_pitch.setBounds(row1.removeFromLeft(knobW3));
    m_pan.setBounds(row1);

    area.removeFromTop(Theme::PADDING);

    // Row 2: start, end, fade in, fade out (4 knobs).
    auto row2 = area.removeFromTop(Theme::KNOB_SIZE + 16);
    const int knobW4 = row2.getWidth() / 4;
    m_start.setBounds(row2.removeFromLeft(knobW4));
    m_end.setBounds(row2.removeFromLeft(knobW4));
    m_fadeIn.setBounds(row2.removeFromLeft(knobW4));
    m_fadeOut.setBounds(row2);

    area.removeFromTop(Theme::PADDING);

    // Toggle buttons row: LOOP | 1-SHOT | REV
    const int btnH = 22;
    const int btnW = area.getWidth() / 3 - 4;
    auto btnRow = area.removeFromTop(btnH);
    m_loopBtn.setBounds(btnRow.removeFromLeft(btnW));
    btnRow.removeFromLeft(4);
    m_oneShotBtn.setBounds(btnRow.removeFromLeft(btnW));
    btnRow.removeFromLeft(4);
    m_reverseBtn.setBounds(btnRow.removeFromLeft(btnW));
}

} // namespace AIT
