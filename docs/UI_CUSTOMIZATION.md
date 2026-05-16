# UI Customization

## Changing colors

All theme colors are defined as `constexpr uint32` values in [src/UI/Theme.h](../src/UI/Theme.h).

```cpp
namespace AIT::Theme {
    inline constexpr juce::uint32 ACCENT_PRIMARY = 0xFF7C4DFF;  // purple
    // ...
}
```

Change any value here to retheme the entire plugin. No other files need editing for color changes.

### Color format

Colors use `0xAARRGGBB` format (JUCE convention):
- `AA` — alpha (FF = fully opaque)
- `RR` — red
- `GG` — green
- `BB` — blue

Example: `0xFF7C4DFF` = fully opaque purple (R=124, G=77, B=255)

---

## Changing fonts

Font sizes are also in `Theme.h`:

```cpp
inline constexpr float FONT_SMALL  = 10.f;
inline constexpr float FONT_NORMAL = 12.f;
inline constexpr float FONT_LABEL  = 11.f;
inline constexpr float FONT_TITLE  = 14.f;
```

To use a custom font, load it via `juce::CustomTypeface` or embed a TTF using `juce_add_binary_data`. Then modify `PluginLookAndFeel` to return your custom font from the appropriate methods.

---

## Changing layout / spacing

Layout constants are also in `Theme.h`:

```cpp
inline constexpr int KNOB_SIZE     = 56;
inline constexpr int HEADER_HEIGHT = 36;
inline constexpr int MARGIN        = 8;
// etc.
```

Layout math is in `PluginEditor::resized()`. Resize the plugin window with `setSize()` in the editor constructor.

---

## Adding a new control

1. Add the parameter ID and range to `Parameters.h` and `Parameters.cpp`.
2. Add a `KnobControl` (or other JUCE component) to the appropriate panel (e.g., `SamplePanel`).
3. Add a `SliderAttachment` or `ButtonAttachment` to the panel's attachment vector.
4. Add the control to `resized()` in the panel.

---

## Custom knob style

Modify `PluginLookAndFeel::drawRotarySlider()` in [src/UI/PluginLookAndFeel.cpp](../src/UI/PluginLookAndFeel.cpp). This single function controls how every knob in the plugin is drawn.

---

## Window resizing

The plugin window is resizable within the bounds set in `PluginEditor`:

```cpp
setResizable(true, true);
setResizeLimits(640, 400, 1600, 960);
```

Change `setResizeLimits()` to allow different minimum/maximum sizes.
