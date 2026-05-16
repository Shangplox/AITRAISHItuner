#pragma once

// Debug-only logging. All macros compile to nothing in Release builds.
// Never call these from the audio thread — they allocate.

#ifdef JUCE_DEBUG
  #include <JuceHeader.h>
  #define AIT_LOG(msg)       DBG("[AITRAISHItuner] " << msg)
  #define AIT_LOG_ERR(msg)   DBG("[AITRAISHItuner ERROR] " << msg)
#else
  #define AIT_LOG(msg)       do {} while(0)
  #define AIT_LOG_ERR(msg)   do {} while(0)
#endif
