#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "DSP/SampleEngine.h"

using namespace AIT;
using Catch::Approx;

// Helper: load a simple sine-like buffer into SampleEngine.
static juce::AudioBuffer<float> makeSineBuffer(int numSamples = 4096, float freq = 440.f, float sampleRate = 44100.f)
{
    juce::AudioBuffer<float> buf(2, numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        const float s = std::sin(2.f * juce::MathConstants<float>::pi * freq * i / sampleRate);
        buf.setSample(0, i, s);
        buf.setSample(1, i, s);
    }
    return buf;
}

TEST_CASE("SampleEngine: no sample produces silent output", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);

    juce::AudioBuffer<float> out(2, 512);
    out.clear();
    juce::MidiBuffer midi;

    SampleEngineParams params;
    engine.process(out, midi, params);

    REQUIRE(out.getMagnitude(0, 512) == Approx(0.f).margin(1e-5f));
}

TEST_CASE("SampleEngine: note-on triggers non-silent output", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);

    // Load a sample.
    auto buf = makeSineBuffer(4096);
    engine.commitPendingBuffer(std::move(buf), 44100.0);

    juce::AudioBuffer<float> out(2, 512);
    out.clear();

    // Trigger a note.
    juce::MidiBuffer midi;
    midi.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);

    SampleEngineParams params;
    params.gainLinear = 1.f;
    params.panL = 1.f;
    params.panR = 1.f;
    params.startNorm = 0.f;
    params.endNorm = 1.f;
    params.oneShot = true;

    engine.process(out, midi, params);

    // After a note-on, output should not be silent.
    REQUIRE(out.getMagnitude(0, 512) > 0.01f);
}

TEST_CASE("SampleEngine: start > end produces silence safely", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);

    auto buf = makeSineBuffer(4096);
    engine.commitPendingBuffer(std::move(buf), 44100.0);

    juce::AudioBuffer<float> out(2, 512);
    out.clear();

    juce::MidiBuffer midi;
    midi.addEvent(juce::MidiMessage::noteOn(1, 60, 1.0f), 0);

    SampleEngineParams params;
    params.startNorm = 0.8f;  // start > end → invalid range → should produce silence safely
    params.endNorm   = 0.2f;
    params.gainLinear = 1.f;
    params.panL = 1.f;
    params.panR = 1.f;

    // Should not crash or produce NaN.
    REQUIRE_NOTHROW(engine.process(out, midi, params));

    const float mag = out.getMagnitude(0, 512);
    REQUIRE_FALSE(std::isnan(mag));
    REQUIRE_FALSE(std::isinf(mag));
}

TEST_CASE("SampleEngine: gain=0 produces silence", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);

    auto buf = makeSineBuffer(4096);
    engine.commitPendingBuffer(std::move(buf), 44100.0);

    juce::AudioBuffer<float> out(2, 512);
    out.clear();

    juce::MidiBuffer midi;
    midi.addEvent(juce::MidiMessage::noteOn(1, 60, 1.0f), 0);

    SampleEngineParams params;
    params.gainLinear = 0.f;  // zero gain
    params.panL = 1.f;
    params.panR = 1.f;
    params.startNorm = 0.f;
    params.endNorm = 1.f;

    engine.process(out, midi, params);

    REQUIRE(out.getMagnitude(0, 512) == Approx(0.f).margin(1e-5f));
}

TEST_CASE("SampleEngine: hasSample returns false before load", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);
    REQUIRE_FALSE(engine.hasSample());
}

TEST_CASE("SampleEngine: hasSample returns true after commitPendingBuffer", "[sample_engine]")
{
    SampleEngine engine;
    engine.prepare(44100.0, 512);

    auto buf = makeSineBuffer(4096);
    engine.commitPendingBuffer(std::move(buf), 44100.0);

    REQUIRE(engine.hasSample());
}
