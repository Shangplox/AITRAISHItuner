#include "TransportBar.h"
#include "Theme.h"

namespace AIT {

TransportBar::TransportBar()
{
    m_playBtn.setButtonText("Play");
    m_playBtn.setTooltip("Preview the loaded sample (sends C3 note-on)");
    m_playBtn.onClick = [this] { if (onPlay) onPlay(); };
    addAndMakeVisible(m_playBtn);

    m_stopBtn.setButtonText("Stop");
    m_stopBtn.setTooltip("Stop preview playback");
    m_stopBtn.onClick = [this] { if (onStop) onStop(); };
    addAndMakeVisible(m_stopBtn);
}

void TransportBar::paint(juce::Graphics& g)
{
    // Subtle background — slightly different from the main background.
    g.setColour(Theme::colour(Theme::BACKGROUND_MID).withAlpha(0.5f));
    g.fillRect(getLocalBounds());
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced(Theme::PADDING, 2);

    const int btnW = 60;
    const int btnH = area.getHeight();

    m_playBtn.setBounds(area.removeFromLeft(btnW).withHeight(btnH));
    area.removeFromLeft(4);
    m_stopBtn.setBounds(area.removeFromLeft(btnW).withHeight(btnH));
}

void TransportBar::setPlaying(bool playing)
{
    m_playBtn.setEnabled(!playing);
    m_stopBtn.setEnabled(playing);
}

} // namespace AIT
