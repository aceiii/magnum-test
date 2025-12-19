#include <memory>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Containers.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>

#include "TexturedTriangleShader.h"


namespace Magnum {
    namespace Examples {
        class TexturedTriangleExample: public Platform::Application {
        public:
            virtual ~TexturedTriangleExample() = default;

            explicit TexturedTriangleExample(const Arguments& arguments);

        private:
            void drawEvent() override;

            GL::Buffer _buffer;
            GL::Mesh _mesh;
            TexturedTriangleShader _shader;
            GL::Texture2D _texture;
        };

        TexturedTriangleExample::TexturedTriangleExample(const Arguments& arguments):
            Platform::Application{arguments, Configuration{}.setTitle("Magnum Textured Triangle Example")}
        {
            struct TriangleVertex {
                Vector2 position;
                Vector2 textureCoordinates;
                Vector3 color;
            };
            constexpr TriangleVertex data[]{
                {{-0.5f, -0.5f}, {0.0f, 0.0f}, { 0.9453125f, 0.83203125f, 0.61328125f }}, /* Left vertex position and texture coordinate */
                {{ 0.5f, -0.5f}, {1.0f, 0.0f}, { 0.8359375f, 0.91796875f, 0.921875f }}, /* Right vertex position and texture coordinate */
                {{ 0.0f,  0.5f}, {0.5f, 1.0f}, { 0.8671875f, 0.11328125f, 0.11328125f }}  /* Top vertex position and texture coordinate */
            };

            _buffer.setData(data, GL::BufferUsage::StaticDraw);
            _mesh.setPrimitive(GL::MeshPrimitive::Triangles)
                .setCount(3)
                .addVertexBuffer(_buffer, 0,
                    TexturedTriangleShader::Position{},
                    TexturedTriangleShader::TextureCoordinates{},
                    TexturedTriangleShader::Color{});

            PluginManager::Manager<Trade::AbstractImporter> manager;
            Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("TgaImporter");
            if(!importer) std::exit(1);

            const Utility::Resource rs{"textured-triangle-data"};
            if(!importer->openData(rs.getRaw("stone.tga")))
                std::exit(2);

            Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);

            CORRADE_INTERNAL_ASSERT(image);
            _texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setMinificationFilter(GL::SamplerFilter::Linear)
                .setStorage(1, GL::TextureFormat::RGB8, image->size())
                .setSubImage(0, {}, *image);
        }

        void TexturedTriangleExample::drawEvent() {
            GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

            using namespace Math::Literals;

            _shader.setColor(0xffffff_rgbf)
                .bindTexture(_texture);
            _shader.draw(_mesh);

            swapBuffers();
        }
    }
}

MAGNUM_APPLICATION_MAIN(Magnum::Examples::TexturedTriangleExample)
