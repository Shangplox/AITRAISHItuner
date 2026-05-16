#pragma once
#include <JuceHeader.h>

namespace AIT {

// All colors, fonts, and spacing live here.
// No hex literals in component files — always reference Theme constants.
// Change these to retheme the entire plugin.
namespace Theme {

// ── Palette ────────────────────────────────────────────────────────────────────
inline constexpr juce::uint32 BACKGROUND_DARK  = 0xFF1A1A1A;
inline constexpr juce::uint32 BACKGROUND_MID   = 0xFF242424;
inline constexpr juce::uint32 BACKGROUND_PANEL = 0xFF2C2C2C;
inline constexpr juce::uint32 BACKGROUND_CTRL  = 0xFF333333;

inline constexpr juce::uint32 ACCENT_PRIMARY   = 0xFF7C4DFF;  // purple
inline constexpr juce::uint32 ACCENT_SECONDARY = 0xFF03DAC6;  // teal
inline constexpr juce::uint32 ACCENT_DANGER    = 0xFFCF6679;  // red-pink

inline constexpr juce::uint32 TEXT_PRIMARY      = 0xFFE0E0E0;
inline constexpr juce::uint32 TEXT_SECONDARY    = 0xFF9E9E9E;
inline constexpr juce::uint32 TEXT_DISABLED     = 0xFF555555;

inline constexpr juce::uint32 METER_LOW         = 0xFF4CAF50;  // green
inline constexpr juce::uint32 METER_MID         = 0xFFFFEB3B;  // yellow
inline constexpr juce::uint32 METER_CLIP        = 0xFFF44336;  // red

inline constexpr juce::uint32 WAVEFORM_FILL     = 0x3C7C4DFF;  // accent at 24% opacity
inline constexpr juce::uint32 WAVEFORM_LINE     = 0xFF7C4DFF;

inline constexpr juce::uint32 SEPARATOR         = 0xFF3A3A3A;
inline constexpr juce::uint32 KNOB_TRACK        = 0xFF444444;
inline constexpr juce::uint32 KNOB_FILL         = 0xFF7C4DFF;

// ── Helper: get colour from uint32 ─────────────────────────────────────────────
inline juce::Colour colour(juce::uint32 argb) noexcept
{
    return juce::Colour(argb);
}

// ── Typography ─────────────────────────────────────────────────────────────────
inline constexpr float FONT_SMALL  = 10.f;
inline constexpr float FONT_NORMAL = 12.f;
inline constexpr float FONT_LABEL  = 11.f;
inline constexpr float FONT_TITLE  = 14.f;

// ── Spacing ────────────────────────────────────────────────────────────────────
inline constexpr int MARGIN         = 8;
inline constexpr int PADDING        = 6;
inline constexpr int KNOB_SIZE      = 56;
inline constexpr int PANEL_RADIUS   = 6;
inline constexpr int HEADER_HEIGHT  = 36;
inline constexpr int WAVEFORM_HEIGHT= 100;
inline constexpr int METER_WIDTH    = 20;
inline constexpr int BOTTOM_BAR_H   = 36;

} // namespace Theme
} // namespace AIT
