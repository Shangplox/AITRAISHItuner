cmake_minimum_required(VERSION 3.22)

include(cmake/PluginConfig.cmake)

project(${PLUGIN_NAME}
    VERSION     ${PLUGIN_VERSION}
    DESCRIPTION ${PLUGIN_DESCRIPTION}
    LANGUAGES   CXX
)

# ── JUCE ──────────────────────────────────────────────────────────────────────
include(cmake/JuceSetup.cmake)
include(cmake/CompilerOptions.cmake)

# ── Plugin target ─────────────────────────────────────────────────────────────
juce_add_plugin(AITRAISHItuner
    VERSION                 ${PLUGIN_VERSION}
    PLUGIN_MANUFACTURER_CODE ${PLUGIN_MANUFACTURER_CODE}
    PLUGIN_CODE             ${PLUGIN_CODE}
    FORMATS                 VST3
    IS_SYNTH                TRUE
    NEEDS_MIDI_INPUT        TRUE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    PRODUCT_NAME            ${PLUGIN_NAME}
    COMPANY_NAME            ${PLUGIN_MANUFACTURER}
    BUNDLE_ID               ${PLUGIN_BUNDLE_ID}
    VST3_CATEGORIES         ${PLUGIN_VST3_CATEGORIES}
    DESCRIPTION             ${PLUGIN_DESCRIPTION}
    COPY_PLUGIN_AFTER_BUILD FALSE  # set TRUE locally for fast iteration; keep FALSE for CI
)

# ── Sources ───────────────────────────────────────────────────────────────────
target_sources(AITRAISHItuner
    PRIVATE
        src/Plugin/PluginProcessor.cpp
        src/Plugin/PluginEditor.cpp
        src/Plugin/Parameters.cpp
        src/Plugin/Presets.cpp
        src/DSP/SampleEngine.cpp
        src/DSP/SampleLoader.cpp
        src/DSP/DistortionEngine.cpp
        src/DSP/OversamplingWrapper.cpp
        src/DSP/ToneFilter.cpp
        src/State/StateManager.cpp
        src/State/Serialization.cpp
        src/UI/Theme.cpp
        src/UI/PluginLookAndFeel.cpp
        src/UI/KnobControl.cpp
        src/UI/WaveformDisplay.cpp
        src/UI/MeterDisplay.cpp
        src/UI/SamplePanel.cpp
        src/UI/DistortionPanel.cpp
        src/UI/OutputPanel.cpp
        src/UI/HeaderBar.cpp
)

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(AITRAISHItuner
    PRIVATE
        src
)

# ── Compiler options ──────────────────────────────────────────────────────────
apply_compiler_options(AITRAISHItuner)

# ── JUCE compile definitions ──────────────────────────────────────────────────
target_compile_definitions(AITRAISHItuner
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

# ── JUCE modules ──────────────────────────────────────────────────────────────
target_link_libraries(AITRAISHItuner
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_formats
    PUBLIC
        juce::juce_recommended_config_flags
)

juce_generate_juce_header(AITRAISHItuner)

# ── Tests ─────────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ── Install (manual VST3 copy) ────────────────────────────────────────────────
# Run: cmake --install build/release --prefix "C:/Program Files/Common Files/VST3"
# Requires admin elevation.
install(
    DIRECTORY "$<TARGET_FILE_DIR:AITRAISHItuner_VST3>/../"
    DESTINATION "AITRAISHItuner"
    FILES_MATCHING PATTERN "*.vst3" PATTERN "*.dll"
)
