#pragma once

#include "Widget.hpp"

namespace GUI {

class ImageWidget : public Widget {
public:
    CREATE_VALUE(llgl::opengl::Texture const*, image, nullptr)

private:
    virtual void draw(GUI::Window& window) const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
};

}
