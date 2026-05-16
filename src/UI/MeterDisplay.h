#pragma once
#include <JuceHeader.h>

namespace AIT {

class Metering;

// Two-channel peak + RMS meter display. Reads from Metering atomics on timer tick.
class MeterDisplay : public juce::Component, public juce::Timer
{
public:
    explicit MeterDisplay(Metering& metering);
    ~MeterDisplay() override;

    void paint(juce::Graphics&) override;
    void timerCallback() override;

private:
    Metering& m_metering;
    float m_peakL{-96.f}, m_peakR{-96.f};
    float m_rmsL{-96.f},  m_rmsR{-96.f};

    // Cached display peaks with hold/decay (message thread only — no atomics needed).
    float m_displayPeakL{-96.f}, m_displayPeakR{-96.f};
    static constexpr float DECAY_PER_FRAME = -3.f;  // dB per timer tick (~60ms)

    void drawChannel(juce::Graphics& g, juce::Rectangle<float> bounds,
                     float peakdB, float rmsdB) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterDisplay)
};

} // namespace AIT
