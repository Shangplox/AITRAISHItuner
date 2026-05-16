# Preset Format

## File format

Presets are plain JSON files with the `.json` extension. They are human-readable and easy to share.

## Schema

```json
{
    "name":        "My Preset",
    "author":      "Producer Name",
    "category":    "808 / Kicks / Effects",
    "description": "Optional description",
    "version":     "0.1.0",
    "parameters": {
        "sample-gain":     0.0,
        "sample-pitch":    0.0,
        "sample-start":    0.0,
        "sample-end":      1.0,
        "sample-reverse":  0,
        "sample-loop":     0,
        "sample-oneshot":  1,
        "sample-fade-in":  0.0,
        "sample-fade-out": 0.0,
        "sample-pan":      0.0,
        "dist-mode":       0,
        "dist-drive":      0.0,
        "dist-tone":       0.5,
        "dist-mix":        1.0,
        "dist-output":     0.0,
        "dist-input-gain": 0.0,
        "dist-oversample": 2,
        "output-gain":     0.0,
        "output-pan":      0.0,
        "analyzer-enabled": 1,
        "analyzer-fft-size": 2048
    },
    "sampleFile": "C:\\path\\to\\sample.wav"
}
```

## Field reference

| Field | Type | Notes |
|---|---|---|
| `name` | string | Displayed in the preset dropdown |
| `author` | string | Optional — shown in future preset browser |
| `category` | string | Optional — used for filtering |
| `description` | string | Optional — shown as tooltip |
| `version` | string | Preset format version — use "0.1.0" |
| `parameters` | object | All APVTS parameter values by ID |
| `sampleFile` | string | Absolute Windows path to WAV file, or empty |

## Storage locations

- **Factory presets**: `assets/presets/factory/*.json` — version-controlled, shipped with the plugin
- **User presets**: `%AppData%\AITRAISHItuner\Presets\*.json` — user-created, not version-controlled

## Migration

If a parameter is missing from an older preset, the plugin uses its default value. Parameters with unknown IDs are silently ignored.

This means presets are forwards and backwards compatible within the same major parameter version.

## Sharing presets

A preset JSON file can be sent to another user. If the `sampleFile` path does not exist on their machine, the preset loads with no sample active (all parameter values are still restored).

## Parameter ID reference

All parameter IDs are defined in `src/Plugin/Parameters.h`. They are permanent and will not change after the first public release.
