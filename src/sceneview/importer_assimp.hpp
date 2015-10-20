#ifndef SCENEVIEW_ASSIMP_IMPORTER_HPP__
#define SCENEVIEW_ASSIMP_IMPORTER_HPP__

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sceneview {

/**
 * Imports assets from a file.
 *
 * @param fname file name. This can also be a Qt resource specifier (e.g.,
 * ":/assets/model.obj")
 */
Scene::Ptr ImportAssimpFile(ResourceManager::Ptr resources,
    const QString& fname,
    const QString& scene_name = ResourceManager::kAutoName);

}  // namespace sceneview

#endif  // SCENEVIEW_ASSIMP_IMPORTER_HPP__
