#include <sceneview/scene.hpp>

#include <cassert>
#include <deque>

#include <sceneview/stock_resources.hpp>

namespace sceneview {

const QString Scene::kAutoName = "";

Scene::Scene(const QString& name) :
  scene_name_(name),
  root_node_("root"),
  name_counter_(0),
  lights_(),
  meshes_(),
  cameras_(),
  nodes_({{ root_node_.Name(), &root_node_}}) {
}

Scene::~Scene() {
  for (auto& item : nodes_) {
    if (item.second != &root_node_) {
      delete item.second;
    }
  }
}

bool Scene::ContainsNode(SceneNode* node) const {
  for (SceneNode* iter = node; iter; iter = node->ParentNode()) {
    if (iter == &root_node_) {
      return true;
    }
  }
  return false;
}

GroupNode* Scene::MakeGroup(GroupNode* parent,
    const QString& name) {
  const QString actual_name = PickName(name);
  GroupNode* node = new GroupNode(actual_name);
  if (parent) {
    parent->AddChild(node);
  }
  nodes_[actual_name] = node;
  return node;
}

GroupNode* Scene::MakeGroupFromScene(GroupNode* parent,
    Scene::Ptr scene,
    const QString& name) {
  if (scene.get() == this) {
    throw std::invalid_argument("Scene cannot copy itself.");
  }
  GroupNode* node = MakeGroup(parent, name);
  node->CopyAsChildren(this, scene->Root());
  return node;
}

CameraNode* Scene::MakeCamera(GroupNode* parent,
    const QString& name) {
  const QString actual_name = PickName(name);
  CameraNode* camera = new CameraNode(actual_name);
  if (parent) {
    parent->AddChild(camera);
  }
  cameras_.push_back(camera);
  nodes_[actual_name] = camera;
  return camera;
}

LightNode* Scene::MakeLight(GroupNode* parent,
    const QString& name) {
  const QString actual_name = PickName(name);
  LightNode* light = new LightNode(actual_name);
  if (parent) {
    parent->AddChild(light);
  }
  lights_.push_back(light);
  nodes_[actual_name] = light;
  return light;
}

MeshNode* Scene::MakeMesh(GroupNode* parent, const QString& name) {
  const QString actual_name = PickName(name);
  MeshNode* node = new MeshNode(actual_name);
  if (parent) {
    parent->AddChild(node);
  }
  meshes_.push_back(node);
  nodes_[actual_name] = node;
  return node;
}

void Scene::DestroyNode(SceneNode* node) {
  assert(node != &root_node_);
  nodes_.erase(node->Name());
  switch (node->NodeType()) {
    case SceneNodeType::kGroupNode:
      {
        GroupNode* group = dynamic_cast<GroupNode*>(node);
        std::vector<SceneNode*> children = group->Children();
        for (SceneNode* child : children) {
          DestroyNode(child);
        }
      }
      break;
    case SceneNodeType::kCameraNode:
      {
        CameraNode* camera = dynamic_cast<CameraNode*>(node);
        cameras_.erase(std::find(cameras_.begin(), cameras_.end(), camera));
      }
      break;
    case SceneNodeType::kLightNode:
      {
        LightNode* light = dynamic_cast<LightNode*>(node);
        lights_.erase(std::find(lights_.begin(), lights_.end(), light));
      }
      break;
    case SceneNodeType::kMeshNode:
      {
        MeshNode* mesh = dynamic_cast<MeshNode*>(node);
        meshes_.erase(std::find(meshes_.begin(), meshes_.end(), mesh));
      }
      break;
  }
  node->ParentNode()->RemoveChild(node);
  delete node;
}

void Scene::PrintStats() {
  std::deque<GroupNode*> to_count = { &root_node_ };
  int num_nodes = 1;
  while (!to_count.empty()) {
    GroupNode* node = to_count.front();
    to_count.pop_front();

    for (SceneNode* child : node->Children()) {
      num_nodes++;
      if (child->NodeType() == SceneNodeType::kGroupNode) {
        to_count.push_back(dynamic_cast<GroupNode*>(child));
      }
    }
  }

  printf("nodes: %d\n", (int)num_nodes);
  printf("nodes in map: %d\n", (int)nodes_.size());
}

QString Scene::AutogenerateName() {
  QString name;
  do {
    name = "b3_" + QString::number(name_counter_);
    name_counter_++;
  } while (nodes_.find(name) != nodes_.end());
  return name;
}

QString Scene::PickName(const QString& name) {
  if (name == kAutoName) {
    return AutogenerateName();
  } else {
    if (nodes_.find(name) != nodes_.end()) {
      throw std::invalid_argument("Duplicate node name " + name.toStdString());
    }
    return name;
  }
}

}  // namespace sceneview
