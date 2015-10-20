#include <sceneview/asset_importer.hpp>

#include "importer_assimp.hpp"
#include "importer_rwx.hpp"

namespace sceneview {

Scene::Ptr AssetImporter::ImportFile(ResourceManager::Ptr resources,
    const QString& fname) {
  Scene::Ptr assimp_scene = ImportAssimpFile(resources, fname);
  if (assimp_scene) {
    return assimp_scene;
  }
  Scene::Ptr rwx_scene = ImportRwxFile(resources, fname);
  return rwx_scene;
}

}  // namespace sceneview
