#pragma once
#include <JuceHeader.h>
#include "KnobControl.h"

namespace AIT {

// Output gain and pan controls.
class OutputPanel : public juce::Component
{
public:
    explicit OutputPanel(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KnobControl m_gain;
    KnobControl m_pan;
    juce::Label m_titleLabel;

    using SliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::vector<std::unique_ptr<SliderAttach>> m_attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputPanel)
};

} // namespace AIT
