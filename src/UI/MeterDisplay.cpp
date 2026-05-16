#include "MeterDisplay.h"
#include "Theme.h"
#include "DSP/Metering.h"

namespace AIT {

MeterDisplay::MeterDisplay(Metering& metering)
    : m_metering(metering)
{
    startTimerHz(15);
}

MeterDisplay::~MeterDisplay()
{
    stopTimer();
}

void MeterDisplay::timerCallback()
{
    // Read from atomics (safe — Metering guarantees lock-free reads).
    m_peakL = m_metering.getPeakLdB();
    m_peakR = m_metering.getPeakRdB();
    m_rmsL  = m_metering.getRmsLdB();
    m_rmsR  = m_metering.getRmsRdB();

    // Apply display peak hold + decay.
    m_displayPeakL = std::max(m_peakL, m_displayPeakL + DECAY_PER_FRAME);
    m_displayPeakR = std::max(m_peakR, m_displayPeakR + DECAY_PER_FRAME);

    repaint();
}

void MeterDisplay::paint(juce::Graphics& g)
{
    g.setColour(Theme::colour(Theme::BACKGROUND_PANEL));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), Theme::PANEL_RADIUS);

    const auto area = getLocalBounds().reduced(3).toFloat();
    const float halfW = area.getWidth() * 0.5f;

    // Left channel.
    drawChannel(g, area.withWidth(halfW - 1.f), m_peakL, m_rmsL);

    // Right channel.
    drawChannel(g, area.withX(area.getX() + halfW + 1.f).withWidth(halfW - 1.f), m_peakR, m_rmsR);

    // Label.
    g.setColour(Theme::colour(Theme::TEXT_SECONDARY));
    g.setFont(juce::Font(Theme::FONT_SMALL));
    g.drawText("L", area.withWidth(halfW - 1.f).withHeight(12.f), juce::Justification::centred);
    g.drawText("R", area.withX(area.getX() + halfW + 1.f).withWidth(halfW - 1.f).withHeight(12.f),
               juce::Justification::centred);
}

void MeterDisplay::drawChannel(juce::Graphics& g, juce::Rectangle<float> bounds,
                                float peakdB, float rmsdB) const
{
    const float minDB = -60.f;
    const float maxDB = 0.f;

    auto dbToFraction = [&](float dB) -> float {
        return juce::jlimit(0.f, 1.f, (dB - minDB) / (maxDB - minDB));
    };

    const float h = bounds.getHeight() - 14.f;  // leave room for label
    const float y = bounds.getY() + 14.f;
    const float x = bounds.getX();
    const float w = bounds.getWidth();

    // Background track.
    g.setColour(Theme::colour(Theme::BACKGROUND_CTRL));
    g.fillRect(x, y, w, h);

    // RMS bar.
    const float rmsFrac   = dbToFraction(rmsdB);
    const float rmsHeight = rmsFrac * h;
    const float rmsY      = y + h - rmsHeight;
    g.setColour(Theme::colour(Theme::METER_LOW).withAlpha(0.6f));
    g.fillRect(x, rmsY, w, rmsHeight);

    // Peak bar.
    const float peakFrac   = dbToFraction(peakdB);
    const float peakHeight = peakFrac * h;
    const float peakY      = y + h - peakHeight;
    const auto meterColour = peakdB > -6.f
        ? (peakdB > -0.1f ? Theme::colour(Theme::METER_CLIP) : Theme::colour(Theme::METER_MID))
        : Theme::colour(Theme::METER_LOW);
    g.setColour(meterColour);
    g.fillRect(x, peakY, w, 2.f);  // peak hold indicator line
}

} // namespace AIT
