#pragma once

#include "ArrowButton.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace GUI {

class SelectWidget : public Container {
public:
    ~SelectWidget();
    virtual void on_init() override;

    struct SelectOption {
        std::string name;
        Util::UString label;
    };

    void add_option(SelectOption option);
    void set_selected_index(unsigned idx, NotifyUser = NotifyUser::Yes);
    unsigned selected_index() const { return m_selected_index; }

    std::function<void(unsigned)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    std::shared_ptr<Container> m_elements_container;

    Textbox* m_textbox = nullptr;

    std::vector<SelectOption> m_options;
    unsigned m_selected_index = 0;
    bool m_expanded = false;
    HostWindow* m_select_overlay = nullptr;
};

}
