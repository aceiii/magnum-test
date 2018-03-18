#include "TexturedObject.h"

using namespace Magnum;

TexturedObject::TexturedObject(Magnum::ResourceKey meshId, Magnum::ResourceKey materialId, Magnum::ResourceKey diffuseTextureId, Object3D *parent, Magnum::SceneGraph::DrawableGroup3D *group):
    Object3D {parent}, SceneGraph::Drawable3D {*this, group},
    _mesh {ViewerResourceManager::instance().get<Mesh>(meshId)},
    _diffuseTexture {ViewerResourceManager::instance().get<Texture2D>(diffuseTextureId)},
    _shader {ViewerResourceManager::instance().get<Shaders::Phong>("texture")}
{
    auto material = ViewerResourceManager::instance().get<Trade::PhongMaterialData>(materialId);
    _ambientColor = material->ambientColor();
    _specularColor = material->specularColor();
    _shininess = material->shininess();
}

void TexturedObject::draw(const Magnum::Matrix4 &transformationMatrix, Magnum::SceneGraph::Camera3D &camera) {
    _shader->setAmbientColor(_ambientColor)
        .bindDiffuseTexture(*_diffuseTexture)
        .setSpecularColor(_specularColor)
        .setShininess(_shininess)
        .setLightPosition(camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}))
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.rotation())
        .setProjectionMatrix(camera.projectionMatrix());

    _mesh->draw(*_shader);
}
