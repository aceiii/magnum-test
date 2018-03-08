#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Context.h>
#include <Magnum/Renderer.h>
#include <Magnum/Version.h>
#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData3D.h>

using namespace Magnum;
using namespace Magnum::Math::Literals;

struct TriangleVertex {
    Vector2 position;
    Color3 color;
};

const TriangleVertex data[] {
    {{-0.5f, -0.5f}, 0xff0000_rgbf},
    {{ 0.5f, -0.5f}, 0x00ff00_rgbf},
    {{ 0.0f,  0.5f}, 0x0000ff_rgbf},
};

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;
        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;

        Buffer _indexBuffer, _vertexBuffer, _triangleBuffer;
        Mesh _mesh, _triangleMesh;
        Shaders::Phong _shader;
        Shaders::VertexColor2D _triangleShader;

        Matrix4 _transformation, _projection;
        Vector2i _previousMousePosition;
        Color3 _color;
};

MyApplication::MyApplication(const Arguments& arguments)
    : Platform::Application{arguments, Configuration {}.setTitle("Triangles!!1")}
{
    using namespace Magnum::Math::Literals;

    Debug {} << "Hello! This application is running on" << Context::current().version()
             << "using" << Context::current().rendererString();

    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::FaceCulling);
    Renderer::setClearColor(0xa5c9ea_rgbf);

    _triangleBuffer.setData(data, BufferUsage::StaticDraw);

    _triangleMesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(3)
        .addVertexBuffer(_triangleBuffer, 0,
            Shaders::VertexColor2D::Position {},
            Shaders::VertexColor2D::Color {Shaders::VertexColor2D::Color::Components::Three});

    const Trade::MeshData3D cube = Primitives::Cube::solid();

    _vertexBuffer.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)), BufferUsage::StaticDraw);

    Containers::Array<char> indexData;
    Mesh::IndexType indexType;
    UnsignedInt indexStart, indexEnd;

    std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(cube.indices());

    _indexBuffer.setData(indexData, BufferUsage::StaticDraw);

    _mesh.setPrimitive(cube.primitive())
        .setCount(cube.indices().size())
        .addVertexBuffer(_vertexBuffer, 0,
                Shaders::Phong::Position {}, Shaders::Phong::Normal {})
        .setIndexBuffer(_indexBuffer, 0, indexType, indexStart, indexEnd);

    _transformation = Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);
    _color = Color3::fromHsv(35.0_degf, 1.0f, 1.0f);

    _projection = Matrix4::perspectiveProjection(35.0_degf, Vector2 {defaultFramebuffer.viewport().size()}.aspectRatio(), 0.01f, 100.0f) * Matrix4::translation(Vector3::zAxis(-10.0f));
}

void MyApplication::drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Color | FramebufferClear::Depth);

    _shader.setLightPosition({7.0f, 5.0f, 2.5f})
        .setLightColor(Color3 {1.0f})
        .setDiffuseColor(_color)
        .setAmbientColor(Color3::fromHsv(_color.hue(), 1.0f, 0.3f))
        .setTransformationMatrix(_transformation)
        .setNormalMatrix(_transformation.rotationScaling())
        .setProjectionMatrix(_projection);

    _mesh.draw(_shader);

    _triangleMesh.draw(_triangleShader);

    swapBuffers();
}

void MyApplication::mousePressEvent(MouseEvent& event) {
    if (event.button() != MouseEvent::Button::Left) {
        return;
    }

    _previousMousePosition = event.position();
    event.setAccepted();
}

void MyApplication::mouseReleaseEvent(MouseEvent& event) {
    _color = Color3::fromHsv(_color.hue() + 50.0_degf, 1.0f, 1.0f);

    event.setAccepted();
    redraw();
}

void MyApplication::mouseMoveEvent(MouseMoveEvent& event) {
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) {
        return;
    }

    const Vector2 delta = 3.0f * Vector2 {event.position() - _previousMousePosition} / Vector2 {defaultFramebuffer.viewport().size()};

    _transformation = Matrix4::rotationX(Rad {delta.y()}) * _transformation * Matrix4::rotationY(Rad {delta.x()});
    _previousMousePosition = event.position();

    event.setAccepted();
    redraw();
}

MAGNUM_APPLICATION_MAIN(MyApplication)
