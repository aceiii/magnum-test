#ifndef __TEXTURE_OBJECT_H__
#define __TEXTURE_OBJECT_H__

#include <Magnum/Mesh.h>
#include <Magnum/Resource.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Phong.h>

#include "Types.h"


class TexturedObject : public Object3D, Magnum::SceneGraph::Drawable3D {
    using ResourceKey = Magnum::ResourceKey;
    using Matrix4 = Magnum::Matrix4;
    using Vector3 = Magnum::Vector3;
    using Float = Magnum::Float;

public:
    explicit TexturedObject(Magnum::ResourceKey meshId, Magnum::ResourceKey materialId, Magnum::ResourceKey diffuseTextureId, Object3D *parent, Magnum::SceneGraph::DrawableGroup3D *group);

private:
    void draw(const Magnum::Matrix4 &transformationMatrix, Magnum::SceneGraph::Camera3D &camera) override;

    Magnum::Resource<Magnum::Mesh> _mesh;
    Magnum::Resource<Magnum::Texture2D> _diffuseTexture;
    Magnum::Resource<Magnum::Shaders::Phong> _shader;

    Vector3 _ambientColor;
    Vector3 _specularColor;
    Float _shininess;

};


#endif//__TEXTURE_OBJECT_H__
