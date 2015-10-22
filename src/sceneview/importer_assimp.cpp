// Copyright [2015] Albert Huang

#include "importer_assimp.hpp"

#include <cassert>
#include <deque>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "sceneview/group_node.hpp"
#include "sceneview/draw_node.hpp"
#include "sceneview/stock_resources.hpp"

#if 0
#define dbg(...) printf(__VA_ARGS__)
#else
#define dbg(...)
#endif

namespace sv {

namespace {

struct AssimpMaterial {
  explicit AssimpMaterial(const aiMaterial& mat);

  void Print() const;

  std::vector<float> diffuse;
  bool have_diffuse;

  std::vector<float> specular;
  bool have_specular;

  std::vector<float> ambient;
  bool have_ambient;

  std::vector<float> emissive;
  bool have_emissive;

  std::vector<float> transparent;
  bool have_transparent;

  bool wireframe;

  bool two_sided;

  int shading_model;

  int blend_func;

  float opacity;

  float shininess;

  float shininess_strength;

  float index_of_refraction;

  // TODO(albert) add texture fields
};

static bool LoadColor(const aiMaterial& mat,
    const char* key, unsigned int type, unsigned int index,
    std::vector<float> *result) {
  aiColor3D ai_color;
  const bool success = mat.Get(key, type, index, ai_color) == aiReturn_SUCCESS;
  (*result)[0] = ai_color.r;
  (*result)[1] = ai_color.g;
  (*result)[2] = ai_color.b;
  return success;
}

AssimpMaterial::AssimpMaterial(const aiMaterial& mat) :
  diffuse({0, 0, 0}),
  specular({0, 0, 0}),
  ambient({0, 0, 0}),
  emissive({0, 0, 0}),
  transparent({0, 0, 0}),
  wireframe(false),
  two_sided(false) {
  have_diffuse = LoadColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
  have_specular = LoadColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
  have_ambient = LoadColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambient);
  have_emissive = LoadColor(mat, AI_MATKEY_COLOR_EMISSIVE, &emissive);
  have_transparent = LoadColor(mat, AI_MATKEY_COLOR_TRANSPARENT, &transparent);

  int wireframe_int;
  const bool have_wireframe =
    mat.Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe_int);
  if (have_wireframe) {
    wireframe = wireframe_int != 0;
  }

  int two_sided_int;
  mat.Get(AI_MATKEY_TWOSIDED, two_sided_int);
  two_sided = two_sided_int != 0;

  mat.Get(AI_MATKEY_SHADING_MODEL, shading_model);

  mat.Get(AI_MATKEY_BLEND_FUNC, blend_func);

  mat.Get(AI_MATKEY_OPACITY, opacity);

  mat.Get(AI_MATKEY_SHININESS, shininess);

  mat.Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);

  mat.Get(AI_MATKEY_REFRACTI, index_of_refraction);
}

void AssimpMaterial::Print() const {
  printf("   diffuse (%d): <%.3f, %.3f, %.3f>\n",
      have_diffuse, diffuse[0], diffuse[1], diffuse[2]);
  printf("   specular (%d): <%.3f, %.3f, %.3f>\n",
      have_specular, specular[0], specular[1], specular[2]);
  printf("   ambient (%d): <%.3f, %.3f, %.3f>\n",
      have_ambient, ambient[0], ambient[1], ambient[2]);
  printf("   emissive (%d): <%.3f, %.3f, %.3f>\n",
      have_emissive, emissive[0], emissive[1], emissive[2]);
  printf("  transparent (%d): <%.3f, %.3f, %.3f>\n",
      have_transparent, transparent[0], transparent[1], transparent[2]);
  printf("  wireframe: %d\n", wireframe);
  printf("  two sided: %d\n", two_sided);
  printf("  shading model: %d\n", shading_model);
  printf("  blend func: %d\n", blend_func);
  printf("  opacity: %f\n", opacity);
  printf("  shininess: %f\n", shininess);
  printf("  shininess strength: %f\n", shininess_strength);
  printf("  index of refraction: %f\n", index_of_refraction);
}

