#ifndef __TYPES_H__
#define __TYPES_H__

#include <Magnum/Mesh.h>
#include <Magnum/Buffer.h>
#include <Magnum/Texture.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/PhongMaterialData.h>


typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> Object3D;
typedef Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D> Scene3D;

typedef Magnum::ResourceManager<Magnum::Buffer, Magnum::Mesh, Magnum::Texture2D, Magnum::Shaders::Phong, Magnum::Trade::PhongMaterialData> ViewerResourceManager;


#endif//__TYPES_H__
