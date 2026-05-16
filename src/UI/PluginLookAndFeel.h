#pragma once
#include <JuceHeader.h>
#include "Theme.h"

namespace AIT {

// Custom JUCE LookAndFeel for the AITRAISHItuner plugin.
// All visual rendering is centralised here — components just call standard JUCE methods.
class PluginLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PluginLookAndFeel();

    // ── Rotary slider (knobs) ─────────────────────────────────────────────────
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    // ── Linear slider ─────────────────────────────────────────────────────────
    void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle, juce::Slider&) override;

    // ── Combo box ─────────────────────────────────────────────────────────────
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;

    // ── Toggle button (for Loop, OneShot, etc.) ───────────────────────────────
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    // ── Label ─────────────────────────────────────────────────────────────────
    void drawLabel(juce::Graphics&, juce::Label&) override;

    // ── Text for combo box popup ───────────────────────────────────────────────
    juce::Font getComboBoxFont(juce::ComboBox&) override;

    // ── Scrollbar ─────────────────────────────────────────────────────────────
    void drawScrollbar(juce::Graphics&, juce::ScrollBar&, int x, int y, int w, int h,
                       bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                       bool isMouseOver, bool isMouseDown) override;
};

} // namespace AIT