class Importer {
  public:
    explicit Importer(ResourceManager::Ptr resources);

    Scene::Ptr ImportFile(const QString& fname, const QString& scene_name);

  private:
    ResourceManager::Ptr resources_;
    const struct aiScene* ai_scene_;
    std::vector<MaterialResource::Ptr> materials_;
    std::vector<GeometryResource::Ptr> geometries_;
    std::map<GeometryResource::Ptr, MaterialResource::Ptr>
      geometry_materials_;
};

Importer::Importer(ResourceManager::Ptr resources) : resources_(resources) {
}

Scene::Ptr Importer::ImportFile(const QString& fname,
    const QString& scene_name) {
  Assimp::Importer importer;
  ai_scene_ = importer.ReadFile(fname.toStdString(),
      aiProcess_Triangulate |
      aiProcess_GenUVCoords |
      aiProcess_FindInvalidData |
      aiProcess_JoinIdenticalVertices |
      aiProcess_SortByPType |
      aiProcess_SplitLargeMeshes |
      aiProcess_GenNormals |
      aiProcess_FixInfacingNormals |
      aiProcess_OptimizeMeshes |
      aiProcess_OptimizeGraph);

  if (!ai_scene_) {
    return nullptr;
  }

  Scene::Ptr model = resources_->MakeScene(scene_name);

  StockResources stock(resources_);

  // Add materials
  for (size_t mat_index = 0; mat_index < ai_scene_->mNumMaterials;
      ++mat_index) {
    const AssimpMaterial ai_mat(*ai_scene_->mMaterials[mat_index]);

#if dbg
    dbg("material: %d\n", static_cast<int>(mat_index));
    ai_mat.Print();
#endif

    MaterialResource::Ptr material =
      stock.NewMaterial(StockResources::kUniformColorLighting);

    material->SetParam("diffuse",
        ai_mat.diffuse[0],
        ai_mat.diffuse[1],
        ai_mat.diffuse[2],
        ai_mat.opacity);
    material->SetParam("specular",
        ai_mat.specular[0],
        ai_mat.specular[1],
        ai_mat.specular[2],
        ai_mat.opacity);
//    material->SetParam("ambient", ai_mat.ambient);
    material->SetParam("shininess",
        ai_mat.shininess * ai_mat.shininess_strength);

    material->SetTwoSided(ai_mat.two_sided);

    materials_.push_back(material);
  }

  // TODO(albert) add textures

  // Add meshes
  for (size_t mesh_index = 0; mesh_index < ai_scene_->mNumMeshes;
      ++mesh_index) {
    const aiMesh* mesh = ai_scene_->mMeshes[mesh_index];

    dbg("Loading mesh %d / %d\n", static_cast<int>(mesh_index),
        static_cast<int>(ai_scene_->mNumMeshes));

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
      dbg("Skipping mesh %d - not of type TRIANGLE\n",
          static_cast<int>(mesh_index));
      continue;
    }

    // Add vertices and normal vectors
    GeometryData gdata;
    gdata.gl_mode = GL_TRIANGLES;
    for (size_t vert_ind = 0; vert_ind < mesh->mNumVertices; ++vert_ind) {
      const aiVector3D& ai_vertex = mesh->mVertices[vert_ind];
      gdata.vertices.emplace_back(ai_vertex.x, ai_vertex.y, ai_vertex.z);
      const aiVector3D& ai_normal = mesh->mNormals[vert_ind];
      gdata.normals.emplace_back(ai_normal.x, ai_normal.y, ai_normal.z);
    }

    // Add faces
    for (size_t face_ind = 0; face_ind < mesh->mNumFaces; ++face_ind) {
      const aiFace& ai_face = mesh->mFaces[face_ind];
      assert(ai_face.mNumIndices == 3);
      gdata.indices.push_back(ai_face.mIndices[0]);
      gdata.indices.push_back(ai_face.mIndices[1]);
      gdata.indices.push_back(ai_face.mIndices[2]);
    }

    GeometryResource::Ptr geom = resources_->MakeGeometry();
    geom->Load(gdata);
    geometries_.push_back(geom);
    geometry_materials_[geometries_.back()] = materials_[mesh->mMaterialIndex];
  }

  // Create the graph structure
  std::deque<const aiNode*> nodes_to_process = { ai_scene_->mRootNode };
  std::map<const aiNode*, GroupNode*> node_mapping = {
    { ai_scene_->mRootNode, model->Root() }
  };

  std::vector<GroupNode*> groups;
  while (!nodes_to_process.empty()) {
    const aiNode* ai_node = nodes_to_process.front();
    nodes_to_process.pop_front();

    for (size_t child_ind = 0; child_ind < ai_node->mNumChildren; ++child_ind) {
      const aiNode* child = ai_node->mChildren[child_ind];
      nodes_to_process.push_back(child);
    }

    // create a group node to match this node.
    const aiNode* ai_parent = ai_node->mParent;
    GroupNode* parent = node_mapping[ai_parent];
    GroupNode* group;
    if (ai_node == ai_scene_->mRootNode) {
      group = model->Root();
    } else {
      group = model->MakeGroup(parent);
    }
    groups.push_back(group);

    // attach draw nodes to this group
    for (size_t mesh_ind = 0; mesh_ind < ai_node->mNumMeshes; ++mesh_ind) {
      const size_t mesh_id = ai_node->mMeshes[mesh_ind];
      assert(mesh_id < geometries_.size());
      GeometryResource::Ptr& geom = geometries_[mesh_id];
      assert(geometry_materials_.find(geom) != geometry_materials_.end());
      MaterialResource::Ptr& material = geometry_materials_[geom];
      DrawNode* draw_node = model->MakeDrawNode(group);
      draw_node->Add(geom, material);
    }

    // The node transform
    aiVector3D ai_pos;
    aiQuaternion ai_quat;
    aiVector3D ai_scale;
    ai_node->mTransformation.Decompose(ai_scale, ai_quat, ai_pos);

    group->SetTranslation(QVector3D(ai_pos.x, ai_pos.y, ai_pos.z));
    group->SetScale(QVector3D(ai_scale.x, ai_scale.y, ai_scale.z));
    group->SetRotation(QQuaternion(ai_quat.w, ai_quat.x, ai_quat.y, ai_quat.z));

    node_mapping[ai_node] = group;
  }

  dbg("loaded %d nodes\n", static_cast<int>(groups.size()));

