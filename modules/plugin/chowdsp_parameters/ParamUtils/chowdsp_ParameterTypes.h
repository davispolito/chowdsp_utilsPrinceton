#pragma once

#if JUCE_MODULE_AVAILABLE_chowdsp_clap_extensions
#include <chowdsp_clap_extensions/chowdsp_clap_extensions.h>
#endif

namespace chowdsp
{
#if ! JUCE_MODULE_AVAILABLE_chowdsp_clap_extensions
namespace ParamUtils
{
    /** Mixin for parameters that recognize some form of modulation. */
    struct ModParameterMixin
    {
        virtual ~ModParameterMixin() = default;

        /** Returns true if this parameter supports monophonic modulation. */
        virtual bool supportsMonophonicModulation() { return false; }

        /** Returns true if this parameter supports polyphonic modulation. */
        virtual bool supportsPolyphonicModulation() { return false; }

        /** Base function for applying monophonic modulation to a parameter. */
        [[maybe_unused]] virtual void applyMonophonicModulation (double /*value*/)
        {
        }

        /** Base function for applying polyphonic modulation to a parameter. */
        [[maybe_unused]] virtual void applyPolyphonicModulation (int32_t /*note_id*/, int16_t /*port_index*/, int16_t /*channel*/, int16_t /*key*/, double /*value*/)
        {
        }
    };
} // namespace ParamUtils
#endif

#ifndef DOXYGEN
#if JUCE_VERSION < 0x070000
// JUCE 6 doesn't have the new juce::ParameterID class, so this is a little workaround.
using ParameterID = juce::String;
#else
using ParameterID = juce::ParameterID;
#endif
#endif

/** Wrapper of juce::AudioParameterFloat that supports monophonic modulation. */
class FloatParameter : public juce::AudioParameterFloat,
                       public ParamUtils::ModParameterMixin
{
public:
    FloatParameter (const ParameterID& parameterID,
                    const juce::String& parameterName,
                    const juce::NormalisableRange<float>& valueRange,
                    float defaultFloatValue,
                    std::atomic<float>* valuePtr,
                    const std::function<void ( float)>& setterFunc,
                    const std::function<juce::String (float)>& valueToTextFunction,
                    std::function<float (const juce::String&)>&& textToValueFunction);

    using Ptr = OptionalPointer<FloatParameter>;

    /** Returns the default value for the parameter. */
    float getDefaultValue() const override { return unsnappedDefault; }

    /** TRUE! */
    bool supportsMonophonicModulation() override { return true; }

    /** Applies monphonic modulation to this parameter. */
    void applyMonophonicModulation (double value) override;

    /** Returns the current parameter value accounting for any modulation that is currently applied. */
    float getCurrentValue() const noexcept;

    /** Returns the current parameter value accounting for any modulation that is currently applied. */
    operator float() const noexcept { return getCurrentValue(); } // NOSONAR, NOLINT(google-explicit-constructor): we want to be able to do implicit conversion here

    /** Print debug info. */
    void printDebug() const
    {
        DBG(paramID + " : " + juce::String(get()));
    }
    std::function<void ( float)> setFunc;
private:
    const float unsnappedDefault;
    const juce::NormalisableRange<float> normalisableRange;

    float modulationAmount = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FloatParameter)
};

/** Wrapper of juce::AudioParameterChoice that does not support modulation. */
class ChoiceParameter : public juce::AudioParameterChoice,
                        public ParamUtils::ModParameterMixin
{
public:
    ChoiceParameter (const ParameterID& parameterID, const juce::String& parameterName, const juce::StringArray& parameterChoices, int defaultItemIndex)
        : juce::AudioParameterChoice (parameterID, parameterName, parameterChoices, defaultItemIndex),
          defaultChoiceIndex (defaultItemIndex)
    {
    }
    void printDebug() const
    {
        DBG(paramID + " : " + juce::String(getIndex())); // Using getIndex() for ChoiceParameter
    }

    using Ptr = OptionalPointer<ChoiceParameter>;

    /** Returns the default value for the parameter. */
    int getDefaultIndex() const noexcept { return defaultChoiceIndex; }

private:
    const int defaultChoiceIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChoiceParameter)
};

/**
 * A Choice parameter based off of an enum class type.
 *
 * By default, underscores in enum names will be replaced with spaces.
 * For custom behaviour, replace the charMap argument with a custom
 * character map.
 */
template <typename EnumType>
class EnumChoiceParameter : public ChoiceParameter
{
public:
    // Ideally we could use any fully specified enum, but since there's no way to enforce
    // that, let's stick to "flags" enums.
    static_assert (magic_enum::detail::is_flags_v<EnumType>, "In order to enforce consistent serialization/deserialization, enum types should be constructed as flags.");

    EnumChoiceParameter (const ParameterID& parameterID,
                         const juce::String& parameterName,
                         EnumType defaultChoice,
                         const std::initializer_list<std::pair<char, char>>& charMap = { { '_', ' ' } })
        : ChoiceParameter (
            parameterID,
            parameterName,
            EnumHelpers::createStringArray<EnumType> (charMap),
            static_cast<int> (*magic_enum::enum_index (defaultChoice)))
    {
    }

    EnumType get() const noexcept
    {
        return magic_enum::enum_value<EnumType> ((size_t) getIndex());
    }
    void printDebug() const
    {
        DBG(paramID + " : " + juce::String(static_cast<int>(get())));
    }

