# Windows / MSVC compiler settings for the plugin target.
# Call: apply_compiler_options(YourTarget)

function(apply_compiler_options TARGET)
    target_compile_features(${TARGET} PRIVATE cxx_std_20)

    if (MSVC)
        target_compile_options(${TARGET} PRIVATE
            /W3             # moderate warnings
            /wd4996         # suppress deprecated CRT warnings from JUCE internals
            /MP             # parallel compilation
            /fp:fast        # fast floating-point (safe for audio DSP)
            /Zc:__cplusplus # correctly report C++ version to __cplusplus macro
        )
        target_compile_definitions(${TARGET} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX        # prevent Windows.h min/max macros clobbering std::min/max
            WIN32_LEAN_AND_MEAN
        )
    endif()
endfunction()
