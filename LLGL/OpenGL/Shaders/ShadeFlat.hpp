#pragma once

#include "../Shader.hpp"

namespace llgl::opengl::shaders {

class ShadeFlat : public Shader {
public:
    ShadeFlat();

    virtual AttributeMapping attribute_mapping() const override;

    void set_light_position(Util::Vector3f lp) { m_light_position = lp; }
    void set_light_color(Util::Colorf lc) { m_light_color = lc; }

private:
    virtual void on_bind(ShaderScope& scope) const override;

    Util::Vector3f m_light_position;
    Util::Colorf m_light_color;
};

}
