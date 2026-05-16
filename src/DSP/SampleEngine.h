#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <array>

namespace AIT {

// Parameters passed into SampleEngine::process() each block.
// Values are pre-fetched and pre-smoothed by PluginProcessor — no APVTS reference here.
struct SampleEngineParams
{
    float pitchSemitones{0.f};  // manual pitch offset from APVTS (+ MIDI note offset applied internally)
    float gainLinear{1.f};      // linear gain from sample-gain dB
    float panL{1.f};            // left channel gain (constant-power)
    float panR{1.f};            // right channel gain (constant-power)
    float startNorm{0.f};       // 0–1 normalized start position
    float endNorm{1.f};         // 0–1 normalized end position
    bool  loop{false};
    bool  oneShot{true};
};

// Monophonic MIDI-triggered sample playback engine.
// No JUCE UI headers included here — DSP only.
// Double-buffered for lock-free sample loading from background thread.
class SampleEngine
{
public:
    static constexpr int   ROOT_MIDI_NOTE = 60;  // C3 — MIDI note that plays sample at original pitch

    SampleEngine() = default;
    ~SampleEngine() = default;

    // Called from prepareToPlay (message thread or audio thread before streaming starts).
    void prepare(double sampleRate, int maxBlockSize);

    // Called from audio thread inside processBlock.
    // Renders into outputBuffer (assumed cleared before this call).
    void process(juce::AudioBuffer<float>& outputBuffer,
                 const juce::MidiBuffer& midiBuffer,
                 const SampleEngineParams& params) noexcept;

    // Called from audio thread — handles a single MIDI note-on event.
    void noteOn(int midiNote, float velocity) noexcept;

    // Called from audio thread — handles a single MIDI note-off event.
    void noteOff(int midiNote) noexcept;

    // Called from message thread after SampleLoader finishes loading.
    // Thread-safe via atomic double-buffer swap.
    void commitPendingBuffer(juce::AudioBuffer<float>&& newBuffer, double sourceSampleRate);

    bool hasSample() const noexcept { return m_hasSample.load(std::memory_order_acquire); }

    // Read-only access for waveform display — message thread only.
    // Returns a reference to the currently active buffer; valid until next commitPendingBuffer().
    const juce::AudioBuffer<float>& getDisplayBuffer() const noexcept;

private:
    // Double buffer for lock-free sample swap.
    // Audio thread reads from m_buffers[m_activeIndex].
    // Message thread writes to m_buffers[1 - m_activeIndex], then swaps.
    juce::AudioBuffer<float> m_buffers[2];
    double                   m_sourceSampleRates[2]{44100.0, 44100.0};
    std::atomic<int>         m_activeIndex{0};
    std::atomic<bool>        m_hasSample{false};

    // Playback state — audio thread only.
    double m_playPos{0.0};
    bool   m_isPlaying{false};
    int    m_currentMidiNote{-1};
    float  m_velocity{1.f};

    double m_hostSampleRate{44100.0};

    // Renders a slice of the voice into output. Called inside process().
    void renderVoice(juce::AudioBuffer<float>& output,
                     int startSample, int numSamples,
                     const SampleEngineParams& params) noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleEngine)
};

} // namespace AIT
