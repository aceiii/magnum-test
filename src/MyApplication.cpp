#include <Magnum/DefaultFramebuffer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Context.h>
#include <Magnum/Renderer.h>
#include <Magnum/Version.h>
#include <Magnum/Texture.h>
#include <Magnum/Buffer.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/TextureFormat.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/ObjectData3D.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>

#include "TexturedTriangleShader.h"
#include "Types.h"
#include "ColoredObject.h"
#include "TexturedObject.h"
#include "configure.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class MyApplication: public Platform::Application {
    public:
        explicit MyApplication(const Arguments& arguments);

    private:
        void viewportEvent(const Vector2i &size) override;
        void drawEvent() override;
        void mousePressEvent(MouseEvent &event) override;
        void mouseReleaseEvent(MouseEvent &event) override;
        void mouseMoveEvent(MouseMoveEvent &event) override;
        void mouseScrollEvent(MouseScrollEvent &event) override;

        Vector3 positionOnSphere(const Vector2i &_position) const;

        void addObject(Trade::AbstractImporter &importer, Object3D *parent, UnsignedInt i);

        ViewerResourceManager _resourceManager;

        Scene3D _scene;
        Object3D *_object, *_cameraObject;
        SceneGraph::Camera3D *_camera;
        SceneGraph::DrawableGroup3D _drawables;
        Vector3 _previousPosition;

        Buffer _indexBuffer, _vertexBuffer, _triangleBuffer;
        Mesh _mesh, _triangleMesh;
        Shaders::Phong _shader;
        TexturedTriangleShader _triangleShader;
        Texture2D _texture;

        Matrix4 _transformation, _projection;
        Vector2i _previousMousePosition;
        Color3 _color;
};

