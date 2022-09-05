#pragma once

#include "Widget.hpp"

namespace GUI {

class ImageWidget : public Widget {
public:
    void set_image(llgl::opengl::Texture const& image) {
        m_image = &image;
    }

private:
    virtual void draw(GUI::Window& window) const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    llgl::opengl::Texture const* m_image = nullptr;
};

}
