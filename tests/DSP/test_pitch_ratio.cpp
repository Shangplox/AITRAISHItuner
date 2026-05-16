#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "Utils/MathUtils.h"

using Catch::Approx;

TEST_CASE("Pitch ratio at 0 semitones is 1.0", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(0.0) == Approx(1.0).epsilon(0.0001));
}

TEST_CASE("Pitch ratio at +12 semitones is exactly 2.0 (one octave up)", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(12.0) == Approx(2.0).epsilon(0.001));
}

TEST_CASE("Pitch ratio at -12 semitones is exactly 0.5 (one octave down)", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(-12.0) == Approx(0.5).epsilon(0.001));
}

TEST_CASE("Pitch ratio at +24 semitones is 4.0 (two octaves up)", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(24.0) == Approx(4.0).epsilon(0.01));
}

TEST_CASE("Pitch ratio at -24 semitones is 0.25 (two octaves down)", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(-24.0) == Approx(0.25).epsilon(0.001));
}

TEST_CASE("Pitch ratio at +7 semitones (perfect fifth) is approx 1.498", "[pitch]")
{
    REQUIRE(AIT::semitoneRatio(7.0) == Approx(1.4983f).epsilon(0.001));
}

TEST_CASE("Semitone ratio is always positive", "[pitch]")
{
    for (double st : {-24.0, -12.0, -6.0, 0.0, 6.0, 12.0, 24.0})
        REQUIRE(AIT::semitoneRatio(st) > 0.0);
}
