#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Utils/MathUtils.h"
#include <cmath>
#include <limits>

using namespace AIT;
using Catch::Approx;

TEST_CASE("softClip: silence in, silence out", "[distortion]")
{
    for (float drive : {0.f, 0.3f, 0.7f, 1.f})
        REQUIRE(softClip(0.f, drive) == Approx(0.f).margin(1e-6f));
}

TEST_CASE("softClip: no NaN or Inf output", "[distortion]")
{
    for (float drive : {0.f, 0.001f, 0.1f, 0.5f, 0.99f, 1.f})
    {
        for (float x : {-2.f, -1.f, -0.5f, 0.f, 0.5f, 1.f, 2.f})
        {
            const float out = softClip(x, drive);
            REQUIRE_FALSE(std::isnan(out));
            REQUIRE_FALSE(std::isinf(out));
        }
    }
}

TEST_CASE("softClip: output is monotonic for fixed drive", "[distortion]")
{
    // A higher input magnitude should give a higher or equal output magnitude.
    const float drive = 0.5f;
    float prev = 0.f;
    for (float x : {0.f, 0.1f, 0.2f, 0.3f, 0.5f, 0.7f, 1.f})
    {
        const float out = softClip(x, drive);
        REQUIRE(out >= prev - 1e-5f);
        prev = out;
    }
}

TEST_CASE("softClip: odd symmetry (anti-symmetric)", "[distortion]")
{
    // tanh is an odd function: f(-x) = -f(x)
    for (float drive : {0.1f, 0.5f, 1.f})
    {
        for (float x : {0.1f, 0.5f, 0.9f})
            REQUIRE(softClip(-x, drive) == Approx(-softClip(x, drive)).epsilon(0.0001f));
    }
}

TEST_CASE("softClip: heavy drive compresses dynamic range", "[distortion]")
{
    // At high drive, the gap between 0.5 and 1.0 input should be smaller than at low drive.
    const float gap_low_drive  = softClip(1.f, 0.05f) - softClip(0.5f, 0.05f);
    const float gap_high_drive = softClip(1.f, 0.9f)  - softClip(0.5f, 0.9f);
    REQUIRE(gap_high_drive < gap_low_drive);
}

TEST_CASE("dBToLinear / linearTodB roundtrip", "[distortion]")
{
    for (float dB : {-24.f, -12.f, -6.f, 0.f, 6.f, 12.f, 24.f})
        REQUIRE(linearTodB(dBToLinear(dB)) == Approx(dB).epsilon(0.01f));
}
