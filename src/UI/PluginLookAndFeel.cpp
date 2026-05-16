#include "PluginLookAndFeel.h"

namespace AIT {

using namespace Theme;

PluginLookAndFeel::PluginLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId,   colour(BACKGROUND_DARK));
    setColour(juce::Slider::thumbColourId,                 colour(ACCENT_PRIMARY));
    setColour(juce::Slider::rotarySliderFillColourId,      colour(KNOB_FILL));
    setColour(juce::Slider::rotarySliderOutlineColourId,   colour(KNOB_TRACK));
    setColour(juce::Slider::trackColourId,                 colour(KNOB_FILL));
    setColour(juce::Slider::backgroundColourId,            colour(KNOB_TRACK));
    setColour(juce::Label::textColourId,                   colour(TEXT_PRIMARY));
    setColour(juce::Label::backgroundColourId,             juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId,          colour(BACKGROUND_CTRL));
    setColour(juce::ComboBox::textColourId,                colour(TEXT_PRIMARY));
    setColour(juce::ComboBox::outlineColourId,             colour(SEPARATOR));
    setColour(juce::ComboBox::arrowColourId,               colour(ACCENT_PRIMARY));
    setColour(juce::ToggleButton::textColourId,            colour(TEXT_PRIMARY));
    setColour(juce::ToggleButton::tickColourId,            colour(ACCENT_PRIMARY));
    setColour(juce::ToggleButton::tickDisabledColourId,    colour(TEXT_DISABLED));
    setColour(juce::PopupMenu::backgroundColourId,         colour(BACKGROUND_MID));
    setColour(juce::PopupMenu::textColourId,               colour(TEXT_PRIMARY));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, colour(ACCENT_PRIMARY));
    setColour(juce::PopupMenu::highlightedTextColourId,    colour(TEXT_PRIMARY));
    setColour(juce::ScrollBar::thumbColourId,              colour(SEPARATOR));
}

void PluginLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle,
                                          float rotaryEndAngle, juce::Slider&)
{
    const float radius   = std::min(width, height) * 0.5f - 4.f;
    const float centreX  = x + width  * 0.5f;
    const float centreY  = y + height * 0.5f;
    const float angle    = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float thickness = 4.f;

    // Track arc (background).
    {
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f,
                          rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(colour(KNOB_TRACK));
        g.strokePath(arc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    }

    // Fill arc (value).
    {
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.f,
                          rotaryStartAngle, angle, true);
        g.setColour(colour(KNOB_FILL));
        g.strokePath(arc, juce::PathStrokeType(thickness, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    }

    // Pointer dot.
    const float dotRadius = 3.f;
    const float dotX = centreX + (radius - 2.f) * std::cos(angle - juce::MathConstants<float>::halfPi);
    const float dotY = centreY + (radius - 2.f) * std::sin(angle - juce::MathConstants<float>::halfPi);
    g.setColour(colour(TEXT_PRIMARY));
    g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.f, dotRadius * 2.f);

    // Inner circle (knob body).
    const float innerR = radius - 8.f;
    g.setColour(colour(BACKGROUND_CTRL));
    g.fillEllipse(centreX - innerR, centreY - innerR, innerR * 2.f, innerR * 2.f);
}

void PluginLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                          juce::Slider::SliderStyle style, juce::Slider& slider)
{
    // Delegate to default for linear sliders (not used much in this plugin).
    LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                     0.f, 1.f, style, slider);
}

void PluginLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                      int, int, int, int, juce::ComboBox& box)
{
    const auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height);
    g.setColour(colour(BACKGROUND_CTRL));
    g.fillRoundedRectangle(bounds, 4.f);
    g.setColour(colour(SEPARATOR));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.f, 1.f);

    // Arrow.
    const float arrowX = width - 16.f;
    const float arrowY = height * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(arrowX, arrowY - 3.f, arrowX + 8.f, arrowY - 3.f, arrowX + 4.f, arrowY + 3.f);
    g.setColour(colour(ACCENT_PRIMARY));
    g.fillPath(arrow);
}

void PluginLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool /*shouldDrawButtonAsDown*/)
{
    const bool on = button.getToggleState();
    const auto bounds = button.getLocalBounds().toFloat().reduced(1.f);

    g.setColour(on ? colour(ACCENT_PRIMARY) : colour(BACKGROUND_CTRL));
    g.fillRoundedRectangle(bounds, 4.f);

    g.setColour(on ? colour(TEXT_PRIMARY) : colour(TEXT_SECONDARY));
    if (shouldDrawButtonAsHighlighted)
        g.setColour(colour(TEXT_PRIMARY));

    g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
}

void PluginLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(juce::Font(FONT_LABEL));
        g.drawFittedText(label.getText(), label.getLocalBounds(),
                         label.getJustificationType(), 1, 1.f);
    }
}

juce::Font PluginLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return juce::Font(FONT_NORMAL);
}

void PluginLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar&,
                                       int x, int y, int w, int h,
                                       bool /*isVertical*/, int thumbStartPosition, int thumbSize,
                                       bool /*isMouseOver*/, bool /*isMouseDown*/)
{
    g.setColour(colour(BACKGROUND_MID));
    g.fillRect(x, y, w, h);
    g.setColour(colour(SEPARATOR));
    g.fillRect(x, y + thumbStartPosition, w, thumbSize);
}

} // namespace AIT
