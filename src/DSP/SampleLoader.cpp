#include "SampleLoader.h"
#include "SampleEngine.h"
#include "Utils/Logging.h"

namespace AIT {

SampleLoader::SampleLoader(SampleEngine& engine)
    : juce::Thread("AIT::SampleLoader")
    , m_engine(engine)
{
    m_formatManager.registerBasicFormats();
    startThread(juce::Thread::Priority::low);
}

SampleLoader::~SampleLoader()
{
    stopThread(2000);
}

void SampleLoader::loadFile(const juce::File& file)
{
    {
        juce::ScopedLock lock(m_pendingLock);
        m_pendingFile      = file;
        m_hasPendingFile   = true;
    }
    notify();
}

juce::String SampleLoader::getLoadedFilePath() const
{
    juce::ScopedLock lock(m_pendingLock);
    return m_loadedFilePath;
}

juce::String SampleLoader::getLastError() const
{
    juce::ScopedLock lock(m_pendingLock);
    return m_lastError;
}

void SampleLoader::run()
{
    while (!threadShouldExit())
    {
        wait(-1);  // sleep until notified by loadFile()

        juce::File fileToLoad;
        {
            juce::ScopedLock lock(m_pendingLock);
            if (!m_hasPendingFile)
                continue;
            fileToLoad       = m_pendingFile;
            m_hasPendingFile = false;
        }

        if (fileToLoad.existsAsFile())
            doLoad(fileToLoad);
    }
}

void SampleLoader::doLoad(const juce::File& file)
{
    AIT_LOG("Loading: " << file.getFullPathName());

    std::unique_ptr<juce::AudioFormatReader> reader(m_formatManager.createReaderFor(file));
    if (!reader)
    {
        const auto err = "Cannot read: " + file.getFullPathName();
        AIT_LOG_ERR(err);
        juce::ScopedLock lock(m_pendingLock);
        m_lastError = err;
        return;
    }

    const int    numChannels = static_cast<int>(reader->numChannels);
    const int    numSamples  = static_cast<int>(reader->lengthInSamples);
    const double sourceRate  = reader->sampleRate;

    if (numSamples <= 0 || numChannels <= 0)
    {
        juce::ScopedLock lock(m_pendingLock);
        m_lastError = "Empty or unreadable audio file";
        return;
    }

    // Decode into a buffer on this background thread — no audio thread involvement.
    juce::AudioBuffer<float> buffer(numChannels, numSamples);
    reader->read(&buffer, 0, numSamples, 0, true, true);

    const juce::String path = file.getFullPathName();

    // Hand off to SampleEngine on the message thread.
    // SampleEngine outlives any pending async messages (it's owned by PluginProcessor).
    auto* enginePtr = &m_engine;
    juce::MessageManager::callAsync([enginePtr, buf = std::move(buffer), sourceRate, path, this]() mutable {
        enginePtr->commitPendingBuffer(std::move(buf), sourceRate);
        {
            juce::ScopedLock lock(m_pendingLock);
            m_loadedFilePath = path;
            m_lastError      = {};
        }
        AIT_LOG("Loaded: " << path);
    });
}

} // namespace AIT