    using Ptr = OptionalPointer<EnumChoiceParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnumChoiceParameter)
};

/** Wrapper of juce::AudioParameterBool that does not support modulation. */
class BoolParameter : public juce::AudioParameterBool,
                      public ParamUtils::ModParameterMixin
{
public:
    BoolParameter (const ParameterID& parameterID, const juce::String& parameterName, bool defaultBoolValue)
        : juce::AudioParameterBool (parameterID, parameterName, defaultBoolValue)
    {
    }
    void printDebug() const
    {
        DBG(paramID + " : " + juce::String(static_cast<int>(get())));
    }
    using Ptr = OptionalPointer<BoolParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoolParameter)
};

/** A float parameter which specifically stores a percentage value. */
class PercentParameter : public FloatParameter
{
public:
    PercentParameter (const ParameterID& parameterID,
                      const juce::String& paramName,
                      std::atomic<float> *valuePtr,
                      const std::function<void (float)>& setterFunc,
                      float defaultValue = 0.5f,
                      bool isBipolar = false)
        : FloatParameter (parameterID,
                          paramName,
                          juce::NormalisableRange { isBipolar ? -1.0f : 0.0f, 1.0f },
                          defaultValue,
                          valuePtr,
                          setterFunc,
                          &ParamUtils::percentValToString,
                          &ParamUtils::stringToPercentVal)
    {
    }

    using Ptr = OptionalPointer<PercentParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PercentParameter)
};

/** A float parameter which specifically stores a gain value in Decibels. */
class GainDBParameter : public FloatParameter
{
public:
    GainDBParameter (const ParameterID& parameterID,
                     const juce::String& paramName,
                     std::atomic<float> *valuePtr,
                     const std::function<void ( float)>& setterFunc,
                     const juce::NormalisableRange<float>& paramRange,
                     float defaultValue)
        : FloatParameter (parameterID,
                          paramName,
                          paramRange,
                          defaultValue,
                          valuePtr,
                          setterFunc,
                          &ParamUtils::gainValToString,
                          &ParamUtils::stringToGainVal)
    {
    }

    using Ptr = OptionalPointer<GainDBParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainDBParameter)
};

/** A float parameter which specifically stores a frequency value in Hertz. */
class FreqHzParameter : public FloatParameter
{
public:
    FreqHzParameter (const ParameterID& parameterID,
                     const juce::String& paramName,
                     std::atomic<float> *valuePtr,
                     const std::function<void (float)>& setterFunc,
                     const juce::NormalisableRange<float>& paramRange,
                     float defaultValue)
        : FloatParameter (parameterID,
                          paramName,
                          paramRange,
                          defaultValue,
                          valuePtr,
                          setterFunc,
                          &ParamUtils::freqValToString,
                          &ParamUtils::stringToFreqVal)
    {
    }

    using Ptr = OptionalPointer<FreqHzParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreqHzParameter)
};

/** A float parameter which specifically stores a time value in milliseconds. */
class TimeMsParameter : public FloatParameter
{
public:
    TimeMsParameter (const ParameterID& parameterID,
                     const juce::String& paramName,
                     std::atomic<float> *valuePtr,
                     const std::function<void ( float)>& setterFunc,
                     const juce::NormalisableRange<float>& paramRange,
                     float defaultValue)
        : FloatParameter (parameterID,
                          paramName,
                          paramRange,
                          defaultValue,
                          valuePtr,
                          setterFunc,
                          &ParamUtils::timeMsValToString,
                          &ParamUtils::stringToTimeMsVal)
    {
    }

    using Ptr = OptionalPointer<TimeMsParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeMsParameter)
};

/** A float parameter which specifically stores a ratio value. */
class RatioParameter : public FloatParameter
{
public:
    RatioParameter (const ParameterID& parameterID,
                    const juce::String& paramName,
                    std::atomic<float> *valuePtr,
                    const std::function<void ( float)>& setterFunc,
                    const juce::NormalisableRange<float>& paramRange,
                    float defaultValue)
        : FloatParameter (parameterID,
                          paramName,
                          paramRange,
                          defaultValue,
                          valuePtr,
                          setterFunc,
                          &ParamUtils::ratioValToString,
                          &ParamUtils::stringToRatioVal)
    {
    }

    using Ptr = OptionalPointer<RatioParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RatioParameter)
};

/** A float parameter which specifically stores a semitones value. */
class SemitonesParameter : public FloatParameter
{
public:
    SemitonesParameter (const ParameterID& parameterID,
                        const juce::String& paramName,
                        std::atomic<float> *valuePtr,
                        const std::function<void ( float)>& setterFunc,
                        juce::NormalisableRange<float> paramRange,
                        float defaultValue,
                        bool snapToInt = false)
        : FloatParameter (
            parameterID,
            paramName,
            (paramRange.interval = snapToInt ? 1.0f : paramRange.interval, paramRange),
            defaultValue,
            valuePtr,
            setterFunc,
            [snapToInt] (float val)
            { return ParamUtils::semitonesValToString (val, snapToInt); },
            &ParamUtils::stringToSemitonesVal)
    {
    }

    using Ptr = OptionalPointer<SemitonesParameter>;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SemitonesParameter)
};
} // namespace chowdsp
