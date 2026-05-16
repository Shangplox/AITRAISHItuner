#pragma once
#include <JuceHeader.h>
#include "State/Serialization.h"

namespace AIT {

// Preset management: factory presets, user preset load/save.
// All file I/O happens on the message thread — never call from audio thread.
class Presets
{
public:
    explicit Presets(juce::AudioProcessorValueTreeState& apvts);

    // Returns the directory where user presets are stored.
    // %AppData%/AITRAISHItuner/Presets/
    static juce::File getUserPresetDirectory();

    // Returns all factory preset files from assets/presets/factory/.
    static juce::Array<juce::File> getFactoryPresets();

    // Returns all user preset files.
    static juce::Array<juce::File> getUserPresets();

    // Save current APVTS state as a named user preset JSON file.
    // Returns true on success.
    bool saveUserPreset(const juce::String& name,
                        const juce::String& author,
                        const juce::String& category,
                        const juce::String& sampleFilePath);

    // Load a preset file and apply it to APVTS.
    // Returns the sample file path embedded in the preset, or empty string.
    juce::String loadPreset(const juce::File& presetFile);

    // Apply a PresetData to APVTS (each parameter value by ID).
    void applyPresetData(const PresetData& data);

private:
    juce::AudioProcessorValueTreeState& m_apvts;
};

} // namespace AIT
