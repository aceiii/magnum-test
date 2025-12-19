#ifndef __TEXTURED_TRIANGLE_SHADER_H__
#define __TEXTURED_TRIANGLE_SHADER_H__

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Types.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>


class TexturedTriangleShader: public Magnum::GL::AbstractShaderProgram {
public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

    explicit TexturedTriangleShader();

    TexturedTriangleShader& setColor(const Magnum::Color3& color) {
        setUniform(_colorUniform, color);
        return *this;
    }

    TexturedTriangleShader& bindTexture(Magnum::GL::Texture2D& texture) {
        texture.bind(TextureLayer);
        return *this;
    }

private:
    enum: Magnum::Int { TextureLayer = 0 };

    Magnum::Int _colorUniform;
};



#endif//__TEXTURED_TRIANGLE_SHADER_H__
