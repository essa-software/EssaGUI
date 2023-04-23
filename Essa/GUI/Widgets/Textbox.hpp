#pragma once

#include "TextEditor.hpp"
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <functional>
#include <limits>
#include <string>

namespace GUI {

class Textbox : public TextEditor {
public:
    enum Type {
        TEXT,
        NUMBER
    };

    virtual void on_init() override { set_multiline(false); }

    CREATE_VALUE(double, min, std::numeric_limits<double>::min())
    CREATE_VALUE(double, max, std::numeric_limits<double>::max())
    CREATE_VALUE(size_t, limit, 1024)
    CREATE_VALUE(Type, type, Type::NUMBER)

private:
    void m_fit_in_range();
    bool find_decimal() const;
    std::string m_fix_content(std::string content) const;
    virtual bool can_insert_codepoint(uint32_t ch) const override;
    virtual void on_content_change() override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    virtual LengthVector initial_size() const override { return { Util::Length::Auto, { static_cast<int>(theme().line_height), Util::Length::Px } }; }

    bool m_has_decimal = false;
};

}