MyApplication::MyApplication(const Arguments& arguments)
    : Platform::Application{arguments, Configuration {}.setTitle("Triangles!!1")}
{
    CORRADE_PLUGIN_IMPORT(TgaImporter);
    CORRADE_PLUGIN_IMPORT(AnySceneImporter);
    CORRADE_PLUGIN_IMPORT(OpenGexImporter);

    Utility::Arguments args;
    args.addArgument("file").setHelp("file", "file to load")
        .setHelp("Loads and displays 3D scene file (such as OpenGEX or "
                 "COLLADA one) provided on the command-line.")
        .parse(arguments.argc, arguments.argv);

    using namespace Magnum::Math::Literals;

    struct TriangleVertex {
        Vector2 position;
        Vector2 textureCoordinates;
        Color3 color;
    };

    const TriangleVertex data[] {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}, 0xff0000_rgbf},
        {{ 0.5f, -0.5f}, {1.0f, 0.0f}, 0x00ff00_rgbf},
        {{ 0.0f,  0.5f}, {0.5f, 1.0f}, 0x0000ff_rgbf},
    };

    Debug {} << "Hello! This application is running on" << Context::current().version()
             << "using" << Context::current().rendererString();

    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::FaceCulling);
    Renderer::setClearColor(0xa5c9ea_rgbf);

    _triangleBuffer.setData(data, BufferUsage::StaticDraw);

    _triangleMesh.setPrimitive(MeshPrimitive::Triangles)
        .setCount(3)
        .addVertexBuffer(_triangleBuffer, 0,
            TexturedTriangleShader::Position {},
            TexturedTriangleShader::TextureCoordinates {},
            TexturedTriangleShader::Color {TexturedTriangleShader::Color::Components::Three});

    PluginManager::Manager<Trade::AbstractImporter> manager {MAGNUM_PLUGINS_IMPORTER_DIR};
    std::unique_ptr<Trade::AbstractImporter> tgaImporter = manager.loadAndInstantiate("TgaImporter");
    if (!tgaImporter) {
        std::exit(1);
    }

    const Utility::Resource rs {"textured-triangle-data"};
    if(!tgaImporter->openData(rs.getRaw("stone.tga"))) {
        std::exit(2);
    }

    Containers::Optional<Trade::ImageData2D> image = tgaImporter->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);

    _texture.setWrapping(Sampler::Wrapping::ClampToEdge)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setMinificationFilter(Sampler::Filter::Linear)
        .setStorage(1, TextureFormat::RGB8, image->size())
        .setSubImage(0, {}, *image);

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


    _resourceManager
        .set("color", new Shaders::Phong)
        .set("texture", new Shaders::Phong {Shaders::Phong::Flag::DiffuseTexture});

    auto material = new Trade::PhongMaterialData {{}, 50.0f};
    material->ambientColor() = 0x000000_rgbf;
    material->diffuseColor() = 0xe5e5e5_rgbf;
    material->specularColor() = 0xffffff_rgbf;

    _resourceManager
        .setFallback(material)
        .setFallback(new Texture2D)
        .setFallback(new Mesh);

    auto &cameraObject = *(_cameraObject = new Object3D {&_scene});
    cameraObject.translate(Vector3::zAxis(5.0f));

    auto &camera = *(_camera = new SceneGraph::Camera3D {*_cameraObject});
    camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01, 10.0f))
        .setViewport(defaultFramebuffer.viewport().size());

    std::unique_ptr<Trade::AbstractImporter> sceneImporter = manager.loadAndInstantiate("AnySceneImporter");
    if (!sceneImporter) {
        std::exit(3);
    }

    Debug {} << "Opening file" << args.value("file");

    if (!sceneImporter->openFile(args.value("file"))) {
        std::exit(4);
    }

    for (UnsignedInt i = 0; i != sceneImporter->materialCount(); i += 1) {
        Debug {} << "Importing material" << i << sceneImporter->materialName(i);

        std::unique_ptr<Trade::AbstractMaterialData> materialData = sceneImporter->material(i);
        if (!materialData || materialData->type() != Trade::MaterialType::Phong) {
            Warning {} << "Cannot load material, skipping";
            continue;
        }

        _resourceManager.set(ResourceKey {i}, static_cast<Trade::PhongMaterialData*>(materialData.release()));
    }

    for (UnsignedInt i = 0; i != sceneImporter->textureCount(); i += 1) {
        Debug {} << "Importing texture" << i << sceneImporter->textureName(i);

        Containers::Optional<Trade::TextureData> textureData = sceneImporter->texture(i);
        if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
            Warning {} << "Cannot load texture, skipping";
            continue;
        }

        Debug {} << "Importing image" << textureData->image() << sceneImporter->image2DName(textureData->image());

        Containers::Optional<Trade::ImageData2D> imageData = sceneImporter->image2D(textureData->image());
        if (!imageData || (imageData->format() != PixelFormat::RGB
#ifndef MAGNUM_TARGET_GLES
            && imageData->format() != PixelFormat::BGR
#endif
        )) {
            Warning {} << "Cannot load texture image, skipping";
            continue;
        }

        auto texture = new Texture2D;
        texture->setMagnificationFilter(textureData->magnificationFilter())
            .setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
            .setWrapping(textureData->wrapping().xy())
            .setStorage(1, TextureFormat::RGB8, imageData->size())
            .setSubImage(0, {}, *imageData)
            .generateMipmap();

        _resourceManager.set(ResourceKey {i}, texture, ResourceDataState::Final, ResourcePolicy::Manual);
    }

    for (UnsignedInt i = 0; i != sceneImporter->mesh3DCount(); i += 1) {
        Debug {} << "Importing mesh" << i << sceneImporter->mesh3DName(i);

        Containers::Optional<Trade::MeshData3D> meshData = sceneImporter->mesh3D(i);
        if (!meshData || !meshData->hasNormals() || meshData->primitive() != MeshPrimitive::Triangles) {
            Warning {} << "Cannot load mesh, skipping";
            continue;
        }

        Mesh mesh {NoCreate};
        std::unique_ptr<Buffer> buffer, indexBuffer;
        std::tie(mesh, buffer, indexBuffer) = MeshTools::compile(*meshData, BufferUsage::StaticDraw);

        _resourceManager.set(ResourceKey {i}, new Mesh {std::move(mesh)}, ResourceDataState::Final, ResourcePolicy::Manual);
        _resourceManager.set(std::to_string(i) + "-vertices", buffer.release(), ResourceDataState::Final, ResourcePolicy::Manual);

        if (indexBuffer) {
            _resourceManager.set(std::to_string(i) + "-indices", indexBuffer.release(), ResourceDataState::Final, ResourcePolicy::Manual);
        }
    }

    _object = new Object3D {&_scene};

    if (sceneImporter->defaultScene() != -1) {
        Debug {} << "Adding default scene" << sceneImporter->sceneName(sceneImporter->defaultScene());

        Containers::Optional<Trade::SceneData> sceneData = sceneImporter->scene(sceneImporter->defaultScene());
        if (!sceneData) {
            Error {} << "Cannot load scene, exiting";
            return;
        }

        for (UnsignedInt objectId : sceneData->children3D()) {
            addObject(*sceneImporter, _object, objectId);
        }
    } else if (_resourceManager.state<Mesh>(ResourceKey {0}) == ResourceState::Final) {
        new ColoredObject {ResourceKey {0}, ResourceKey(-1), _object, &_drawables};
    }

    _resourceManager.setFallback<Trade::PhongMaterialData>(nullptr)
        .clear<Trade::PhongMaterialData>()
        .free<Texture2D>()
        .free<Mesh>();
}

