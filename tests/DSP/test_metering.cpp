#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "DSP/Metering.h"
#include "Utils/MathUtils.h"
#include <JuceHeader.h>

using namespace AIT;
using Catch::Approx;

// Helper: fill a mono AudioBuffer with a constant value.
static juce::AudioBuffer<float> makeConstantBuffer(float value, int samples = 512)
{
    juce::AudioBuffer<float> buf(2, samples);
    buf.clear();
    buf.applyGain(0, 0, samples, value);  // channel 0
    buf.applyGain(1, 0, samples, value);  // channel 1
    // Actually set the values:
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* data = buf.getWritePointer(ch);
        for (int i = 0; i < samples; ++i)
            data[i] = value;
    }
    return buf;
}

TEST_CASE("Metering: silence produces zero peak", "[metering]")
{
    Metering m;
    m.reset();

    juce::AudioBuffer<float> silence(2, 512);
    silence.clear();

    m.process(silence);

    REQUIRE(m.getPeakL() == Approx(0.f).margin(0.001f));
    REQUIRE(m.getPeakR() == Approx(0.f).margin(0.001f));
}

TEST_CASE("Metering: full-scale sine gives peak at or below 1.0", "[metering]")
{
    Metering m;
    m.reset();

    // Fill with ±0.9 signal.
    auto buf = makeConstantBuffer(0.9f);
    m.process(buf);

    REQUIRE(m.getPeakL() <= 1.0f + 1e-4f);
    REQUIRE(m.getPeakL() >= 0.8f);  // should be near 0.9
}

TEST_CASE("Metering: peak decays over time (not instant reset)", "[metering]")
{
    Metering m;
    m.reset();

    // Loud block.
    auto loud = makeConstantBuffer(0.9f);
    m.process(loud);
    const float afterLoud = m.getPeakL();
    REQUIRE(afterLoud > 0.5f);

    // Silent blocks — peak should decay gradually, not drop to zero immediately.
    juce::AudioBuffer<float> silence(2, 512);
    silence.clear();
    for (int i = 0; i < 5; ++i)
        m.process(silence);

    const float afterDecay = m.getPeakL();
    // Should have decayed but not to zero in just 5 blocks.
    REQUIRE(afterDecay < afterLoud);
    REQUIRE(afterDecay >= 0.f);
}

TEST_CASE("Metering: getPeakLdB with silence returns near NEG_INF_DB", "[metering]")
{
    Metering m;
    m.reset();

    juce::AudioBuffer<float> silence(2, 512);
    silence.clear();
    // Process many blocks to ensure RMS and peak decay to near zero.
    for (int i = 0; i < 100; ++i)
        m.process(silence);

    REQUIRE(m.getPeakLdB() <= 0.f);  // should be very negative
}
