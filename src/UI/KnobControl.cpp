#include "KnobControl.h"
#include "Theme.h"

namespace AIT {

KnobControl::KnobControl(const juce::String& labelText, const juce::String& tooltip)
{
    m_slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    m_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, Theme::KNOB_SIZE, 16);
    m_slider.setPopupDisplayEnabled(true, true, nullptr);
    if (tooltip.isNotEmpty())
        m_slider.setTooltip(tooltip);

    // Double-click resets to default.
    m_slider.setDoubleClickReturnValue(true, m_slider.getDoubleClickReturnValue());

    m_label.setText(labelText, juce::dontSendNotification);
    m_label.setJustificationType(juce::Justification::centred);
    m_label.setFont(juce::Font(Theme::FONT_LABEL));
    m_label.setColour(juce::Label::textColourId, Theme::colour(Theme::TEXT_SECONDARY));

    addAndMakeVisible(m_slider);
    addAndMakeVisible(m_label);
}

void KnobControl::resized()
{
    auto area = getLocalBounds();
    const int labelH = 14;
    m_label.setBounds(area.removeFromBottom(labelH));
    m_slider.setBounds(area);
}

void KnobControl::paint(juce::Graphics& /*g*/)
{
    // Background is transparent — parent panel provides the background.
}

} // namespace AIT
