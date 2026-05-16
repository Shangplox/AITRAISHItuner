#pragma once
#include <JuceHeader.h>

namespace AIT {

// Small transport bar with Play/Stop buttons for in-plugin sample audition.
// Placed between the waveform display and control panels.
// Does NOT add new APVTS parameters — preview is a transient UI action.
class TransportBar : public juce::Component
{
public:
    // Callbacks wired by PluginEditor to PluginProcessor.
    std::function<void()> onPlay;
    std::function<void()> onStop;

    TransportBar();

    void paint(juce::Graphics&) override;
    void resized() override;

    void setPlaying(bool playing);

private:
    juce::TextButton m_playBtn;
    juce::TextButton m_stopBtn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportBar)
};

} // namespace AIT
