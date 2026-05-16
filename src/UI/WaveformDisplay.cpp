#include "WaveformDisplay.h"
#include "Theme.h"
#include "DSP/SampleEngine.h"

namespace AIT {

WaveformDisplay::WaveformDisplay(SampleEngine& engine)
    : m_engine(engine)
{
    startTimerHz(10);  // check for new sample data 10x per second
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::timerCallback()
{
    if (m_needsRefresh)
    {
        buildWaveformPath();
        m_needsRefresh = false;
        repaint();
    }
}

void WaveformDisplay::refreshWaveform()
{
    m_needsRefresh = true;
}

void WaveformDisplay::buildWaveformPath()
{
    m_waveformPath.clear();

    if (!m_engine.hasSample())
        return;

    const auto& buffer = m_engine.getDisplayBuffer();
    const int numSamples = buffer.getNumSamples();
    if (numSamples < 2)
        return;

    const auto bounds = getLocalBounds().toFloat().reduced(2.f);
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    const float midY = bounds.getCentreY();

    // Downsample to display pixels — one pixel = many samples averaged to peak.
    const int pixelWidth = std::max(1, static_cast<int>(w));
    const float samplesPerPixel = numSamples / static_cast<float>(pixelWidth);

    // Use first channel (mono or L of stereo).
    const auto* data = buffer.getReadPointer(0);

    m_waveformPath.startNewSubPath(bounds.getX(), midY);

    for (int px = 0; px < pixelWidth; ++px)
    {
        const int startS = static_cast<int>(px * samplesPerPixel);
        const int endS   = std::min(numSamples, static_cast<int>((px + 1) * samplesPerPixel));

        float peak = 0.f;
        for (int s = startS; s < endS; ++s)
            peak = std::max(peak, std::abs(data[s]));

        const float x = bounds.getX() + px;
        const float y = midY - peak * (h * 0.5f);
        m_waveformPath.lineTo(x, y);
    }

    // Mirror bottom half.
    juce::Path mirrorPath;
    for (int px = pixelWidth - 1; px >= 0; --px)
    {
        const int startS = static_cast<int>(px * samplesPerPixel);
        const int endS   = std::min(numSamples, static_cast<int>((px + 1) * samplesPerPixel));

        float peak = 0.f;
        for (int s = startS; s < endS; ++s)
            peak = std::max(peak, std::abs(data[s]));

        const float x = bounds.getX() + px;
        const float y = midY + peak * (h * 0.5f);
        m_waveformPath.lineTo(x, y);
    }

    m_waveformPath.closeSubPath();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // Background.
    g.setColour(Theme::colour(Theme::BACKGROUND_PANEL));
    g.fillRoundedRectangle(bounds, Theme::PANEL_RADIUS);

    if (m_isDragOver)
    {
        g.setColour(Theme::colour(Theme::ACCENT_PRIMARY).withAlpha(0.2f));
        g.fillRoundedRectangle(bounds, Theme::PANEL_RADIUS);
        g.setColour(Theme::colour(Theme::ACCENT_PRIMARY));
        g.drawRoundedRectangle(bounds.reduced(1.f), Theme::PANEL_RADIUS, 2.f);
    }

    if (!m_engine.hasSample())
    {
        // Placeholder text.
        g.setColour(Theme::colour(Theme::TEXT_DISABLED));
        g.setFont(juce::Font(Theme::FONT_NORMAL));
        g.drawText("Drag & drop a WAV file here, or click to open",
                   getLocalBounds(), juce::Justification::centred, true);
        return;
    }

    // Waveform fill.
    g.setColour(Theme::colour(Theme::WAVEFORM_FILL));
    g.fillPath(m_waveformPath);

    // Waveform outline.
    g.setColour(Theme::colour(Theme::WAVEFORM_LINE));
    g.strokePath(m_waveformPath, juce::PathStrokeType(1.f));

    // Border.
    g.setColour(Theme::colour(Theme::SEPARATOR));
    g.drawRoundedRectangle(bounds.reduced(0.5f), Theme::PANEL_RADIUS, 1.f);
}

void WaveformDisplay::resized()
{
    buildWaveformPath();
}

bool WaveformDisplay::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& f : files)
    {
        const auto ext = juce::File(f).getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".aif" || ext == ".aiff" || ext == ".flac" || ext == ".mp3")
            return true;
    }
    return false;
}

void WaveformDisplay::filesDropped(const juce::StringArray& files, int, int)
{
    m_isDragOver = false;
    for (const auto& f : files)
    {
        const juce::File file(f);
        if (file.existsAsFile() && onFileDropped)
        {
            onFileDropped(file);
            break;  // only load first valid file
        }
    }
    repaint();
}

void WaveformDisplay::fileDragEnter(const juce::StringArray&, int, int)
{
    m_isDragOver = true;
    repaint();
}

void WaveformDisplay::fileDragExit(const juce::StringArray&)
{
    m_isDragOver = false;
    repaint();
}

} // namespace AIT
