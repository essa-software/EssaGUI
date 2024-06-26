#include "AST.hpp"

#include "EMLObject.hpp"
#include "Loader.hpp"

#include <EssaUtil/Config.hpp>
#include <EssaUtil/ScopeGuard.hpp>
#include <EssaUtil/UStringBuilder.hpp>
#include <fmt/format.h>

namespace EML {

static std::string indent(size_t d) {
    std::string str;
    for (size_t s = 0; s < d; s++)
        str += "    ";
    return str;
}

void Scope::print(size_t depth) const {
    for (auto const& object : objects) {
        fmt::print("{}", indent(depth));
        object.print(depth);
    }
    for (auto const& class_definition : class_definitions) {
        class_definition.second.print(depth);
    }
}

void Object::print(size_t depth) const {
    fmt::print("@{}", class_name);
    if (!id.empty()) {
        fmt::print(" {}", id);
    }
    fmt::print(" {{\n");
    for (auto const& property : properties) {
        property.second.print(depth + 1);
    }
    fmt::print("\n");
    Scope::print(depth + 1);
    fmt::print("{}}}\n\n", indent(depth));
}

void ClassDefinition::print(size_t depth) const {
    fmt::print("{}define {} : ", indent(depth), class_name);
    base.print(depth);
}

void Property::print(size_t depth) const {
    auto value_string = value.string();
    fmt::print("{}{}: {}\n", indent(depth), name, value_string);
}

std::string Value::string() const {
    return std::visit(
        Util::Overloaded {
            [](double d) -> std::string { return fmt::format("{}", d); },
            [](bool b) -> std::string { return b ? "true" : "false"; },
            [](Util::UString const& str) -> std::string { return "\"" + str.encode() + "\""; },
            [](Object const&) -> std::string { return "@TODO"; },
            [](Util::Length const& length) -> std::string {
                switch (length.unit()) {
                case Util::Length::Initial:
                    return "initial";
                case Util::Length::Auto:
                    return "auto";
                case Util::Length::Px:
                    return fmt::format("{}px", length.value());
                case Util::Length::Percent:
                    return fmt::format("{}%", length.value());
                }
                ESSA_UNREACHABLE;
            },
            [](Gfx::ResourceId const& id) -> std::string {
                switch (id.type()) {
                case Gfx::ResourceId::Type::Asset:
                    return fmt::format("asset({})", id.path());
                case Gfx::ResourceId::Type::External:
                    return fmt::format("external({})", id.path());
                }
                ESSA_UNREACHABLE;
            },
            [](Range const& r) -> std::string { return fmt::format("{}..{}", r.min, r.max); },
            [](Array const& array) -> std::string {
                Util::UStringBuilder builder;
                builder.append("[");
                size_t s = 0;
                for (auto const& value : array.values()) {
                    builder.append(Util::UString { value.string() });
                    if (s != array.values().size() - 1) {
                        builder.append(", ");
                    }
                    s++;
                }
                builder.append("]");
                return builder.release_string().encode();
            },
            [](Util::Color const& color) -> std::string { return color.to_html_string().encode(); },
        },
        *this
    );
}

Array::Array(std::vector<Value> values)
    : m_values(std::move(values)) { }

Value Array::at(size_t index) const {
    return m_values[index];
}

EMLErrorOr<Value> Object::require_property(std::string const& name) const {
    auto it = properties.find(name);
    if (it == properties.end())
        return EMLError { "Required property '" + name + "' not defined for type " + class_name };

    return it->second.value;
}

Value Object::get_property(std::string const& name, Value&& fallback) const {
    auto it = properties.find(name);
    if (it == properties.end())
        return fallback;

    return it->second.value;
}

EMLErrorOr<std::unique_ptr<EMLObject>> Object::construct_impl(EML::Loader& loader) const {
    // 1. Try to construct the object with specified class name
    auto object = EML::Loader::construct_object(class_name);
    if (!object.is_error())
        return object.release_value();

    // 2. If this is a `define`d object, allow constructing it
    //    from base without C++ wrapper defined for this exact
    //    type. E.g if there is `define EssaSplash : @ToolWindow`
    //    and EssaSplash isn't registered, construct ToolWindow.
    auto const* class_definition = loader.find_class_definition(class_name);
    if (class_definition) {
        return class_definition->base.construct_impl(loader);
    }
    return object.release_error();
}

EMLErrorOr<void> Object::populate_impl(EML::Loader& loader, EMLObject& constructed_object) const {
    loader.push_scope(this);
    Util::ScopeGuard guard { [&loader]() { loader.pop_scope(); } };

    Object object_to_load = *this;

    // 1. Load EML defined defaults into this object (`define` declaration)
    auto const* class_definition = loader.find_class_definition(class_name);
    if (class_definition)
        object_to_load.merge(class_definition->base);

    // 2. Populate native object using the merged objects
    TRY(constructed_object.load_from_eml_object(object_to_load, loader));

    return {};
}

void Object::merge(Object const& other) {
    for (auto const& property : other.properties) {
        properties.insert_or_assign(property.first, property.second);
    }
}

}
