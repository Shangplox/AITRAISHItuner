# Agent Handoff File

## Rules for Every Agent That Touches This File

1. **Read this file first** before doing anything else in a new session.
2. **Delete the previous agent's "Last Advance" section** entirely — do not keep it.
3. **Write your own "Last Advance" section** when you finish your session or hit a stopping point.
4. **Be concise and actionable** — the next agent needs facts, not prose. Bullet points only.
5. **Include blockers explicitly** — if you are stopped by something external (user action needed, install pending, etc.), say so clearly.
6. **Do not write what is already in CLAUDE.md** — phase rules, parameter IDs, and coding standards live there. Only write transient state here.
7. **Update this file at the end of every work session**, even if nothing changed (note why).

---

## Last Advance

**Agent:** Claude Sonnet 4.6
**Date:** 2026-05-16
**Session summary:** VS2022 installed, full build succeeded, 31/31 tests pass. Blocked on FL Studio scan.

### What was completed
- VS2022 Community installed silently (MSVC 14.44.35227, Windows SDK 10.0.26100)
- CMake configure succeeded (187s first run, ~10s incremental)
- **VST3 plugin compiled and linked** → `build\release\AITRAISHItuner_artefacts\Release\VST3\AITRAISHItuner.vst3`
- **31/31 tests passing** (`ctest --preset windows-vs2022-release`)
- Fixed 6 compile errors from JUCE 8 API differences (see below)

### Bugs fixed this session
| Bug | Fix |
|---|---|
| `using P = AIT::Params` (namespace can't be aliased with `using T=`) | Changed to `namespace P = AIT::Params` in 4 files |
| `getLatencySamples() const override` — not virtual in JUCE 8 | Removed override; `setLatencySamples()` already called in `updateLatency()` |
| `AlertWindow::showInputBoxAsync` removed in JUCE 8 | Replaced with `AlertWindow` + `addTextEditor` + `enterModalState` |
| `FileChooser::browseForFileToOpen()` removed in JUCE 8 | Replaced with `launchAsync`; added `m_fileChooser` member + `onFileOpen` callback |
| Test `JuceHeader.h` pulled in `juce_audio_plugin_client` | Created `tests/JuceHeader.h` that includes only DSP modules |
| Non-ASCII chars (`→` `≤`) in test names corrupted by CTest/Windows | Replaced with ASCII equivalents in all 4 affected test names |

### Current blocker
- **VST3 must be copied to `C:\Program Files\Common Files\VST3\AITRAISHItuner\` (requires admin)**
- Command (run in admin PowerShell):
  ```
  xcopy /E /I /Y "C:\Project\AITRAISHItuner\build\release\AITRAISHItuner_artefacts\Release\VST3\AITRAISHItuner.vst3" "C:\Program Files\Common Files\VST3\AITRAISHItuner\AITRAISHItuner.vst3\"
  ```
- Then: rescan VST3 plugins in FL Studio

### Next actions (in order)
1. Copy VST3 to Program Files (admin required — see command above)
2. Open FL Studio → Options → Manage plugins → rescan VST3
3. Verify: plugin appears in browser, scan completes without crash
4. Load plugin on a Mixer channel or as step sequencer instrument
5. Verify: plugin editor opens (820×520 window with Header, Waveform, Sample/Dist/Output panels)
6. Test drag-and-drop of a WAV file onto the waveform display
7. Trigger a MIDI note → confirm audio output
8. Run through the full Phase 1 Testing Checklist in CLAUDE.md

### Key file locations
- Entry point: [src/Plugin/PluginProcessor.cpp](src/Plugin/PluginProcessor.cpp)
- DSP chain: [src/DSP/DistortionEngine.cpp](src/DSP/DistortionEngine.cpp), [src/DSP/SampleEngine.cpp](src/DSP/SampleEngine.cpp)
- UI entry: [src/Plugin/PluginEditor.cpp](src/Plugin/PluginEditor.cpp)
- All parameters: [src/Plugin/Parameters.h](src/Plugin/Parameters.h)
- Build presets: [CMakePresets.json](CMakePresets.json)
- Test config: [tests/CMakeLists.txt](tests/CMakeLists.txt), [tests/JuceHeader.h](tests/JuceHeader.h)

### Current phase
**Phase 1 complete pending FL Studio scan.**
Phase 2 (APVTS state save/load) is structurally implemented in code but not verified in FL Studio yet.
Do not mark Phase 1 done until the Testing Checklist in CLAUDE.md is fully checked.
