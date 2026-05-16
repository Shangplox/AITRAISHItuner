#pragma once
#include <JuceHeader.h>
#include "KnobControl.h"

namespace AIT {

// Sample Engine control panel: gain, pitch, pan, start, end, fade in/out, loop, one-shot, reverse.
class SamplePanel : public juce::Component
{
public:
    explicit SamplePanel(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KnobControl m_gain;
    KnobControl m_pitch;
    KnobControl m_pan;
    KnobControl m_start;
    KnobControl m_end;
    KnobControl m_fadeIn;
    KnobControl m_fadeOut;

    juce::ToggleButton m_loopBtn;
    juce::ToggleButton m_oneShotBtn;
    juce::ToggleButton m_reverseBtn;

    // APVTS attachments keep sliders/buttons in sync with parameter state.
    using SliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttach = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<SliderAttach>> m_sliderAttachments;
    std::vector<std::unique_ptr<ButtonAttach>> m_buttonAttachments;

    juce::Label m_titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplePanel)
};

} // namespace AIT
