// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_RESOURCE_MANAGER_HPP__
#define SCENEVIEW_RESOURCE_MANAGER_HPP__

#include <cstdint>
#include <map>

#include <sceneview/font_resource.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>
#include <sceneview/shader_resource.hpp>
#include <sceneview/scene.hpp>

namespace sv {

class Scene;

/**
 * Central repository for resources.
 *
 * The ResourceManager maintains a repository of all resources (materials,
 * shaders, geometries) and is the only class able to create new resources. The
 * main purpose of the ResourceManager is to act as a point of coordination for
 * sharing resources.
 *
 * Some notes on memory management:
 * - All resources are managed using shared pointers.
 * - The ResourceManager itself maintains a weak reference to each resource.
 * - If you create a resource (e.g., with MakeMaterial()) and do not retain the
 *   pointer, it gets immediately destroyed.
 *
 * @ingroup sv_resources
 * @headerfile sceneview/resource_manager.hpp
 */
class ResourceManager {
  public:
    typedef std::shared_ptr<ResourceManager> Ptr;

    static const QString kAutoName;

  public:
    virtual ~ResourceManager();

    static Ptr Create();

    /**
     * Create a new material.
     *
     * @param shader the shader for the material to use.
     *
     * @throw std::invalid_argument If a resource with the same name already
     * exists.
     */
    MaterialResource::Ptr MakeMaterial(const ShaderResource::Ptr& shader,
        const QString& name = kAutoName);

    /**
     * Create a new shader.
     *
     * @throw std::invalid_argument If a resource with the same name already
     * exists.
     */
    ShaderResource::Ptr MakeShader(const QString& name = kAutoName);

    /**
     * Create a new geometry.
     *
     * @throw std::invalid_argument If a resource with the same name already
     * exists.
     */
    GeometryResource::Ptr MakeGeometry(const QString& name = kAutoName);

    /**
     * Create a new scene graph.
     *
     * @throw std::invalid_argument If a resource with the same name already
     * exists.
     */
    Scene::Ptr MakeScene(const QString& name = kAutoName);

    /**
     * Retrieves a FontResource, creating one if needed.
     */
    FontResource::Ptr Font(const QFont& qfont);

    /**
     * Retrieve the specified material.
     *
     * If the material isn't found, then an empty pointer is returned.
     */
    MaterialResource::Ptr GetMaterial(const QString& name);

    /**
     * Retrieve the specified shader.
     *
     * If the shader isn't found, then an empty pointer is returned.
     */
    ShaderResource::Ptr GetShader(const QString& name);

    /**
     * Retrieve the specified geometry.
     *
     * If the geometry isn't found, then an empty pointer is returned.
     */
    GeometryResource::Ptr GetGeometry(const QString& name);

    /**
     * Debugging
     */
    void PrintStats();

  private:
    typedef std::weak_ptr<MaterialResource> MaterialResourceWeakPtr;
    typedef std::weak_ptr<ShaderResource> ShaderResourceWeakPtr;
    typedef std::weak_ptr<GeometryResource> GeometryResourceWeakPtr;
    typedef std::weak_ptr<Scene> SceneWeakPtr;
    typedef std::weak_ptr<FontResource> FontResourceWeakPtr;

    ResourceManager();

    void Cleanup();

    QString AutogenerateName();
    QString PickName(const QString& name);
    bool NameExists(const QString& name);

    struct Priv;
    Priv* p_;
};

}  // namespace sv

#endif  // SCENEVIEW_RESOURCE_MANAGER_HPP__
