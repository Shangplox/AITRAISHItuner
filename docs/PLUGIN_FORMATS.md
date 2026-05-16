# Plugin Formats

## VST3 (Current — MVP)

VST3 is the primary format for AITRAISHItuner.

**Status:** Implemented  
**DAW support:** FL Studio 21+, Reaper, Bitwig, Ableton Live, and any VST3-compatible Windows DAW  
**Plugin type:** Instrument (IS_SYNTH = TRUE, receives MIDI)  
**VST3 SDK license:** MIT (bundled inside JUCE 8)

### Install path on Windows

```
C:\Program Files\Common Files\VST3\AITRAISHItuner\AITRAISHItuner.vst3
```

### Tested DAWs

- FL Studio 21 (Windows) — primary test target
- FL Studio 24 (Windows) — secondary test target

---

## CLAP (Phase 2)

CLAP is a modern open-source plugin format developed by Bitwig and u-he.

**Status:** Not yet implemented  
**Planned via:** `clap-juce-extensions` (third-party community wrapper for JUCE)  
**License:** MIT (clap-juce-extensions is MIT-licensed)  
**DAW support:** Bitwig Studio, Reaper 6.40+, FL Studio (partial support)

### Notes

JUCE 8 does not have official built-in CLAP support. The `clap-juce-extensions` project provides a CMake-based integration. Before adding CLAP:

1. Verify the stability of `clap-juce-extensions` with JUCE 8.
2. Verify that PDC (latency compensation) works correctly in CLAP hosts.
3. Add a `CLAP` entry to `FORMATS` in CMakeLists.txt.
4. Test in Bitwig and Reaper.

---

## Formats not supported

| Format | Reason |
|---|---|
| VST2 | Steinberg discontinued VST2 licensing. No new VST2 products are possible. |
| AU / AUv3 | macOS only. This plugin is Windows-only. |
| AAX | Requires Pro Tools and a commercial PACE/Avid developer program. Not planned. |
| Standalone | JUCE can generate a standalone app, but this plugin is designed for DAW use only. |
