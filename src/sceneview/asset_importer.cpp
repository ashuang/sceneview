// Copyright [2015] Albert Huang

#include "sceneview/asset_importer.hpp"

#include "sceneview/importer_assimp.hpp"
#include "sceneview/importer_rwx.hpp"

namespace sv {

Scene::Ptr AssetImporter::ImportFile(ResourceManager::Ptr resources,
    const QString& fname, const QString& resource_name) {
  Scene::Ptr assimp_scene = ImportAssimpFile(resources, fname, resource_name);
  if (assimp_scene) {
    return assimp_scene;
  }
  Scene::Ptr rwx_scene = ImportRwxFile(resources, fname, resource_name);
  return rwx_scene;
}

}  // namespace sv
