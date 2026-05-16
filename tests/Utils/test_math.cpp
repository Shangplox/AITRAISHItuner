#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Utils/MathUtils.h"

using namespace AIT;
using Catch::Approx;

TEST_CASE("dBToLinear", "[math]")
{
    REQUIRE(dBToLinear(0.f)   == Approx(1.f).epsilon(0.001f));
    REQUIRE(dBToLinear(6.f)   == Approx(1.995f).epsilon(0.01f));
    REQUIRE(dBToLinear(-6.f)  == Approx(0.501f).epsilon(0.01f));
    REQUIRE(dBToLinear(-96.f) == Approx(0.f).margin(0.001f));
    REQUIRE(dBToLinear(-200.f) == 0.f);  // below NEG_INF_DB floor
}

TEST_CASE("linearTodB", "[math]")
{
    REQUIRE(linearTodB(1.f)  == Approx(0.f).epsilon(0.001f));
    REQUIRE(linearTodB(0.f)  == Approx(NEG_INF_DB).epsilon(0.1f));
    REQUIRE(linearTodB(2.f)  == Approx(6.02f).epsilon(0.01f));
    REQUIRE(linearTodB(0.5f) == Approx(-6.02f).epsilon(0.01f));
    REQUIRE(linearTodB(-1.f) == NEG_INF_DB);  // negative linear → floor
}

TEST_CASE("softClip at zero drive is unity gain", "[math]")
{
    for (float x : {-1.f, -0.5f, 0.f, 0.5f, 1.f})
        REQUIRE(softClip(x, 0.f) == Approx(x).epsilon(0.001f));
}

TEST_CASE("softClip full-scale input maps to full-scale output at any drive", "[math]")
{
    // The normalisation tanh(k) / tanh(k) ensures ±1.0 input → ±1.0 output.
    for (float drive : {0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 1.f})
    {
        REQUIRE(softClip( 1.f, drive) == Approx( 1.f).epsilon(0.001f));
        REQUIRE(softClip(-1.f, drive) == Approx(-1.f).epsilon(0.001f));
    }
}

TEST_CASE("softClip output is bounded [-1, 1]", "[math]")
{
    for (float drive : {0.5f, 1.f})
    {
        for (float x : {-2.f, -1.5f, -1.f, 0.f, 1.f, 1.5f, 2.f})
        {
            const float out = softClip(x, drive);
            REQUIRE(out >= -1.f - 1e-5f);
            REQUIRE(out <=  1.f + 1e-5f);
        }
    }
}

TEST_CASE("semitoneRatio correctness", "[math]")
{
    REQUIRE(semitoneRatio(0.0)   == Approx(1.0).epsilon(0.0001));
    REQUIRE(semitoneRatio(12.0)  == Approx(2.0).epsilon(0.001));
    REQUIRE(semitoneRatio(-12.0) == Approx(0.5).epsilon(0.001));
    REQUIRE(semitoneRatio(24.0)  == Approx(4.0).epsilon(0.01));
}

TEST_CASE("constantPowerPan centre is equal power", "[math]")
{
    auto [l, r] = constantPowerPan(0.f);
    REQUIRE(l == Approx(r).epsilon(0.001f));
    REQUIRE(l * l + r * r == Approx(1.f).epsilon(0.001f));
}

TEST_CASE("constantPowerPan hard left/right", "[math]")
{
    {
        auto [l, r] = constantPowerPan(-1.f);
        REQUIRE(l == Approx(1.f).epsilon(0.001f));
        REQUIRE(r == Approx(0.f).margin(0.001f));
    }
    {
        auto [l, r] = constantPowerPan(1.f);
        REQUIRE(l == Approx(0.f).margin(0.001f));
        REQUIRE(r == Approx(1.f).epsilon(0.001f));
    }
}
