#pragma once
#include <JuceHeader.h>
#include "Serialization.h"

namespace AIT {

// Owns the getStateInformation / setStateInformation logic for PluginProcessor.
// Keeps PluginProcessor lean — all serialization detail lives here.
class StateManager
{
public:
    explicit StateManager(juce::AudioProcessorValueTreeState& apvts);

    // Called by PluginProcessor::getStateInformation.
    void getState(const juce::String& sampleFilePath, juce::MemoryBlock& dest) const;

    // Called by PluginProcessor::setStateInformation.
    // Returns the sample file path embedded in the state (so PluginProcessor can reload it).
    juce::String setState(const void* data, int size);

private:
    juce::AudioProcessorValueTreeState& m_apvts;
};

} // namespace AIT
