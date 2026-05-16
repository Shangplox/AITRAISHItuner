#pragma once
#include <JuceHeader.h>

namespace AIT {

// Thin wrapper around juce::SmoothedValue for block-rate parameter smoothing.
// Set target each block; call getNextValue() once per block for a smooth step.
// For per-sample smoothing, call getNextValue() inside the sample loop instead.
template <typename T = float>
using ParamSmoother = juce::SmoothedValue<T, juce::ValueSmoothingTypes::Linear>;

} // namespace AIT
