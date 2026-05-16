#include "StateManager.h"
#include "Utils/Logging.h"

namespace AIT {

StateManager::StateManager(juce::AudioProcessorValueTreeState& apvts)
    : m_apvts(apvts)
{}

void StateManager::getState(const juce::String& sampleFilePath, juce::MemoryBlock& dest) const
{
    PresetData data;
    data.apvtsState    = m_apvts.copyState();
    data.sampleFilePath = sampleFilePath;
    data.presetName    = "DAW State";
    data.presetAuthor  = {};
    data.presetCategory = "Session";
    data.presetVersion  = "0.1.0";

    Serialization::stateToMemoryBlock(data, dest);
}

juce::String StateManager::setState(const void* data, int size)
{
    PresetData loaded;
    if (!Serialization::memoryBlockToState(data, size, loaded))
    {
        AIT_LOG_ERR("setState: failed to parse binary state");
        return {};
    }

    // Restore APVTS parameters. Strip our extra child nodes first so APVTS
    // doesn't see unknown child trees and silently discard the restore.
    auto cleanState = loaded.apvtsState.createCopy();
    cleanState.removeChild(cleanState.getChildWithName("SampleFile"), nullptr);
    cleanState.removeChild(cleanState.getChildWithName("PresetMeta"), nullptr);

    m_apvts.replaceState(cleanState);
    return loaded.sampleFilePath;
}

} // namespace AIT
