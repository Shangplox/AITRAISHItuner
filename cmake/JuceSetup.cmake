# Fetches JUCE 8 via CMake FetchContent.
# Pin this tag to a known stable JUCE 8 release.
# Update GIT_TAG when upgrading JUCE — verify the new tag on github.com/juce-framework/JUCE/releases.

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   FALSE
)

FetchContent_MakeAvailable(JUCE)
