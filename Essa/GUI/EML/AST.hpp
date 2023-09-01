#pragma once

#include "EMLError.hpp"
#include "EMLObject.hpp"

#include <Essa/GUI/Graphics/ResourceManager.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Length.hpp>
#include <EssaUtil/UString.hpp>
#include <cstdint>
#include <list>
#include <variant>

namespace EML {

struct ClassDefinition;
struct Property;
struct Object;

struct Scope {
    std::list<Object> objects;
    std::map<std::string, ClassDefinition> class_definitions;

    void print(size_t depth) const;
};

class EMLObject;
class Loader;
class Value;

struct Object : public Scope {
    std::string class_name;
    std::string id;
    std::map<std::string, Property> properties;

    void print(size_t depth) const;

    EMLErrorOr<Value> require_property(std::string const& name) const;
    Value get_property(std::string const& name, Value&& fallback) const;

    template<Util::Enum T> using FromStringFunction = std::optional<T>(std::string_view);

    template<Util::Enum T> EMLErrorOr<T> get_enum(std::string const& name, FromStringFunction<T> from_string, T fallback) const;

    template<Util::Enum T> EMLErrorOr<T> require_enum(std::string const& name, FromStringFunction<T> from_string) const;

    template<class T, class... Args>
    EMLErrorOr<std::unique_ptr<T>> require_and_construct_object(std::string const& name, Loader& loader, Args&&... args) const;

    template<class T, class... Args>
        requires(sizeof...(Args) == 0) || requires(T t, Args... args) { t.eml_construct(args...); }
    EMLErrorOr<std::unique_ptr<T>> construct(EML::Loader& loader, Args&&... args) const {
        auto object = TRY(construct_impl(loader)).release();
        // MANUAL MEMORY MANAGEMENT OF object STARTS HERE
        std::unique_ptr<T> specific_object { dynamic_cast<T*>(object) };
        if (!specific_object) {
            delete object;
            return EMLError { "Cannot convert " + class_name + " object to " + typeid(T).name() };
        }
        // and ENDS HERE
        if constexpr (requires() { specific_object->eml_construct(std::forward<Args>(args)...); }) {
            specific_object->eml_construct(std::forward<Args>(args)...);
        }
        TRY(populate_impl(loader, *specific_object));
        return specific_object;
    }

    void merge(Object const& other);

private:
    EMLErrorOr<std::unique_ptr<EMLObject>> construct_impl(EML::Loader&) const;
    EMLErrorOr<void> populate_impl(EML::Loader&, EMLObject& constructed_object) const;
};

struct Range {
    double min;
    double max;
};

class Array {
public:
    explicit Array(std::vector<Value> values);

    Value at(size_t index) const;
    std::vector<Value> const& values() const { return m_values; }

    template<class T, size_t S> EMLErrorOr<std::array<T, S>> to_static() const;

private:
    std::vector<Value> m_values;
};

using ValueVariant = std::variant<double, bool, Util::UString, Object, Util::Length, Gfx::ResourceId, Range, Array, Util::Color>;

#define VALUE_TYPE(Type, camel_case)                                                                              \
    explicit Value(Type v)                                                                                        \
        : ValueVariant(std::move(v)) { }                                                                          \
                                                                                                                  \
    EMLErrorOr<Type> to_##camel_case() const {                                                                    \
        if (!is_##camel_case())                                                                                   \
            return EMLError { "Failed to convert " + string() + " to " #camel_case };                             \
        return std::get<Type>(*this);                                                                             \
    }                                                                                                             \
    Type as_##camel_case() const { return std::get<Type>(*this); }                                                \
    Type as_##camel_case##_or(Type&& fallback) const { return is_##camel_case() ? as_##camel_case() : fallback; } \
    bool is_##camel_case() const { return std::holds_alternative<Type>(*this); }

class Value : public ValueVariant {
public:
    VALUE_TYPE(double, double)
    VALUE_TYPE(bool, bool)
    VALUE_TYPE(Util::UString, string)
    VALUE_TYPE(Object, object)
    VALUE_TYPE(Util::Length, length)
    VALUE_TYPE(Gfx::ResourceId, resource_id)
    VALUE_TYPE(Range, range)
    VALUE_TYPE(Array, array)
    VALUE_TYPE(Util::Color, color)

    // Normally C++ would convert this to bool, because... pointer ??
    Value(char const* str)
        : Value(Util::UString { str }) { }

    template<class T> EMLErrorOr<T> to() const {
        if (!std::holds_alternative<T>(*this))
            return EMLError { "Invalid type" };
        return std::get<T>(*this);
    }

    std::string string() const;
};

template<class T, size_t S> EMLErrorOr<std::array<T, S>> Array::to_static() const {
    std::array<T, S> result;
    if (m_values.size() != S)
        return EMLError { fmt::format("Array of size {} required", S) };
    for (size_t s = 0; s < S; s++) {
        result[s] = TRY(m_values[s].to<T>());
    }
    return result;
}

template<class T, class... Args>
EMLErrorOr<std::unique_ptr<T>> Object::require_and_construct_object(std::string const& name, Loader& loader, Args&&... args) const {
    // This awkward "if err != nil" error handling is because
    // of a -Werror=free-nonheap-object warning (GCC bug?)
    auto maybe_object = TRY(require_property(name)).to_object();
    if (maybe_object.is_error()) {
        return maybe_object.release_error();
    }
    return maybe_object.release_value().construct<T>(loader, std::forward<Args>(args)...);
}

template<Util::Enum T> EMLErrorOr<T> Object::get_enum(std::string const& name, FromStringFunction<T> from_string, T fallback) const {
    if (!properties.contains(name)) {
        return fallback;
    }
    return require_enum(name, from_string);
}

template<Util::Enum T> EMLErrorOr<T> Object::require_enum(std::string const& name, FromStringFunction<T> from_string) const {
    auto object = TRY(require_property(name));
    if (!object.is_string()) {
        // FIXME: Print which enum exactly caused the eror
        return EMLError { fmt::format("Value for property '{}: {}' must be a string, {} given", name, typeid(T).name(), object.string()) };
    }
    auto value = from_string(object.as_string().encode());
    if (!value) {
        return EMLError {
            fmt::format("Invalid enum constant for property '{}: {}': ", name, typeid(T).name(), object.as_string().encode())
        };
    }
    return *value;
}

struct ClassDefinition {
    std::string class_name;
    Object base;

    void print(size_t depth) const;
};

using Declaration = std::variant<ClassDefinition, Property, Object>;

struct Property {
    std::string name;
    Value value;

    void print(size_t depth) const;
};

}
