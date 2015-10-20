// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_IMPORTER_RWX_HPP__
#define SCENEVIEW_IMPORTER_RWX_HPP__

#include <sceneview/scene.hpp>
#include <sceneview/resource_manager.hpp>

namespace sceneview {

/**
 * Imports a model from a .rwx file (Renderware)
 */
Scene::Ptr ImportRwxFile(ResourceManager::Ptr resources,
        const QString& fname,
        const QString& resource_name = ResourceManager::kAutoName);

}  // namespace sceneview

#endif  // SCENEVIEW_IMPORTER_RWX_HPP__
