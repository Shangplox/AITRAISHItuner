#pragma once
#include <JuceHeader.h>

namespace AIT {

class SampleEngine;

// Read-only waveform display. Also serves as the drag-and-drop target for WAV files.
// Waveform is drawn directly from SampleEngine's active buffer (message thread safe
// because we only call getDisplayBuffer() on the message thread).
class WaveformDisplay : public juce::Component,
                        public juce::FileDragAndDropTarget,
                        public juce::Timer
{
public:
    // Callback fired when the user drops or opens a file.
    std::function<void(const juce::File&)> onFileDropped;

    explicit WaveformDisplay(SampleEngine& engine);
    ~WaveformDisplay() override;

    // Rebuilds the waveform thumbnail from the current sample buffer.
    // Call from message thread after a new sample is loaded.
    void refreshWaveform();

    void paint(juce::Graphics&) override;
    void resized() override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void fileDragEnter(const juce::StringArray&, int, int) override;
    void fileDragExit(const juce::StringArray&) override;

    // Timer: periodically refreshes waveform after load.
    void timerCallback() override;

private:
    SampleEngine& m_engine;
    juce::Path    m_waveformPath;
    bool          m_isDragOver{false};
    bool          m_needsRefresh{false};

    void buildWaveformPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};

} // namespace AIT
