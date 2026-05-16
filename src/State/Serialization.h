#pragma once
#include <JuceHeader.h>

namespace AIT {

// Helpers for reading and writing preset data.
// Preset format: XML (via APVTS) with an extra <SampleFile path="..."/> child element.

struct PresetData
{
    juce::ValueTree apvtsState;      // full APVTS state tree
    juce::String    sampleFilePath;  // absolute path to the loaded WAV
    juce::String    presetName;
    juce::String    presetAuthor;
    juce::String    presetCategory;
    juce::String    presetVersion;
};

namespace Serialization {

// Serialize APVTS state + sample path to a MemoryBlock (for getStateInformation).
void stateToMemoryBlock(const PresetData& data, juce::MemoryBlock& dest);

// Deserialize from a MemoryBlock (for setStateInformation). Returns false on failure.
bool memoryBlockToState(const void* data, int size, PresetData& out);

// Write a preset to a JSON file (for the user preset system).
bool savePresetToFile(const PresetData& data, const juce::File& dest);

// Load a preset from a JSON file. Returns false on failure.
bool loadPresetFromFile(const juce::File& src, PresetData& out);

} // namespace Serialization
} // namespace AIT
