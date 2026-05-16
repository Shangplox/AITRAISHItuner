# Security

## Reporting a vulnerability

If you discover a security vulnerability in AITRAISHItuner, please **do not open a public GitHub issue**.

Instead, email: **igcelalcantara@gmail.com**

Include:
- A description of the vulnerability
- Steps to reproduce
- Potential impact
- Any suggested fix if you have one

You will receive a response within 7 days. If the issue is confirmed, a fix will be prioritized and you will be credited in the changelog.

---

## Security posture

AITRAISHItuner is designed with the following security properties:

- **No network calls** — the plugin does not connect to the internet
- **No telemetry** — no usage data is collected or transmitted
- **No DRM / licensing check** — MVP has no license enforcement
- **No shell execution** — the plugin never calls `system()`, `ShellExecute`, or similar
- **No arbitrary code execution** — file loading is limited to audio formats via JUCE's `AudioFormatManager`

### File access

The plugin reads audio files from paths provided by the user (drag-and-drop or file dialog). It writes to:
- `%AppData%\AITRAISHItuner\Presets\` — user presets only

The plugin does **not** write to any system directory without user-initiated action.

### Installer

When a Windows installer is added (Phase 5), it will be signed with a code signing certificate. Until then, manually copied VST3 bundles may trigger Windows SmartScreen warnings — this is expected and not a sign of malware.