#if dbg
  for (size_t i = 0; i < groups.size(); ++i) {
    dbg("node %d\n", static_cast<int>(i));
    GroupNode* group = groups[i];
    const QVector3D pos = group->Translation();
    const QQuaternion rot = group->Rotation();
    const QVector3D scale = group->Scale();
    dbg("   children: %d\n", static_cast<int>(group->Children().size()));
    dbg("   pos   %.3f, %.3f, %.3f\n", pos.x(), pos.y(), pos.z());
    dbg("   quat  %.3f, %.3f, %.3f, %.3f\n",
        rot.x(), rot.y(), rot.z(), rot.w());
    dbg("   scale %.3f, %.3f, %.3f\n", scale.x(), scale.y(), scale.z());
    AxisAlignedBox box = group->BoundingBox();
    dbg("    bounding box: %s\n", box.ToString().c_str());
  }

  GroupNode* group = model->Root();
  AxisAlignedBox box = group->BoundingBox();
  dbg("model: %d children\n", static_cast<int>(group->Children().size()));
  dbg("    bounding box: %s\n", box.ToString().c_str());
#endif

  return model;
}

}  // namespace

Scene::Ptr ImportAssimpFile(ResourceManager::Ptr resources,
    const QString& fname, const QString& scene_name) {
  return Importer(resources).ImportFile(fname, scene_name);
}

}  // namespace sv
