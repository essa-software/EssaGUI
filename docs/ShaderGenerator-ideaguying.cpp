// lighting shader which takes light pos, light color and vertex pos+tex coord+normal

struct MyLightingShader : public llgl::Shader {
    llgl::Shader::Uniform<Vector3f> light_pos;
    llgl::Shader::Uniform<Color> light_color;
    llgl::Shader::UniformOptionalSampler2D<0> texture;

    using Vertex = llgl::VertexFormat<llgl::Vertex::Position::XYZ, llgl::Vertex::TexCoord::UV, llgl::Vertex::Normal::XYZ>;
    
    constexpr MyLightingShader() {
        add(light_pos);
        add(light_color);
        add(texture);
        add(position);
        add(tex_coord);
        add(normal);

        auto vert = vertex_shader().main();
        vert->set(Result::Position, Function::TransformLocalToClip{vertex().position});

        auto frag = fragment_shader().main();
        // lighting(light_pos, diffuse, vertex)
        auto lighting = Function::Lighting{light_pos, light_color, vertex()} * texture.sample_or(tex_coord, Util::Colors::White);
        frag->set(Result::FragColor, lighting);
    }
}


llgl::VertexArray<> vertex_array {
    {0,0,0}, {0, 0}, {0,0,0}
};

MyLightingShader shader;

vertex_array.draw(DrawState().shader(shader).texture(0, some_texture).transform(model, view, projection));
