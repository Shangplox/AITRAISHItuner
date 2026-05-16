#include "Presets.h"
#include "Parameters.h"
#include "Utils/Logging.h"

namespace AIT {

Presets::Presets(juce::AudioProcessorValueTreeState& apvts)
    : m_apvts(apvts)
{}

juce::File Presets::getUserPresetDirectory()
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                .getChildFile("AITRAISHItuner")
                .getChildFile("Presets");
}

juce::Array<juce::File> Presets::getFactoryPresets()
{
    // In a shipped plugin, factory presets live next to the .vst3 bundle.
    // For development, look relative to the working directory.
    juce::Array<juce::File> results;

    juce::File factoryDir = juce::File::getCurrentWorkingDirectory()
                                .getChildFile("assets")
                                .getChildFile("presets")
                                .getChildFile("factory");

    if (factoryDir.isDirectory())
        factoryDir.findChildFiles(results, juce::File::findFiles, false, "*.json");

    return results;
}

juce::Array<juce::File> Presets::getUserPresets()
{
    juce::Array<juce::File> results;
    auto dir = getUserPresetDirectory();
    if (dir.isDirectory())
        dir.findChildFiles(results, juce::File::findFiles, false, "*.json");
    return results;
}

bool Presets::saveUserPreset(const juce::String& name,
                             const juce::String& author,
                             const juce::String& category,
                             const juce::String& sampleFilePath)
{
    PresetData data;
    data.apvtsState     = m_apvts.copyState();
    data.sampleFilePath = sampleFilePath;
    data.presetName     = name;
    data.presetAuthor   = author.isEmpty() ? "User" : author;
    data.presetCategory = category.isEmpty() ? "User" : category;
    data.presetVersion  = "0.1.0";

    const auto safeFilename = name.retainCharacters("abcdefghijklmnopqrstuvwxyz"
                                                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                     "0123456789 _-") + ".json";
    const auto destFile = getUserPresetDirectory().getChildFile(safeFilename);

    return Serialization::savePresetToFile(data, destFile);
}

juce::String Presets::loadPreset(const juce::File& presetFile)
{
    PresetData data;
    if (!Serialization::loadPresetFromFile(presetFile, data))
    {
        AIT_LOG_ERR("Failed to load preset: " << presetFile.getFullPathName());
        return {};
    }

    applyPresetData(data);
    return data.sampleFilePath;
}

void Presets::applyPresetData(const PresetData& data)
{
    // Apply each parameter value from the preset to APVTS.
    // Only parameters that exist in the current layout are set — unknown IDs are ignored.
    // This is the safe migration path: old presets with missing params use defaults.
    for (int i = 0; i < data.apvtsState.getNumProperties(); ++i)
    {
        const auto propName = data.apvtsState.getPropertyName(i).toString();
        auto* param = m_apvts.getParameter(propName);
        if (param)
        {
            const float value = static_cast<float>(data.apvtsState.getProperty(
                data.apvtsState.getPropertyName(i)));
            param->setValueNotifyingHost(param->convertTo0to1(value));
        }
    }
}

} // namespace AIT
