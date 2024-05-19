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

    void clear_options();
    void add_option(SelectOption option);
    void set_selected_index(std::optional<unsigned> idx, NotifyUser = NotifyUser::Yes);
    std::optional<unsigned> selected_index() const {
        return m_selected_index;
    }

    // Enable searching/filtering options by typing into the textbox.
    void set_search_enabled(bool enabled);

    std::function<void(std::optional<unsigned>)> on_change;

private:
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    void show_or_render_select_overlay();
    void render_select_overlay();

    Textbox* m_textbox = nullptr;

    std::vector<SelectOption> m_options;
    std::optional<unsigned> m_selected_index = 0;
    bool m_expanded = false;
    HostWindow* m_select_overlay = nullptr;
    bool m_search_enabled = false;
};

}
