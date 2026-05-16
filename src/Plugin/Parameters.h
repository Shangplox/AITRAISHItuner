#pragma once
#include <JuceHeader.h>

// All APVTS parameter IDs live here as compile-time constants.
// These IDs are PERMANENT once a public build ships.
// Never rename after first beta release — breaks DAW project state.
// Add a migration function if any rename is ever required.

namespace AIT::Params {

// ── Sample Engine ─────────────────────────────────────────────────────────────
inline constexpr const char* SAMPLE_GAIN     = "sample-gain";      // dB, -24 to +24
inline constexpr const char* SAMPLE_PITCH    = "sample-pitch";     // semitones, -24 to +24
inline constexpr const char* SAMPLE_START    = "sample-start";     // 0.0 to 1.0
inline constexpr const char* SAMPLE_END      = "sample-end";       // 0.0 to 1.0
inline constexpr const char* SAMPLE_REVERSE  = "sample-reverse";   // bool — NOT automatable
inline constexpr const char* SAMPLE_LOOP     = "sample-loop";      // bool
inline constexpr const char* SAMPLE_ONESHOT  = "sample-oneshot";   // bool — NOT automatable
inline constexpr const char* SAMPLE_FADE_IN  = "sample-fade-in";   // ms, 0 to 1000 — impl Phase 2
inline constexpr const char* SAMPLE_FADE_OUT = "sample-fade-out";  // ms, 0 to 1000 — impl Phase 2
inline constexpr const char* SAMPLE_PAN      = "sample-pan";       // -1.0 to 1.0

// ── Distortion Engine ─────────────────────────────────────────────────────────
inline constexpr const char* DIST_MODE       = "dist-mode";        // int, 0-5 — NOT automatable; modes 1-5 unimpl in MVP
inline constexpr const char* DIST_DRIVE      = "dist-drive";       // 0.0 to 1.0
inline constexpr const char* DIST_TONE       = "dist-tone";        // 0.0 to 1.0
inline constexpr const char* DIST_MIX        = "dist-mix";         // 0.0 to 1.0
inline constexpr const char* DIST_OUTPUT     = "dist-output";      // dB, -24 to 0
inline constexpr const char* DIST_INPUT_GAIN = "dist-input-gain";  // dB, 0 to +24
inline constexpr const char* DIST_OVERSAMPLE = "dist-oversample";  // int 1=1x 2=4x — NOT automatable (changes PDC)

// ── Output / Global ───────────────────────────────────────────────────────────
inline constexpr const char* OUTPUT_GAIN     = "output-gain";      // dB, -24 to +6
inline constexpr const char* OUTPUT_PAN      = "output-pan";       // -1.0 to 1.0

// ── Analyzer ──────────────────────────────────────────────────────────────────
inline constexpr const char* ANALYZER_ENABLED  = "analyzer-enabled";   // bool — NOT automatable
inline constexpr const char* ANALYZER_FFT_SIZE = "analyzer-fft-size";  // int — NOT automatable; Phase 2 display

} // namespace AIT::Params

namespace AIT {

// Creates the full APVTS parameter layout.
// Called once in PluginProcessor constructor.
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

} // namespace AIT
