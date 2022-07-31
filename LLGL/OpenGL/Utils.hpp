#pragma once

#include <EssaUtil/Color.hpp>
#include <EssaUtil/EnumBits.hpp>
#include <EssaUtil/Rect.hpp>
#include <GL/gl.h>

namespace llgl::opengl {

void set_clear_color(Util::Color color = Util::Colors::Black);

enum class ClearMask {
    Color = GL_COLOR_BUFFER_BIT,
    Depth = GL_DEPTH_BUFFER_BIT
};
ESSA_ENUM_BITWISE_OPERATORS(ClearMask)
void clear(ClearMask);
void clear_enabled();

enum class Feature {
    Texture2D = GL_TEXTURE_2D,
    ScissorTest = GL_SCISSOR_TEST,
    DepthTest = GL_DEPTH_TEST,
    Blend = GL_BLEND
};
void enable(Feature);
void disable(Feature);
void ensure_enabled(Feature);
bool is_enabled(Feature);

class FeatureEnabler {
public:
    FeatureEnabler(Feature feature)
        : m_feature(feature)
    {
        ensure_enabled(feature);
    }

    ~FeatureEnabler() { disable(m_feature); }

private:
    Feature m_feature;
};

void set_scissor(Util::Recti);

}
