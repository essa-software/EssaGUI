#pragma once

#include "Widget.hpp"

namespace GUI {

class ImageWidget : public Widget {
public:
    CREATE_VALUE(llgl::Texture const*, image, nullptr)

private:
    virtual void draw(Gfx::Painter& window) const override;
    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;
};

}
