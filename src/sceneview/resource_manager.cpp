// Copyright [2015] Albert Huang

#include "sceneview/resource_manager.hpp"
#include "sceneview/scene.hpp"

#if 0
#define dbg(fmt, ...) printf(fmt, __VA_ARGS__)
#else
#define dbg(...)
#endif

namespace sv {

const QString ResourceManager::kAutoName = "";

struct ResourceManager::Priv {
  std::map<QString, MaterialResourceWeakPtr> materials;
  std::map<QString, ShaderResourceWeakPtr> shaders;
  std::map<QString, GeometryResourceWeakPtr> geometries;
  std::map<QString, SceneWeakPtr> scenes;
  std::map<QString, FontResourceWeakPtr> fonts;

  int64_t name_counter;
};

ResourceManager::~ResourceManager() { delete p_; }

ResourceManager::Ptr ResourceManager::Create() {
  return Ptr(new ResourceManager());
}

ResourceManager::ResourceManager() : p_(new Priv) { p_->name_counter = 0; }

MaterialResource::Ptr ResourceManager::MakeMaterial(
    const ShaderResource::Ptr& shader, const QString& name) {
  QString actual_name = PickName(name);
  MaterialResource::Ptr result(new MaterialResource(actual_name, shader));
  p_->materials[actual_name] = result;
  dbg("MakeMaterial: -> %s (total: %d)\n", actual_name.c_str(),
      static_cast<int>(p_->materials.size()));
  return result;
}

ShaderResource::Ptr ResourceManager::MakeShader(const QString& name) {
  QString actual_name = PickName(name);
  ShaderResource::Ptr result(new ShaderResource(actual_name));
  p_->shaders[actual_name] = result;
  dbg("MakeShader: -> %s (total: %d)\n", actual_name.c_str(),
      static_cast<int>(p_->shaders.size()));
  return result;
}

GeometryResource::Ptr ResourceManager::MakeGeometry(const QString& name) {
  QString actual_name = PickName(name);
  GeometryResource::Ptr result(new GeometryResource(actual_name));
  p_->geometries[actual_name] = result;
  dbg("MakeGeometry: -> %s (total: %d)\n", actual_name.c_str(),
      static_cast<int>(p_->geometries.size()));
  return result;
}

Scene::Ptr ResourceManager::MakeScene(const QString& name) {
  QString actual_name = PickName(name);
  Scene::Ptr result(new Scene(actual_name));
  p_->scenes[actual_name] = result;
  dbg("MakeScene: -> %s (total: %d)\n", actual_name.c_str(),
      static_cast<int>(p_->scenes.size()));
  return result;
}

FontResource::Ptr ResourceManager::Font(const QFont& qfont) {
  QString name = qfont.family();
  auto iter = p_->fonts.find(name);
  if (iter != p_->fonts.end()) {
    FontResource::Ptr result = iter->second.lock();
    if (result) {
      return result;
    }
  }
  FontResource::Ptr result = FontResource::Create(qfont);
  p_->fonts[name] = result;
  return result;
}

MaterialResource::Ptr ResourceManager::GetMaterial(const QString& name) {
  auto iter = p_->materials.find(name);
  if (iter == p_->materials.end()) {
    return MaterialResource::Ptr();
  }
  return iter->second.lock();
}

ShaderResource::Ptr ResourceManager::GetShader(const QString& name) {
  auto iter = p_->shaders.find(name);
  if (iter == p_->shaders.end()) {
    return ShaderResource::Ptr();
  }
  return iter->second.lock();
}

GeometryResource::Ptr ResourceManager::GetGeometry(const QString& name) {
  auto iter = p_->geometries.find(name);
  if (iter == p_->geometries.end()) {
    return GeometryResource::Ptr();
  }
  return iter->second.lock();
}

template <typename MapType>
static void ClearExpired(MapType* map) {
  for (auto iter = map->begin(); iter != map->end();) {
    if (iter->second.expired()) {
      map->erase(iter++);
    } else {
      ++iter;
    }
  }
}

void ResourceManager::Cleanup() {
  ClearExpired(&p_->materials);
  ClearExpired(&p_->shaders);
  ClearExpired(&p_->geometries);
  ClearExpired(&p_->scenes);
}

QString ResourceManager::AutogenerateName() {
  QString name;
  do {
    name = "sv_" + QString::number(p_->name_counter);
    p_->name_counter++;
  } while (NameExists(name));
  return name;
}

QString ResourceManager::PickName(const QString& name) {
  if (name == kAutoName) {
    return AutogenerateName();
  } else {
    if (NameExists(name)) {
      throw std::invalid_argument("Duplicate resource name " +
                                  name.toStdString());
    }
    return name;
  }
}

template <typename MapType>
static bool InMap(const MapType& map, const QString& key) {
  auto iter = map.find(key);
  return iter != map.end() && !iter->second.expired();
}

bool ResourceManager::NameExists(const QString& name) {
  return InMap(p_->materials, name) || InMap(p_->geometries, name) ||
         InMap(p_->scenes, name);
}

void ResourceManager::PrintStats() {
  Cleanup();
  printf("materials: %d\n", static_cast<int>(p_->materials.size()));
  printf("shaders: %d\n", static_cast<int>(p_->shaders.size()));
  printf("geometries: %d\n", static_cast<int>(p_->geometries.size()));
  printf("scenes: %d\n", static_cast<int>(p_->scenes.size()));
}

}  // namespace sv
