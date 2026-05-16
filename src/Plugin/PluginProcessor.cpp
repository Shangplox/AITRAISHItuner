#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils/AudioBufferUtils.h"

// ── Constructor / Destructor ───────────────────────────────────────────────────

AITRAISHItunerProcessor::AITRAISHItunerProcessor()
    : AudioProcessor(BusesProperties()
          .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , m_apvts(*this, nullptr, "AITRAISHItunerState", AIT::createParameterLayout())
    , m_sampleLoader(m_sampleEngine)
    , m_stateManager(m_apvts)
    , m_presets(m_apvts)
{
    // Never do significant work in the constructor — defer to prepareToPlay.
    // FL Studio will blacklist plugins that crash or hang during construction.
    cacheParameterPointers();
}

AITRAISHItunerProcessor::~AITRAISHItunerProcessor() = default;

// ── prepareToPlay ──────────────────────────────────────────────────────────────

void AITRAISHItunerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_sampleEngine.prepare(sampleRate, samplesPerBlock);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels      = 2;

    m_distEngine.prepare(spec);
    m_distEngine.initOversampling(2, sampleRate, samplesPerBlock);

    // Apply current oversample mode from APVTS.
    if (m_pDistOversample)
        m_distEngine.setOversampleMode(static_cast<int>(*m_pDistOversample));

    m_metering.reset();
    initSmoothers(sampleRate);
    updateLatency();
}

void AITRAISHItunerProcessor::releaseResources()
{
    m_distEngine.reset();
    m_metering.reset();
}

// ── processBlock ──────────────────────────────────────────────────────────────

void AITRAISHItunerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Instrument plugin: clear the output buffer before writing.
    buffer.clear();

    if (!m_pSampleGain) return;  // parameters not yet initialised

    // ── Preview note injection (atomic, no allocation) ───────────────────────
    if (m_previewTrigger.exchange(false, std::memory_order_acq_rel))
    {
        // Inject a synthetic note-on at sample position 0.
        midiMessages.addEvent(juce::MidiMessage::noteOn(1, AIT::SampleEngine::ROOT_MIDI_NOTE, 1.0f), 0);
        m_previewPlaying.store(true, std::memory_order_release);
    }
    if (m_previewStop.exchange(false, std::memory_order_acq_rel))
    {
        // Use stopPlayback() instead of note-off injection.
        // Note-off would be filtered out by the one-shot MIDI filter below,
        // making Stop ineffective in one-shot mode.
        m_sampleEngine.stopPlayback();
        m_previewPlaying.store(false, std::memory_order_release);
    }

    // ── Fetch and smooth parameters ──────────────────────────────────────────
    m_gainSmooth.setTargetValue(AIT::dBToLinear(m_pSampleGain->load()));
    m_pitchSmooth.setTargetValue(m_pSamplePitch->load());
    m_startSmooth.setTargetValue(m_pSampleStart->load());
    m_endSmooth.setTargetValue(m_pSampleEnd->load());
    m_panSmooth.setTargetValue(m_pSamplePan->load());
    m_driveSmooth.setTargetValue(m_pDistDrive->load());
    m_toneSmooth.setTargetValue(m_pDistTone->load());
    m_distMixSmooth.setTargetValue(m_pDistMix->load());
    m_distOutSmooth.setTargetValue(AIT::dBToLinear(m_pDistOutput->load()));
    m_distInGainSmooth.setTargetValue(AIT::dBToLinear(m_pDistInputGain->load()));
    m_outGainSmooth.setTargetValue(AIT::dBToLinear(m_pOutputGain->load()));
    m_outPanSmooth.setTargetValue(m_pOutputPan->load());

    const float gainLinear = m_gainSmooth.getNextValue();
    const float pitch      = m_pitchSmooth.getNextValue();
    const float start      = m_startSmooth.getNextValue();
    const float end        = m_endSmooth.getNextValue();
    const float pan        = m_panSmooth.getNextValue();
    const bool  loop       = *m_pSampleLoop > 0.5f;
    const bool  oneShot    = *m_pSampleOneShot > 0.5f;

    const auto [panL, panR] = AIT::constantPowerPan(pan);

    AIT::SampleEngineParams sampleParams;
    sampleParams.pitchSemitones = pitch;
    sampleParams.gainLinear     = gainLinear;
    sampleParams.panL           = panL;
    sampleParams.panR           = panR;
    sampleParams.startNorm      = start;
    sampleParams.endNorm        = end;
    sampleParams.loop           = loop;
    sampleParams.oneShot        = oneShot;

    // ── Sample engine: MIDI → audio ──────────────────────────────────────────
    // In one-shot mode, we must NOT send note-off to the engine.
    // Build a filtered MIDI buffer for the sample engine.
    juce::MidiBuffer filteredMidi;
    if (oneShot)
    {
        for (const auto meta : midiMessages)
        {
            const auto msg = meta.getMessage();
            if (!msg.isNoteOff())  // suppress note-off in one-shot mode
                filteredMidi.addEvent(msg, meta.samplePosition);
        }
    }

    m_sampleEngine.process(buffer, oneShot ? filteredMidi : midiMessages, sampleParams);

    // ── Distortion engine ────────────────────────────────────────────────────
    AIT::DistortionParams distParams;
    distParams.drive             = m_driveSmooth.getNextValue();
    distParams.tone              = m_toneSmooth.getNextValue();
    distParams.mix               = m_distMixSmooth.getNextValue();
    distParams.inputGainLinear   = m_distInGainSmooth.getNextValue();
    distParams.outputGainLinear  = m_distOutSmooth.getNextValue();
    distParams.oversampleMode    = static_cast<int>(*m_pDistOversample);

    m_distEngine.process(buffer, distParams);

    // ── Output gain and pan ───────────────────────────────────────────────────
    const float outGain = m_outGainSmooth.getNextValue();
    const float outPan  = m_outPanSmooth.getNextValue();
    const auto [outPanL, outPanR] = AIT::constantPowerPan(outPan);

    AIT::applyPan(buffer, outPanL * outGain, outPanR * outGain);

    // ── Metering ──────────────────────────────────────────────────────────────
    m_metering.process(buffer);
}

// ── State ─────────────────────────────────────────────────────────────────────

void AITRAISHItunerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    m_stateManager.getState(m_currentSamplePath, destData);
}

void AITRAISHItunerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    const juce::String samplePath = m_stateManager.setState(data, sizeInBytes);

    // If the restored state contains a sample path, reload it on the background thread.
    if (samplePath.isNotEmpty())
    {
        const juce::File file(samplePath);
        if (file.existsAsFile())
        {
            m_currentSamplePath = samplePath;
            m_sampleLoader.loadFile(file);
        }
    }
}

// ── Editor ────────────────────────────────────────────────────────────────────

juce::AudioProcessorEditor* AITRAISHItunerProcessor::createEditor()
{
    return new AITRAISHItunerEditor(*this);
}

// ── Public helpers ────────────────────────────────────────────────────────────

void AITRAISHItunerProcessor::loadSampleFile(const juce::File& file)
{
    m_currentSamplePath = file.getFullPathName();
    m_sampleLoader.loadFile(file);
}

juce::String AITRAISHItunerProcessor::getCurrentSamplePath() const
{
    return m_currentSamplePath;
}

void AITRAISHItunerProcessor::triggerPreviewNote()
{
    m_previewTrigger.store(true, std::memory_order_release);
}

void AITRAISHItunerProcessor::stopPreviewNote()
{
    m_previewStop.store(true, std::memory_order_release);
}

// ── Private helpers ───────────────────────────────────────────────────────────

