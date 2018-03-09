#ifndef __TEXTURED_TRIANGLE_SHADER_H__
#define __TEXTURED_TRIANGLE_SHADER_H__

#include <Magnum/Context.h>
#include <Magnum/Texture.h>
#include <Magnum/Version.h>
#include <Magnum/Shader.h>
#include <Magnum/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>


class TexturedTriangleShader : public Magnum::AbstractShaderProgram {
public:

    typedef Magnum::Attribute<0, Magnum::Vector2> Position;
    typedef Magnum::Attribute<1, Magnum::Vector2> TextureCoordinates;
    typedef Magnum::Attribute<2, Magnum::Color3> Color;

    explicit TexturedTriangleShader();

    /*
    TexturedTriangleShader& setColor(const Magnum::Color3 &color) {
        setUniform(_colorUniform, color);
        return *this;
    }
    */

    TexturedTriangleShader& bindTexture(Magnum::Texture2D &texture) {
        texture.bind(TextureLayer);
        return *this;
    }

private:
    enum: Magnum::Int { TextureLayer = 0 };

    //Magnum::Int _colorUniform;
};


#endif//__TEXTURED_TRIANGLE_SHADER_H__
