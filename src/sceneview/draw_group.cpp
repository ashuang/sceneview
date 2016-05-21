#include "draw_group.hpp"

namespace sv {

DrawGroup::DrawGroup(const QString& name, int order) :
    name_(name), order_(order) {}

void DrawGroup::AddNode(DrawNode* node) {
  nodes_.insert(node);
}

void DrawGroup::RemoveNode(DrawNode* node) {
  nodes_.erase(node);
}

}  // namespace sv
