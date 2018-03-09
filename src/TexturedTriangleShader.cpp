#include "TexturedTriangleShader.h"
#include <Corrade/PluginManager/Manager.h>

using namespace Magnum;

TexturedTriangleShader::TexturedTriangleShader() {
    MAGNUM_ASSERT_VERSION_SUPPORTED(Version::GL330);

    const Utility::Resource rs {"textured-triangle-data"};

    Shader vert {Version::GL330, Shader::Type::Vertex};
    Shader frag {Version::GL330, Shader::Type::Fragment};

    vert.addSource(rs.get("TexturedTriangleShader.vert"));
    frag.addSource(rs.get("TexturedTriangleShader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    //_colorUniform = uniformLocation("color");

    setUniform(uniformLocation("textureData"), TextureLayer);
}

