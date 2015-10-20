#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

//#define dbg(fmt, ...) printf(fmt, __VA_ARGS__)
#define dbg(...)

namespace sceneview {

const QString ResourceManager::kAutoName = "";

ResourceManager::Ptr ResourceManager::Create() {
  return Ptr(new ResourceManager());
}

ResourceManager::ResourceManager() :
  name_counter_(0) {}

MaterialResource::Ptr ResourceManager::MakeMaterial(const ShaderResource::Ptr& shader,
    const QString& name) {
  QString actual_name = PickName(name);
  MaterialResource::Ptr result(new MaterialResource(actual_name, shader));
  materials_[actual_name] = result;
  dbg("MakeMaterial: -> %s (total: %d)\n",
      actual_name.c_str(), (int)materials_.size());
  return result;
}

ShaderResource::Ptr ResourceManager::MakeShader(const QString& name) {
  QString actual_name = PickName(name);
  ShaderResource::Ptr result(new ShaderResource(actual_name));
  shaders_[actual_name] = result;
  dbg("MakeShader: -> %s (total: %d)\n",
      actual_name.c_str(), (int)shaders_.size());
  return result;
}

GeometryResource::Ptr ResourceManager::MakeGeometry(const QString& name) {
  QString actual_name = PickName(name);
  GeometryResource::Ptr result(new GeometryResource(actual_name));
  geometries_[actual_name] = result;
  dbg("MakeGeometry: -> %s (total: %d)\n",
      actual_name.c_str(), (int)geometries_.size());
  return result;
}

Scene::Ptr ResourceManager::MakeScene(const QString& name) {
  QString actual_name = PickName(name);
  Scene::Ptr result(new Scene(actual_name));
  scenes_[actual_name] = result;
  dbg("MakeScene: -> %s (total: %d)\n",
      actual_name.c_str(), (int)scenes_.size());
  return result;
}

FontResource::Ptr ResourceManager::Font(const QFont& qfont) {
  QString name = qfont.family();
  auto iter = fonts_.find(name);
  if (iter != fonts_.end()) {
    return iter->second.lock();
  }
  FontResource::Ptr result = FontResource::Create(qfont);
  fonts_[name] = result;
  return result;
}

MaterialResource::Ptr ResourceManager::GetMaterial(const QString& name) {
  auto iter = materials_.find(name);
  if (iter == materials_.end()) {
    return MaterialResource::Ptr();
  }
  return iter->second.lock();
}

ShaderResource::Ptr ResourceManager::GetShader(const QString& name) {
  auto iter = shaders_.find(name);
  if (iter == shaders_.end()) {
    return ShaderResource::Ptr();
  }
  return iter->second.lock();
}

GeometryResource::Ptr ResourceManager::GetGeometry(const QString& name) {
  auto iter = geometries_.find(name);
  if (iter == geometries_.end()) {
    return GeometryResource::Ptr();
  }
  return iter->second.lock();
}

template<typename MapType>
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
  ClearExpired(&materials_);
  ClearExpired(&shaders_);
  ClearExpired(&geometries_);
  ClearExpired(&scenes_);
}

QString ResourceManager::AutogenerateName() {
  QString name;
  do {
    name = "b3_" + QString::number(name_counter_);
    name_counter_++;
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

template<typename MapType>
static bool InMap(const MapType& map, const QString& key) {
  auto iter = map.find(key);
  return iter != map.end() && !iter->second.expired();
}

bool ResourceManager::NameExists(const QString& name) {
  return InMap(materials_, name) ||
    InMap(geometries_, name) ||
    InMap(scenes_, name);
}

void ResourceManager::PrintStats() {
  Cleanup();
  printf("materials: %d\n", (int)materials_.size());
  printf("shaders: %d\n", (int)shaders_.size());
  printf("geometries: %d\n", (int)geometries_.size());
  printf("scenes: %d\n", (int)scenes_.size());
}

}  // namespace sceneview
