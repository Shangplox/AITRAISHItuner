#pragma once
#include <JuceHeader.h>
#include "KnobControl.h"

namespace AIT {

// Distortion Engine control panel.
class DistortionPanel : public juce::Component
{
public:
    explicit DistortionPanel(juce::AudioProcessorValueTreeState& apvts);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KnobControl m_drive;
    KnobControl m_tone;
    KnobControl m_mix;
    KnobControl m_output;
    KnobControl m_inputGain;

    juce::ComboBox m_modeBox;
    juce::ComboBox m_oversampleBox;
    juce::Label    m_modeLabel;
    juce::Label    m_osLabel;
    juce::Label    m_titleLabel;

    using SliderAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttach  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::vector<std::unique_ptr<SliderAttach>> m_sliderAttachments;
    std::vector<std::unique_ptr<ComboAttach>>  m_comboAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionPanel)
};

} // namespace AIT