void AITRAISHItunerProcessor::cacheParameterPointers()
{
    namespace P = AIT::Params;
    m_pSampleGain     = m_apvts.getRawParameterValue(P::SAMPLE_GAIN);
    m_pSamplePitch    = m_apvts.getRawParameterValue(P::SAMPLE_PITCH);
    m_pSampleStart    = m_apvts.getRawParameterValue(P::SAMPLE_START);
    m_pSampleEnd      = m_apvts.getRawParameterValue(P::SAMPLE_END);
    m_pSamplePan      = m_apvts.getRawParameterValue(P::SAMPLE_PAN);
    m_pSampleLoop     = m_apvts.getRawParameterValue(P::SAMPLE_LOOP);
    m_pSampleOneShot  = m_apvts.getRawParameterValue(P::SAMPLE_ONESHOT);
    m_pDistDrive      = m_apvts.getRawParameterValue(P::DIST_DRIVE);
    m_pDistTone       = m_apvts.getRawParameterValue(P::DIST_TONE);
    m_pDistMix        = m_apvts.getRawParameterValue(P::DIST_MIX);
    m_pDistOutput     = m_apvts.getRawParameterValue(P::DIST_OUTPUT);
    m_pDistInputGain  = m_apvts.getRawParameterValue(P::DIST_INPUT_GAIN);
    m_pDistOversample = m_apvts.getRawParameterValue(P::DIST_OVERSAMPLE);
    m_pOutputGain     = m_apvts.getRawParameterValue(P::OUTPUT_GAIN);
    m_pOutputPan      = m_apvts.getRawParameterValue(P::OUTPUT_PAN);
}

void AITRAISHItunerProcessor::initSmoothers(double sampleRate)
{
    const double tenMs    = 0.01;
    const double fiveMs   = 0.005;
    const double twentyMs = 0.02;

    auto init = [&](AIT::ParamSmoother<float>& s, double rampSec, float initVal) {
        s.reset(sampleRate, rampSec);
        s.setCurrentAndTargetValue(initVal);
    };

    init(m_gainSmooth,      tenMs,    m_pSampleGain     ? AIT::dBToLinear(m_pSampleGain->load())     : 1.f);
    init(m_pitchSmooth,     tenMs,    m_pSamplePitch    ? m_pSamplePitch->load()    : 0.f);
    init(m_startSmooth,     fiveMs,   m_pSampleStart    ? m_pSampleStart->load()    : 0.f);
    init(m_endSmooth,       fiveMs,   m_pSampleEnd      ? m_pSampleEnd->load()      : 1.f);
    init(m_panSmooth,       tenMs,    m_pSamplePan      ? m_pSamplePan->load()      : 0.f);
    init(m_driveSmooth,     twentyMs, m_pDistDrive      ? m_pDistDrive->load()      : 0.f);
    init(m_toneSmooth,      twentyMs, m_pDistTone       ? m_pDistTone->load()       : 0.5f);
    init(m_distMixSmooth,   tenMs,    m_pDistMix        ? m_pDistMix->load()        : 1.f);
    init(m_distOutSmooth,   tenMs,    m_pDistOutput     ? AIT::dBToLinear(m_pDistOutput->load())     : 1.f);
    init(m_distInGainSmooth,tenMs,    m_pDistInputGain  ? AIT::dBToLinear(m_pDistInputGain->load())  : 1.f);
    init(m_outGainSmooth,   tenMs,    m_pOutputGain     ? AIT::dBToLinear(m_pOutputGain->load())     : 1.f);
    init(m_outPanSmooth,    tenMs,    m_pOutputPan      ? m_pOutputPan->load()      : 0.f);
}

void AITRAISHItunerProcessor::updateLatency()
{
    const int latency = m_distEngine.getLatencyInSamples();
    if (latency != m_reportedLatency)
    {
        m_reportedLatency = latency;
        setLatencySamples(latency);
    }
}

// ── JUCE plugin entry point ───────────────────────────────────────────────────
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AITRAISHItunerProcessor();
}
