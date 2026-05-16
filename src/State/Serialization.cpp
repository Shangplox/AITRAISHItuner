#include "Serialization.h"
#include "Utils/Logging.h"

namespace AIT::Serialization {

static const juce::Identifier TAG_ROOT        ("AITRAISHItunerState");
static const juce::Identifier TAG_SAMPLE_FILE ("SampleFile");
static const juce::Identifier TAG_PRESET_META ("PresetMeta");
static const juce::Identifier ATTR_PATH       ("path");
static const juce::Identifier ATTR_NAME       ("name");
static const juce::Identifier ATTR_AUTHOR     ("author");
static const juce::Identifier ATTR_CATEGORY   ("category");
static const juce::Identifier ATTR_VERSION    ("version");

// ── DAW project state (binary XML, used by getStateInformation) ───────────────

void stateToMemoryBlock(const PresetData& data, juce::MemoryBlock& dest)
{
    juce::ValueTree root = data.apvtsState.createCopy();

    // Attach sample file path as a child element.
    juce::ValueTree sampleNode(TAG_SAMPLE_FILE);
    sampleNode.setProperty(ATTR_PATH, data.sampleFilePath, nullptr);
    root.appendChild(sampleNode, nullptr);

    // Attach preset metadata.
    juce::ValueTree metaNode(TAG_PRESET_META);
    metaNode.setProperty(ATTR_NAME,     data.presetName,     nullptr);
    metaNode.setProperty(ATTR_AUTHOR,   data.presetAuthor,   nullptr);
    metaNode.setProperty(ATTR_CATEGORY, data.presetCategory, nullptr);
    metaNode.setProperty(ATTR_VERSION,  data.presetVersion,  nullptr);
    root.appendChild(metaNode, nullptr);

    std::unique_ptr<juce::XmlElement> xml(root.createXml());
    if (xml)
        juce::AudioProcessor::copyXmlToBinary(*xml, dest);
}

bool memoryBlockToState(const void* data, int size, PresetData& out)
{
    std::unique_ptr<juce::XmlElement> xml(juce::AudioProcessor::getXmlFromBinary(data, size));
    if (!xml)
        return false;

    out.apvtsState = juce::ValueTree::fromXml(*xml);
    if (!out.apvtsState.isValid())
        return false;

    // Extract sample file path.
    auto sampleNode = out.apvtsState.getChildWithName(TAG_SAMPLE_FILE);
    if (sampleNode.isValid())
        out.sampleFilePath = sampleNode.getProperty(ATTR_PATH).toString();

    // Extract preset metadata.
    auto metaNode = out.apvtsState.getChildWithName(TAG_PRESET_META);
    if (metaNode.isValid())
    {
        out.presetName     = metaNode.getProperty(ATTR_NAME).toString();
        out.presetAuthor   = metaNode.getProperty(ATTR_AUTHOR).toString();
        out.presetCategory = metaNode.getProperty(ATTR_CATEGORY).toString();
        out.presetVersion  = metaNode.getProperty(ATTR_VERSION).toString();
    }

    return true;
}

// ── JSON preset files (user-visible, shareable) ───────────────────────────────

bool savePresetToFile(const PresetData& data, const juce::File& dest)
{
    // Build a JSON object from the APVTS state tree.
    auto paramsObj = std::make_unique<juce::DynamicObject>();

    // Walk the APVTS state tree and extract parameter values.
    for (int i = 0; i < data.apvtsState.getNumProperties(); ++i)
    {
        auto name = data.apvtsState.getPropertyName(i).toString();
        paramsObj->setProperty(name, data.apvtsState.getProperty(data.apvtsState.getPropertyName(i)));
    }

    auto root = std::make_unique<juce::DynamicObject>();
    root->setProperty("name",       data.presetName);
    root->setProperty("author",     data.presetAuthor);
    root->setProperty("category",   data.presetCategory);
    root->setProperty("version",    data.presetVersion);
    root->setProperty("parameters", juce::var(paramsObj.release()));
    root->setProperty("sampleFile", data.sampleFilePath);

    const juce::String json = juce::JSON::toString(juce::var(root.release()), true);

    if (!dest.getParentDirectory().createDirectory())
    {
        AIT_LOG_ERR("Cannot create preset directory: " << dest.getParentDirectory().getFullPathName());
        return false;
    }

    return dest.replaceWithText(json);
}

bool loadPresetFromFile(const juce::File& src, PresetData& out)
{
    if (!src.existsAsFile())
        return false;

    juce::var json;
    auto result = juce::JSON::parse(src.loadFileAsString(), json);
    if (result.failed() || !json.isObject())
    {
        AIT_LOG_ERR("JSON parse failed: " << src.getFullPathName());
        return false;
    }

    out.presetName     = json.getProperty("name",     {}).toString();
    out.presetAuthor   = json.getProperty("author",   {}).toString();
    out.presetCategory = json.getProperty("category", {}).toString();
    out.presetVersion  = json.getProperty("version",  {}).toString();
    out.sampleFilePath = json.getProperty("sampleFile", {}).toString();

    // Parameters are stored in the preset JSON; caller merges them into APVTS.
    // The apvtsState in PresetData is not populated by this function —
    // the caller (Presets.cpp) applies individual parameter values.
    const auto* paramsObj = json.getProperty("parameters", {}).getDynamicObject();
    if (paramsObj)
    {
        out.apvtsState = juce::ValueTree("Parameters");
        for (const auto& prop : paramsObj->getProperties())
            out.apvtsState.setProperty(prop.name, prop.value, nullptr);
    }

    return true;
}

} // namespace AIT::Serialization
