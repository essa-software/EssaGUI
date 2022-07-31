#pragma once

#include "../Shader.hpp"

namespace llgl::opengl::shaders
{

class Basic330Core : public Shader
{
public:
    Basic330Core();

    virtual AttributeMapping attribute_mapping() const override;
    
private:
    virtual void on_bind(ShaderScope&) const override;
};

}
