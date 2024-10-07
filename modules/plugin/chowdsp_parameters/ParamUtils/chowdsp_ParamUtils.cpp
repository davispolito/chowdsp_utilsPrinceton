#include "chowdsp_ParamUtils.h"

namespace chowdsp::ParamUtils
{
//void createFreqParameter (Parameters& params, const ParameterID& id, const juce::String& name, float min, float max, float centre, float defaultValue,
//                          std::atomic<float> *valuePtr)
//{
//    auto freqRange = createNormalisableRange (min, max, centre);
//    emplace_param<FreqHzParameter> (params, id, name, valuePtr, freqRange, defaultValue);
//}
//
//void createPercentParameter (Parameters& params, const ParameterID& id, const juce::String& name, float defaultValue,  std::atomic<float> *valuePtr)
//{
//    emplace_param<PercentParameter> (params, id, name, valuePtr, defaultValue);
//}
//
//void createBipolarPercentParameter (Parameters& params, const ParameterID& id, const juce::String& name, float defaultValue,  std::atomic<float> *valuePtr)
//{
//    emplace_param<PercentParameter> (params, id, name, valuePtr, defaultValue, true);
//}
//
//void createGainDBParameter (Parameters& params, const ParameterID& id, const juce::String& name, float min, float max, float defaultValue,  std::atomic<float> *valuePtr, float centerValue)
//{
//    juce::NormalisableRange range { min, max };
//    if (centerValue > -1000.0f)
//        range.setSkewForCentre (centerValue);
//
//    emplace_param<GainDBParameter> (params, id, name, valuePtr, range, defaultValue);
//}
//
//void createTimeMsParameter (Parameters& params, const ParameterID& id, const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue,  std::atomic<float> *valuePtr)
//{
//    emplace_param<TimeMsParameter> (params, id, name, valuePtr, range, defaultValue);
//}
//
//void createRatioParameter (Parameters& params, const ParameterID& id, const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue,  std::atomic<float> *valuePtr)
//{
//    emplace_param<RatioParameter> (params, id, name, valuePtr, range, defaultValue);
//}
} // namespace chowdsp::ParamUtils
