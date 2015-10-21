// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_ASSET_IMPORTER_HPP__
#define SCENEVIEW_ASSET_IMPORTER_HPP__

#include <QString>

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sv {

/**
 * Imports 3D assets (models) from file.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/asset_importer.hpp
 */
class AssetImporter {
  public:
    /**
     * Imports assets from a file.
     *
     * @param fname file name. This can also be a Qt resource specifier (e.g.,
     * ":/assets/model.obj")
     *
     * The following file formats are supported:
     * - All file formats supported by Assimp.
     * - Renderware (.RWX) files.
     *
     * Textures are not currently supported.
     *
     * On a successful import, a new Scene graph resource is created and added
     * to the resource manager. To incorporate the imported asset into an
     * existing scene, call Scene::MakeGroupFromScene() on the existing scene.
     */
    static Scene::Ptr ImportFile(ResourceManager::Ptr resources,
        const QString& fname,
        const QString& resource_name = ResourceManager::kAutoName);
};


}  // namespace sv

#endif  // SCENEVIEW_ASSET_IMPORTER_HPP__
