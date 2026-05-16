#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <functional>

namespace AIT {

class SampleEngine;

// Loads WAV (and other JUCE-supported formats) on a dedicated background thread.
// Delivers the decoded buffer to SampleEngine via message-thread callback.
// Never touches the audio thread.
class SampleLoader : public juce::Thread
{
public:
    explicit SampleLoader(SampleEngine& engine);
    ~SampleLoader() override;

    // Queue a file for loading. If a load is already in progress,
    // the new file replaces the pending one — only the latest request is served.
    void loadFile(const juce::File& file);

    // Returns the path of the last successfully loaded file (message thread).
    juce::String getLoadedFilePath() const;

    // Error string from the last failed load attempt (empty on success).
    juce::String getLastError() const;

private:
    void run() override;
    void doLoad(const juce::File& file);

    SampleEngine& m_engine;

    juce::CriticalSection  m_pendingLock;
    juce::File             m_pendingFile;
    bool                   m_hasPendingFile{false};

    juce::AudioFormatManager m_formatManager;

    // Set on message thread after successful load.
    juce::String m_loadedFilePath;
    juce::String m_lastError;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleLoader)
};

} // namespace AIT
