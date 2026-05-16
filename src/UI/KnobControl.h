#pragma once
#include <JuceHeader.h>

namespace AIT {

// A rotary slider with a label below it and a tooltip.
// Attach to APVTS using juce::AudioProcessorValueTreeState::SliderAttachment.
class KnobControl : public juce::Component
{
public:
    KnobControl(const juce::String& labelText, const juce::String& tooltip = {});

    juce::Slider& getSlider() { return m_slider; }

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::Slider m_slider;
    juce::Label  m_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobControl)
};

} // namespace AIT
