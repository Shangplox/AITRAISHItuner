#pragma once
#include <cmath>
#include <limits>

namespace AIT {

constexpr float NEG_INF_DB = -96.f;
constexpr float MAX_OVERSAMPLE_FACTOR = 8;

inline float dBToLinear(float dB) noexcept
{
    if (dB <= NEG_INF_DB)
        return 0.f;
    return std::pow(10.f, dB * 0.05f);
}

inline float linearTodB(float linear) noexcept
{
    if (linear <= 0.f)
        return NEG_INF_DB;
    return 20.f * std::log10(linear);
}

// Soft-clip transfer function.
// drive in [0, 1]. At drive=0: unity gain passthrough.
// At drive=1: heavy tanh saturation with level compensation.
inline float softClip(float x, float drive) noexcept
{
    if (drive < 0.001f)
        return x;
    const float k = 1.f + drive * 63.f;  // maps 0→1 to 1x→64x input scale
    // tanh(k) normalises output so full-scale input → full-scale output
    return std::tanh(x * k) / std::tanh(k);
}

// Semitone offset → playback ratio (simple resampling pitch shift).
// +12 semitones = 2x playback rate (one octave up).
inline double semitoneRatio(double semitones) noexcept
{
    return std::pow(2.0, semitones / 12.0);
}

// Constant-power panning (equal-power law).
// pan in [-1, 1]. Returns {gainL, gainR}.
inline std::pair<float, float> constantPowerPan(float pan) noexcept
{
    // Map pan [-1,1] → angle [0, pi/2]
    const float angle = (pan + 1.f) * 0.25f * 3.14159265f;
    return { std::cos(angle), std::sin(angle) };
}

} // namespace AIT
