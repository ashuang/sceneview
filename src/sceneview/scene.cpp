// Copyright [2015] Albert Huang

#include "sceneview/scene.hpp"

#include <cassert>
#include <deque>
#include <vector>

#include "sceneview/camera_node.hpp"
#include "sceneview/group_node.hpp"
#include "sceneview/light_node.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/scene_node.hpp"
#include "sceneview/stock_resources.hpp"

namespace sv {

const QString Scene::kAutoName = "";

Scene::Scene(const QString& name) :
  scene_name_(name),
  root_node_(new GroupNode("root")),
  name_counter_(0),
  lights_(),
  draw_nodes_(),
  cameras_(),
  nodes_({{ root_node_->Name(), root_node_}}) {
}

Scene::~Scene() {
  for (auto& item : nodes_) {
    delete item.second;
  }
}

bool Scene::ContainsNode(SceneNode* node) const {
  for (SceneNode* iter = node; iter; iter = iter->ParentNode()) {
    if (iter == root_node_) {
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

DrawNode* Scene::MakeDrawNode(GroupNode* parent, const QString& name) {
  const QString actual_name = PickName(name);
  DrawNode* node = new DrawNode(actual_name);
  if (parent) {
    parent->AddChild(node);
  }
  draw_nodes_.push_back(node);
  nodes_[actual_name] = node;
  return node;
}

DrawNode* Scene::MakeDrawNode(GroupNode* parent,
        const GeometryResource::Ptr& geometry,
        const MaterialResource::Ptr& material,
        const QString& name) {
  DrawNode* node = MakeDrawNode(parent, name);
  node->Add(geometry, material);
  return node;
}

void Scene::DestroyNode(SceneNode* node) {
  assert(node != root_node_);
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
    case SceneNodeType::kDrawNode:
      {
        DrawNode* draw_node = dynamic_cast<DrawNode*>(node);
        draw_nodes_.erase(std::find(draw_nodes_.begin(), draw_nodes_.end(),
              draw_node));
      }
      break;
  }
  node->ParentNode()->RemoveChild(node);
  delete node;
}

void Scene::PrintStats() {
  std::deque<GroupNode*> to_count = { root_node_ };
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

  printf("nodes: %d\n", static_cast<int>(num_nodes));
  printf("nodes in map: %d\n", static_cast<int>(nodes_.size()));
}

QString Scene::AutogenerateName() {
  QString name;
  do {
    name = "sv_" + QString::number(name_counter_);
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

}  // namespace sv
