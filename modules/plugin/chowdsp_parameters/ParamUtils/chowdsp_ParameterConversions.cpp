#include "chowdsp_ParameterConversions.h"

namespace chowdsp::ParamUtils
{
juce::String freqValToString (float freqVal)
{
    if (freqVal <= 1000.0f)
        return juce::String (freqVal, 2, false) + " Hz";

    return juce::String (freqVal / 1000.0f, 2, false) + " kHz";
}

float stringToFreqVal (const juce::String& s)
{
    auto freqVal = s.getFloatValue();

    if (s.getLastCharacter() == 'k' || s.endsWith ("kHz") || s.endsWith ("khz"))
        freqVal *= 1000.0f;

    return freqVal;
}
static inline float mtof(float f)
{
    if (f <= -1500.0f) return(0);
    else if (f > 1499.0f) return(mtof(1499.0f));
    else return (8.17579891564f * expf(0.0577622650f * f));
}
juce::String midiValToFreqString (float midiVal)
{
    float freqVal = mtof(midiVal);
    if (freqVal <= 1000.0f)
        return juce::String (freqVal, 2, false) + " Hz";

    return juce::String (freqVal / 1000.0f, 2, false) + " kHz";
}

inline float ftom(float f)
{
    return (f > 0 ? 17.3123405046f * logf(.12231220585f * f) : -1500.0f);
}
float freqStringToMidiVal (const juce::String& s)
{
    auto freqVal = s.getFloatValue();

    if (s.getLastCharacter() == 'k' || s.endsWith ("kHz") || s.endsWith ("khz"))
        freqVal *= 1000.0f;

    return ftom(freqVal);
}

juce::String percentValToString (float percentVal)
{
    auto percentStr = juce::String (int (percentVal * 100.0f));
    return percentStr + "%";
}

float stringToPercentVal (const juce::String& s) { return s.getFloatValue() / 100.0f; }

juce::String gainValToString (float gainVal)
{
    auto gainStr = juce::String (gainVal, 2, false);
    return gainStr + " dB";
}

float stringToGainVal (const juce::String& s) { return s.getFloatValue(); }

juce::String ratioValToString (float ratioVal)
{
    auto ratioStr = juce::String (ratioVal, 2, false);
    return ratioStr + " : 1";
}

float stringToRatioVal (const juce::String& s) { return s.getFloatValue(); }

juce::String timeMsValToString (float timeMsVal)
{
    if (timeMsVal < 1000.0f)
        return juce::String (timeMsVal, 2, false) + " ms";

    auto timeSecStr = juce::String (timeMsVal / 1000.0f, 2, false);
    return timeSecStr + " s";
}

float stringToTimeMsVal (const juce::String& s) { return s.getFloatValue(); }

juce::String semitonesValToString (float semitonesVal, bool snapToInt)
{
    auto semitonesStr = snapToInt
                            ? juce::String (static_cast<int> (semitonesVal)) + " st"
                            : juce::String (semitonesVal, 2, false) + " st";
    if (semitonesVal > 0.0f)
        semitonesStr = "+" + semitonesStr;
    return semitonesStr;
}

float stringToSemitonesVal (const juce::String& s) { return s.getFloatValue(); }

juce::String floatValToString (float floatVal)
{
    return floatValToStringDecimal<2> (floatVal);
}

float stringToFloatVal (const juce::String& s) { return s.getFloatValue(); }
} // namespace chowdsp::ParamUtils
