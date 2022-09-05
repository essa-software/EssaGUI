#pragma once

#include "NotifyUser.hpp"
#include "TextEditor.hpp"

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

    Type m_type = NUMBER;

    virtual void on_init() override { set_multiline(false); }

    void set_limit(unsigned limit) { m_limit = limit; }

    void set_data_type(Type type) { m_type = type; }
    void set_min_max_values(double min_value, double max_value) {
        m_min_value = min_value;
        m_max_value = max_value;
        m_has_limit = true;
    }

private:
    void m_fit_in_range();
    bool find_decimal() const;
    std::string m_fix_content(std::string content) const;
    virtual bool can_insert_codepoint(uint32_t ch) const override;
    virtual void on_content_change() override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    virtual LengthVector initial_size() const override { return { Length::Auto, { static_cast<float>(theme().line_height), Length::Unit::Px } }; }

    unsigned m_limit = 1024;
    bool m_has_decimal = false;
    bool m_has_limit = false;
    double m_min_value = std::numeric_limits<double>::lowest();
    double m_max_value = std::numeric_limits<double>::max();
};

}
