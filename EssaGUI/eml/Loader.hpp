#pragma once

#include "EMLError.hpp"
#include "EMLObject.hpp"
#include "EssaGUI/eml/AST.hpp"

#include <memory>
#include <stack>

namespace EML {

class EMLObjectConstructorBase {
public:
    virtual ~EMLObjectConstructorBase() = default;
    virtual std::unique_ptr<EMLObject> construct() const = 0;
};

template<class T>
requires(std::is_base_of_v<EMLObject, T>) class EMLObjectConstructor : public EMLObjectConstructorBase {
public:
    virtual std::unique_ptr<EMLObject> construct() const override {
        return std::make_unique<T>();
    }
};

class Loader {
public:
    void push_scope(Scope const* scope);
    void pop_scope();

    EMLErrorOr<std::unique_ptr<EMLObject>> construct_object(std::string const& class_name) const;

    template<class T>
    static void register_class(std::string const& name) {
        register_constructor(name, std::make_unique<EMLObjectConstructor<T>>());
    }

    ClassDefinition const* find_class_definition(std::string const& name) const;

private:
    static void register_constructor(std::string const& name, std::unique_ptr<EMLObjectConstructorBase>);

    EMLObjectConstructorBase* lookup_constructor(std::string const& name) const;

    std::list<Scope const*> m_scope_stack;
};

template<class T>
struct EMLClassRegistration {
    EMLClassRegistration(std::string const& name) {
        EML::Loader::register_class<T>(name);
    }
};

#define EML_REGISTER_CLASS(Type) ::EML::EMLClassRegistration<Type> __register_##Type { #Type };

}
