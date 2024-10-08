#pragma once

namespace chowdsp
{
#ifndef DOXYGEN
struct StateValueBase
{
    explicit StateValueBase (std::string_view valueName) : name (valueName) {}
    StateValueBase (const StateValueBase&) = default;
    virtual ~StateValueBase() = default;

    virtual void reset() {}

    virtual void serialize (JSONSerializer::SerializedType&) const {}
    virtual void deserialize (JSONSerializer::DeserializedType) {}

    virtual void serialize (XMLSerializer::SerializedType&) const {}
    virtual void deserialize (XMLSerializer::DeserializedType) {}
    const std::string_view name;
    Broadcaster<void()> changeBroadcaster;
};
#endif

/** A stateful value that can be used to hold some non-parameter state */
template <typename T, typename element_type_ = T>
struct StateValue : StateValueBase
{
    using element_type = element_type_;

    /** Default constructor */
    StateValue()
        : StateValueBase ({}),
          defaultValue (element_type {}),
          currentValue (defaultValue)
    {
    }

    /** Constructs the value with a name and default value */
    StateValue (std::string_view valueName, element_type defaultVal)
        : StateValueBase (valueName),
          defaultValue (defaultVal),
          currentValue (defaultValue)
    {
    }

    StateValue (StateValue&&) noexcept = default;
    StateValue& operator= (StateValue&&) noexcept = default;

    /** Returns the value */
    element_type get() const noexcept { return currentValue; }
    operator element_type() const noexcept { return get(); } // NOSONAR NOLINT(google-explicit-constructor): we want to be able to do implicit conversion

    /** Sets a new value */
    void set (element_type v)
    {
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wfloat-equal")
        if (v == currentValue)
            return;
        JUCE_END_IGNORE_WARNINGS_GCC_LIKE

        currentValue = v;
        changeBroadcaster();
    }

    StateValue& operator= (element_type v)
    {
        set (v);
        return *this;
    }

    /** Resets the value to its default state */
    void reset() override { set (defaultValue); }

    /** XML Serializer */
    void serialize (XMLSerializer::SerializedType& serial) const override
    {
        serialize<XMLSerializer> (serial, *this);
    }

    /** XML Deserializer */
    void deserialize (XMLSerializer::DeserializedType deserial) override
    {
        deserialize<XMLSerializer> (deserial, *this);
    }
    //    /** JSON Serializer */
    //    void serialize (JSONSerializer::SerializedType& serial) const override
    //    {
    //        serialize<JSONSerializer> (serial, *this);
    //    }
    //
    //    /** JSON Deserializer */
    //    void deserialize (JSONSerializer::DeserializedType deserial) override
    //    {
    //        deserialize<JSONSerializer> (deserial, *this);
    //    }

    const element_type defaultValue;

private:
    template <typename Serializer>
    static std::enable_if_t<std::is_same_v<Serializer, XMLSerializer>, void>
        serialize (typename Serializer::SerializedType& serial, const StateValue& value)
    {
        Serializer::addChildElement (serial, toString (value.name), value.get());
    }
    template <typename Serializer>
    static std::enable_if_t<std::is_same_v<Serializer, JSONSerializer>, void>
        serialize (typename Serializer::SerializedType& serial, const StateValue& value)
    {
        Serializer::addChildElement (serial, toString (value.name));
        Serializer::addChildElement (serial, Serialization::serialize<Serializer> (value.get()));
    }

    template <typename Serializer>
    static std::enable_if_t<std::is_same_v<Serializer, JSONSerializer>, void>
        deserialize (typename Serializer::DeserializedType deserial, StateValue& value)
    {
        T val {};
        Serialization::deserialize<Serializer> (deserial, val);
        value.set (val);
    }

    template <typename Serializer>
    static std::enable_if_t<std::is_same_v<Serializer, XMLSerializer>, void>
        deserialize (typename Serializer::DeserializedType deserial, StateValue& value)
    {
        T val {};
        //std::unique_ptr<juce::XmlElement> xmlElementPtr(const_cast<juce::XmlElement*>(deserial));
        Serialization::deserialize<Serializer> (deserial, val);
        value.set (val);
    }
    T currentValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StateValue)
};
} // namespace chowdsp