void MyApplication::drawEvent() {
    defaultFramebuffer.clear(FramebufferClear::Color | FramebufferClear::Depth);

    /*
    _shader.setLightPosition({7.0f, 5.0f, 2.5f})
        .setLightColor(Color3 {1.0f})
        .setDiffuseColor(_color)
        .setAmbientColor(Color3::fromHsv(_color.hue(), 1.0f, 0.3f))
        .setTransformationMatrix(_transformation)
        .setNormalMatrix(_transformation.rotationScaling())
        .setProjectionMatrix(_projection);

    _mesh.draw(_shader);

    //_triangleShader.setColor(0xffb2b2_rgbf)
    _triangleShader.bindTexture(_texture);

    _triangleMesh.draw(_triangleShader);
    */

    _camera->draw(_drawables);

    swapBuffers();
}

void MyApplication::mousePressEvent(MouseEvent& event) {
    /*
    if (event.button() != MouseEvent::Button::Left) {
        return;
    }

    _previousMousePosition = event.position();
    event.setAccepted();
    */

    if (event.button() == MouseEvent::Button::Left) {
        _previousPosition = positionOnSphere(event.position());
    }
}

void MyApplication::mouseReleaseEvent(MouseEvent& event) {
    /*
    _color = Color3::fromHsv(_color.hue() + 50.0_degf, 1.0f, 1.0f);

    event.setAccepted();
    redraw();
    */

    if (event.button() == MouseEvent::Button::Left) {
        _previousPosition = Vector3();
    }
}

void MyApplication::mouseMoveEvent(MouseMoveEvent& event) {
    /*
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) {
        return;
    }

    const Vector2 delta = 3.0f * Vector2 {event.position() - _previousMousePosition} / Vector2 {defaultFramebuffer.viewport().size()};

    _transformation = Matrix4::rotationX(Rad {delta.y()}) * _transformation * Matrix4::rotationY(Rad {delta.x()});
    _previousMousePosition = event.position();

    event.setAccepted();
    redraw();
    */

    if (!(event.buttons() & MouseMoveEvent::Button::Left)) {
        return;
    }

    Vector3 currentPosition = positionOnSphere(event.position());

    Vector3 axis = Math::cross(_previousPosition, currentPosition);

    if (_previousPosition.length() < 0.001f || axis.length() < 0.001f) {
        return;
    }

    _object->rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());

    _previousPosition = currentPosition;

    redraw();
}

void MyApplication::viewportEvent(const Vector2i &size) {
    defaultFramebuffer.setViewport({{}, size});
    _camera->setViewport(size);
}

void MyApplication::mouseScrollEvent(MouseScrollEvent &event) {
    if (!event.offset().y()) {
        return;
    }

    Float distance = _cameraObject->transformation().translation().z();

    distance *= 1 - (event.offset().y() > 0 ? 1.0 / 0.85f : 0.85f);
    _cameraObject->translate(Vector3::zAxis(distance));

    redraw();
}

Vector3 MyApplication::positionOnSphere(const Vector2i &position) const {
    Vector2 positionNormalized = Vector2(position * 2) / Vector2(_camera->viewport()) - Vector2(1.0f);

    Float length = positionNormalized.length();
    Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));

    result.y() *= -1.0f;

    return result.normalized();
}

void MyApplication::addObject(Trade::AbstractImporter &importer, Object3D *parent, UnsignedInt i) {
    Debug {} << "Importing object" << i << importer.object3DName(i);

    Object3D *object = nullptr;
    std::unique_ptr<Trade::ObjectData3D> objectData = importer.object3D(i);
    if (!objectData) {
        Error {} << "Cannot import object, skipping";
        return;
    }

    if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh) {
        Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

        auto materialData = _resourceManager.get<Trade::PhongMaterialData>(ResourceKey(materialId));

        if (!materialData->flags()) {
            object = new ColoredObject(ResourceKey(objectData->instance()),
                    ResourceKey(materialId),
                    parent, &_drawables);
            object->setTransformation(objectData->transformation());
        } else if (materialData->flags() == Trade::PhongMaterialData::Flag::DiffuseTexture) {
            object = new TexturedObject(ResourceKey(objectData->instance()),
                    ResourceKey(materialId),
                    ResourceKey(materialData->diffuseTexture()),
                    parent, &_drawables);
            object->setTransformation(objectData->transformation());
        }
    }

    if (!object && !objectData->children().empty()) {
        object = new Object3D(parent);
    }

    for (std::size_t id : objectData->children()) {
        addObject(importer, object, id);
    }
}

MAGNUM_APPLICATION_MAIN(MyApplication)
