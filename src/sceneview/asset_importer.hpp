// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_ASSET_IMPORTER_HPP__
#define SCENEVIEW_ASSET_IMPORTER_HPP__

#include <QString>

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sceneview {

class AssetImporter {
  public:
    /**
     * Imports assets from a file.
     *
     * @param fname file name. This can also be a Qt resource specifier (e.g.,
     * ":/assets/model.obj")
     */
    static Scene::Ptr ImportFile(ResourceManager::Ptr resources,
        const QString& fname);
};


}  // namespace sceneview

#endif  // SCENEVIEW_ASSET_IMPORTER_HPP__
