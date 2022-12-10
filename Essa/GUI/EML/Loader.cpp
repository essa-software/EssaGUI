#include "Loader.hpp"
#include <memory>

namespace EML {

void Loader::push_scope(Scope const* scope) {
    m_scope_stack.push_back(scope);
}

void Loader::pop_scope() {
    m_scope_stack.pop_back();
}

EMLErrorOr<std::unique_ptr<EMLObject>> Loader::construct_object(std::string const& class_name) const {
    auto constructor = lookup_constructor(class_name);
    if (!constructor) {
        return EMLError { "No native constructor found for class `" + class_name + "`" };
    }

    return constructor->construct();
}

ClassDefinition const* Loader::find_class_definition(std::string const& name) const {
    for (auto const& scope : m_scope_stack) {
        auto it = scope->class_definitions.find(name);
        if (it != scope->class_definitions.end())
            return &it->second;
    }
    return nullptr;
}

static std::map<std::string, std::unique_ptr<EMLObjectConstructorBase>> s_constructor_registry;

void Loader::register_constructor(std::string const& name, std::unique_ptr<EMLObjectConstructorBase> constructor) {
    s_constructor_registry.insert({ name, std::move(constructor) });
}

EMLObjectConstructorBase* Loader::lookup_constructor(std::string const& name) const {
    auto it = s_constructor_registry.find(name);
    if (it == s_constructor_registry.end())
        return nullptr;
    return it->second.get();
}

}
