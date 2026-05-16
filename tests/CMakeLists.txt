cmake_minimum_required(VERSION 3.22)

# ── Catch2 test framework ─────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)

# ── DSP test executable ───────────────────────────────────────────────────────
add_executable(AITRAISHItuner_Tests
    DSP/test_distortion.cpp
    DSP/test_sample_engine.cpp
    DSP/test_metering.cpp
    DSP/test_pitch_ratio.cpp
    Utils/test_math.cpp
    # DSP source files under test (no plugin client, no UI).
    ${CMAKE_SOURCE_DIR}/src/DSP/SampleEngine.cpp
)

target_include_directories(AITRAISHItuner_Tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests   # tests/JuceHeader.h shadows the plugin-generated one
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(AITRAISHItuner_Tests PRIVATE cxx_std_20)

target_link_libraries(AITRAISHItuner_Tests
    PRIVATE
        Catch2::Catch2WithMain
        juce::juce_core
        juce::juce_events
        juce::juce_graphics
        juce::juce_data_structures
        juce::juce_audio_basics
        juce::juce_audio_formats
        juce::juce_dsp
)

target_compile_definitions(AITRAISHItuner_Tests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

include(CTest)
include(Catch)
catch_discover_tests(AITRAISHItuner_Tests)
