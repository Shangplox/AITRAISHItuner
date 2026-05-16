#include "SampleEngine.h"
#include "Utils/MathUtils.h"

namespace AIT {

void SampleEngine::prepare(double sampleRate, int /*maxBlockSize*/)
{
    m_hostSampleRate = sampleRate;
    m_isPlaying      = false;
    m_playPos        = 0.0;
    m_currentMidiNote = -1;
}

void SampleEngine::process(juce::AudioBuffer<float>& outputBuffer,
                           const juce::MidiBuffer& midiBuffer,
                           const SampleEngineParams& params) noexcept
{
    const int totalSamples = outputBuffer.getNumSamples();
    int currentSample = 0;

    // Process MIDI events in order, rendering audio between each event.
    for (const auto metadata : midiBuffer)
    {
        const int eventPos = metadata.samplePosition;

        // Render audio up to this MIDI event.
        if (eventPos > currentSample)
        {
            renderVoice(outputBuffer, currentSample, eventPos - currentSample, params);
            currentSample = eventPos;
        }

        // Handle the MIDI event.
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff())
            noteOff(msg.getNoteNumber());
        // All other MIDI events are silently discarded.
    }

    // Render remaining samples after last MIDI event.
    if (currentSample < totalSamples)
        renderVoice(outputBuffer, currentSample, totalSamples - currentSample, params);
}

void SampleEngine::noteOn(int midiNote, float velocity) noexcept
{
    m_currentMidiNote = midiNote;
    m_velocity        = velocity;
    m_playPos         = 0.0;  // retrigger from the start on every note-on
    m_isPlaying       = true;
}

void SampleEngine::noteOff(int midiNote) noexcept
{
    // Only stop if this is the note that is currently playing.
    if (midiNote == m_currentMidiNote)
    {
        // In one-shot mode, note-off does NOT stop playback — sample plays to end.
        // For non-one-shot loop mode, note-off stops.
        // The params struct carries one-shot flag; we can't read it here without
        // passing it in. For MVP, we stop unconditionally unless told not to.
        // PluginProcessor decides based on oneShot param whether to call this.
        m_isPlaying = false;
    }
}

void SampleEngine::commitPendingBuffer(juce::AudioBuffer<float>&& newBuffer,
                                       double sourceSampleRate)
{
    // Called from message thread only.
    // Write to the inactive buffer, then atomically swap.
    const int pendingIdx = 1 - m_activeIndex.load(std::memory_order_acquire);

    m_buffers[pendingIdx]          = std::move(newBuffer);
    m_sourceSampleRates[pendingIdx] = sourceSampleRate;

    // Release store: all writes above happen-before any subsequent acquire load.
    m_activeIndex.store(pendingIdx, std::memory_order_release);
    m_hasSample.store(true, std::memory_order_release);

    // Stop any currently playing voice — it will be retriggered by the next note-on.
    m_isPlaying = false;
    m_playPos   = 0.0;
}

const juce::AudioBuffer<float>& SampleEngine::getDisplayBuffer() const noexcept
{
    return m_buffers[m_activeIndex.load(std::memory_order_acquire)];
}

void SampleEngine::renderVoice(juce::AudioBuffer<float>& output,
                                int startSample, int numSamples,
                                const SampleEngineParams& params) noexcept
{
    if (!m_isPlaying || !m_hasSample.load(std::memory_order_acquire))
        return;

    const int activeIdx = m_activeIndex.load(std::memory_order_acquire);
    const juce::AudioBuffer<float>& src = m_buffers[activeIdx];
    const double sourceRate = m_sourceSampleRates[activeIdx];

    const int srcNumSamples = src.getNumSamples();
    const int srcNumChannels = src.getNumChannels();

    if (srcNumSamples < 2 || srcNumChannels == 0)
        return;

    // Compute playback bounds from start/end parameters.
    const int startPos = static_cast<int>(params.startNorm * (srcNumSamples - 1));
    const int endPos   = static_cast<int>(params.endNorm   * (srcNumSamples - 1));

    if (startPos >= endPos)
        return;

    // Resampling ratio: combines MIDI pitch offset + manual pitch param + sample rate difference.
    const double midiSemitones   = static_cast<double>(m_currentMidiNote - ROOT_MIDI_NOTE);
    const double totalSemitones  = midiSemitones + static_cast<double>(params.pitchSemitones);
    const double pitchRatio      = semitoneRatio(totalSemitones);
    const double rateRatio       = (m_hostSampleRate > 0.0) ? (sourceRate / m_hostSampleRate) : 1.0;
    const double playbackRatio   = pitchRatio * rateRatio;

    // Set play head to start bounds on first use.
    if (m_playPos < startPos)
        m_playPos = startPos;

    auto* outL = output.getWritePointer(0, startSample);
    auto* outR = output.getNumChannels() > 1 ? output.getWritePointer(1, startSample) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        if (!m_isPlaying)
        {
            // Fill remainder with silence.
            for (int j = i; j < numSamples; ++j)
            {
                outL[j] = 0.f;
                if (outR) outR[j] = 0.f;
            }
            return;
        }

        const int   posInt = static_cast<int>(m_playPos);
        const float frac   = static_cast<float>(m_playPos - posInt);

        // Guard against reading past the end.
        if (posInt + 1 > endPos)
        {
            if (params.loop)
            {
                m_playPos = startPos;
                continue;
            }
            m_isPlaying = false;
            outL[i] = 0.f;
            if (outR) outR[i] = 0.f;
            continue;
        }

        // Linear interpolation for both channels.
        auto lerp = [&](int ch) -> float {
            const int useCh = std::min(ch, srcNumChannels - 1);
            const auto* data = src.getReadPointer(useCh);
            return data[posInt] * (1.f - frac) + data[posInt + 1] * frac;
        };

        const float sampleL = lerp(0) * params.gainLinear * m_velocity * params.panL;
        const float sampleR = lerp(srcNumChannels > 1 ? 1 : 0) * params.gainLinear * m_velocity * params.panR;

        outL[i] = sampleL;
        if (outR) outR[i] = sampleR;

        m_playPos += playbackRatio;

        // Loop / one-shot boundary handling.
        if (m_playPos >= endPos)
        {
            if (params.loop)
                m_playPos = startPos;
            else
                m_isPlaying = false;
        }
    }
}

} // namespace AIT
