#include "AST.hpp"

#include "EMLObject.hpp"
#include "Loader.hpp"

#include <EssaUtil/ScopeGuard.hpp>

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

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

std::string Value::string() const {
    return std::visit(
        overloaded {
            [](bool b) -> std::string {
                return b ? "true" : "false";
            },
            [](double d) -> std::string {
                return fmt::format("{}", d);
            },
            [](Util::UString const& str) -> std::string {
                return "\"" + str.encode() + "\"";
            },
            [](Object const&) -> std::string {
                return "@TODO";
            },
            [](Range const& r) -> std::string {
                return fmt::format("{}..{}", r.min, r.max);
            },
            [](auto const&) -> std::string {
                return "???";
            } },
        *this);
}

EMLErrorOr<Value> Object::require_property(std::string const& name) const {
    auto it = properties.find(name);
    if (it == properties.end())
        return EMLError { "Required property not defined: " + name };

    return it->second.value;
}

Value Object::get_property(std::string const& name, Value&& fallback) const {
    auto it = properties.find(name);
    if (it == properties.end())
        return fallback;

    return it->second.value;
}

EMLErrorOr<std::unique_ptr<EMLObject>> Object::construct_impl(EML::Loader& loader) const {
    return loader.construct_object(class_name);
}

EMLErrorOr<void> Object::populate_impl(EML::Loader& loader, EMLObject& constructed_object) const {
    loader.push_scope(this);
    Util::ScopeGuard guard { [&loader]() { loader.pop_scope(); } };

    // 1. Populate native object with EML defined default (`define` declaration)
    auto class_definition = loader.find_class_definition(class_name);
    if (class_definition)
        TRY(constructed_object.load_from_eml_object(class_definition->base, loader));

    // 2. Populate native object using local overrides from this object
    TRY(constructed_object.load_from_eml_object(*this, loader));

    return {};
}

}
