#include "Border.hpp"
#include <cassert>

namespace GUI {

EML::EMLErrorOr<void> Border::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    std::shared_ptr<Widget> widget = TRY(object.require_and_construct_object<Widget>("child", loader, window_root()));
    assert(widget);
    add_created_widget(widget);

    return {};
}

EML_REGISTER_CLASS(Border);

}
