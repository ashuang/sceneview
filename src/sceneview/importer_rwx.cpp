// Copyright [2015] Albert Huang

#include "importer_rwx.hpp"

#include <cstdlib>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <QFile>
#include <QVector3D>
#include <QVector4D>

#include "sceneview/group_node.hpp"
#include "sceneview/mesh_node.hpp"
#include "sceneview/stock_resources.hpp"

#if 0
#define dbg(...) printf(__VA_ARGS__)
#else
#define dbg(...)
#endif

namespace sceneview {

namespace {

enum class TokenType {
  kEOF,
  kIdentifier,
  kInvalid
};

struct Token {
  Token() :
    type(TokenType::kInvalid),
    value(),
    int_val(0),
    double_val(0) {}
  Token(TokenType type, const std::string& value) :
    type(type),
    value(value),
    int_val(strtol(&value[0], nullptr, 10)),
    double_val(strtod(&value[0], nullptr)) {}

  TokenType type;
  std::string value;
  int64_t int_val;
  double double_val;
};

static std::string ToString(const std::vector<char>& vec) {
  return std::string(&vec[0], vec.size());
}

class Tokenizer {
 public:
    explicit Tokenizer(QIODevice* input) :
      input_(input),
      have_unget_(false),
      unget_char_(0),
      line_pos_(0),
      line_len_(0),
      line_num_(0) {}

    Token NextToken() {
      bool have_char;
      char cur_char;

      // Skip whitespace
      for (have_char = NextChar(&cur_char);
          have_char && std::isspace(cur_char);
          have_char = NextChar(&cur_char)) {}

      if (!have_char) {
        return Token(TokenType::kEOF, "");
      }

      std::vector<char> tok_chars;
      tok_chars.push_back(cur_char);
      while (NextChar(&cur_char) &&
             !std::isspace(cur_char)) {
        tok_chars.push_back(cur_char);
      }
      return Token(TokenType::kIdentifier, ToString(tok_chars));
    }

    std::string FormatError(const std::string& msg,
        const std::string& token_text) {
      return msg + " - " + token_text;
    }

 private:
    void ThrowError(const std::string& msg,
        const std::vector<char>& tok_chars) {
      throw std::runtime_error(FormatError(msg, ToString(tok_chars)));
    }

    bool NextChar(char* result) {
      if (have_unget_) {
        have_unget_ = false;
        *result = unget_char_;
      } else {
        if (line_pos_ == line_len_) {
          line_.clear();
          line_ = input_->readLine();
          line_len_ = line_.size();
          line_pos_ = 0;
          if (line_.isEmpty()) {
            return false;
          }
        }

        *result = line_[line_pos_];
        line_pos_++;
      }

      if (*result == '\n') {
        line_num_++;
      }
      return true;
    }

    void UngetChar(char value) {
      have_unget_ = true;
      unget_char_ = value;
    }

    QIODevice* input_;
    bool have_unget_;
    char unget_char_;
    int line_pos_;
    int line_len_;
    int line_num_;
    QByteArray line_;
};

class Parser {
  public:
    Parser(const ResourceManager::Ptr& resources,
        const QString& scene_name,
        QIODevice* input) :
      resources_(resources),
      scene_(resources->MakeScene(scene_name)),
      scene_name_(scene_->Name()),
      stock_(resources_),
      tokenizer_(input) {}

    Scene::Ptr Parse() {
      GetToken();

      EatTokenOrDie("ModelBegin");
      EatTokenOrDie("ClumpBegin");

      while (true) {
        if (EatToken("ClumpBegin")) {
          ParseClump();
        } else if (EatToken("ClumpEnd")) {
          EatTokenOrDie("ModelEnd");
          break;
        }
      }

#if 0
      const std::vector<SceneNode*>& children = scene_->Root()->Children();
      for (size_t i = 0; i < children.size(); ++i) {
        dbg("node %d\n", static_cast<int>(i));
        MeshNode* mesh = dynamic_cast<MeshNode*>(children[i]);
        const QVector3D pos = mesh->Translation();
        const QQuaternion rot = mesh->Rotation();
        const QVector3D scale = mesh->Scale();
        dbg("   pos   %.3f, %.3f, %.3f\n", pos.x(), pos.y(), pos.z());
        dbg("   quat  %.3f, %.3f, %.3f, %.3f\n",
            rot.x(), rot.y(), rot.z(), rot.scalar());
        dbg("   scale %.3f, %.3f, %.3f\n", scale.x(), scale.y(), scale.z());
        AxisAlignedBox box = mesh->BoundingBox();
        dbg("    bounding box: %s\n", box.ToString().toStdString().c_str());
      }

      GroupNode* group = scene_->Root();
      AxisAlignedBox box = group->BoundingBox();
      dbg("model: %d children\n", static_cast<int>(group->Children().size()));
      dbg("    bounding box: %s\n", box.ToString().toStdString().c_str());
#endif

      return scene_;
    }

  private:
    void ParseClump() {
      EatTokenOrDie("#Layer:");
      GetToken();
      const std::string clump_name = cur_tok_.value;

      GeometryData gdata;
      gdata.gl_mode = GL_TRIANGLES;

      float color[3] = { 0, 0, 0 };
      float opacity = 1;
      float ambient = 1;
      float diffuse = 1;
      float specular = 0;

      while (!EatToken("#texbegin") && cur_tok_.type != TokenType::kEOF) {
        if (EatToken("Color")) {
          for (int i = 0; i < 3; ++i) {
            color[i] = ParseDouble();
          }
        } else if (EatToken("Surface")) {
          ambient = ParseDouble();
          diffuse = ParseDouble();
          specular = ParseDouble();
        } else if (EatToken("Diffuse")) {
          diffuse = ParseDouble();
        } else if (EatToken("Specular")) {
          specular = ParseDouble();
        } else if (EatToken("Opacity")) {
          opacity = ParseDouble();
        } else {
          GetToken();
        }
      }
      (void) ambient;

      EatTokenOrDie(clump_name);

      std::map<int, int> vertex_id_map;

      while (EatToken("Vertex")) {
        const double x = ParseDouble();
        const double y = ParseDouble();
        const double z = ParseDouble();
        const int vertex_index = gdata.vertices.size();
        gdata.vertices.emplace_back(x, y, z);
        if (EatToken("UV")) {
          const double tex_u = ParseDouble();
          const double tex_v = ParseDouble();
          gdata.tex_coords_0.emplace_back(tex_u, tex_v);
        }
        if (EatToken("#!")) {
          GetToken();
        }
        while (cur_tok_.value[0] != '#') {
          if (!GetToken()) {
            ThrowParseError("EOF reached when parsing vertex", cur_tok_);
          }
        }
        if (cur_tok_.value[0] != '#') {
          ThrowParseError("Expected vertex #id", cur_tok_);
        }
        char *eptr = nullptr;
        const int vertex_id = strtol(&cur_tok_.value[1], &eptr, 10);
        if (*eptr != '\0') {
          ThrowParseError("Expected integer vertex ID", cur_tok_);
        }
        vertex_id_map[vertex_id] = vertex_index;
      }

      EatTokenOrDie("#texend");
      EatTokenOrDie(clump_name);

      while (EatToken("Triangle")) {
        const int v0 = vertex_id_map[ParseInt()];
        const int v1 = vertex_id_map[ParseInt()];
        const int v2 = vertex_id_map[ParseInt()];
        gdata.indices.push_back(v0);
        gdata.indices.push_back(v1);
        gdata.indices.push_back(v2);
      }

      EatTokenOrDie("ClumpEnd");

      // Generate normal vectors
      // For every single vertex, average out the normal vectors for every
      // triangle that the vertex participates in.  Set that averaged vector
      // as the normal vector for that vertex.  This results in a much
      // smoother rendered model than the simple way (which is to just have
      // a single normal vector for all three vertices of a triangle when
      // the triangle is drawn).
      const int num_vertices = gdata.vertices.size();
      gdata.normals.resize(num_vertices);
      std::vector<int> in_num_triangles(num_vertices, 0);
      for (size_t index_ind = 0; index_ind < gdata.indices.size();
          index_ind += 3) {
        const int vid0 = gdata.indices[index_ind + 0];
        const int vid1 = gdata.indices[index_ind + 1];
        const int vid2 = gdata.indices[index_ind + 2];

        const QVector3D& vertex0 = gdata.vertices[vid0];
        const QVector3D& vertex1 = gdata.vertices[vid1];
        const QVector3D& vertex2 = gdata.vertices[vid2];

        const QVector3D edge_a = vertex1 - vertex0;
        const QVector3D edge_b = vertex2 - vertex0;
        const QVector3D normal =
          QVector3D::crossProduct(edge_a, edge_b).normalized();

        gdata.normals[vid0] += normal;
        gdata.normals[vid1] += normal;
        gdata.normals[vid2] += normal;

        in_num_triangles[vid0]++;
        in_num_triangles[vid1]++;
        in_num_triangles[vid2]++;
      }

      // Average out the vertex normals and renormalize
      for (int vertex_ind = 0; vertex_ind < num_vertices; ++vertex_ind) {
        gdata.normals[vertex_ind] /= in_num_triangles[vertex_ind];
        gdata.normals[vertex_ind].normalize();
      }

      GeometryResource::Ptr geom = resources_->MakeGeometry();
      geom->Load(gdata);

      MaterialResource::Ptr material =
        stock_.NewMaterial(StockResources::kUniformColorLighting);
//      material->SetParam("ambient",
//        color[0] * ambient, color[1] * ambient, color[2] * ambient, opacity);
      material->SetParam("diffuse",
        color[0] * diffuse, color[1] * diffuse, color[2] * diffuse, opacity);
      material->SetParam("specular",
        color[0] * specular, color[1] * specular, color[2] * specular, opacity);
      material->SetParam("shininess", 16.0f);
      material->SetTwoSided(true);

      MeshNode* mesh = scene_->MakeMesh(scene_->Root(),
          QString::fromStdString(clump_name));
      mesh->Add(geom, material);
    }

    int ParseInt() {
      GetToken();
      return cur_tok_.int_val;
    }

    double ParseDouble() {
      GetToken();
      return cur_tok_.double_val;
    }

    bool GetToken() {
      cur_tok_ = next_tok_;
      next_tok_ = tokenizer_.NextToken();
      return cur_tok_.type != TokenType::kEOF;
    }

    bool EatToken(const std::string& value) {
      if (next_tok_.value == value) {
        GetToken();
        return true;
      }
      return false;
    }

    void ThrowParseError(const std::string& msg, const Token& token) {
      throw std::runtime_error(tokenizer_.FormatError(msg, token.value));
    }

    void EatTokenOrDie(const std::string& value) {
      if (!EatToken(value)) {
        ThrowParseError("Parse error, expected " + value, next_tok_);
      }
    }

    ResourceManager::Ptr resources_;
    Scene::Ptr scene_;
    QString scene_name_;
    StockResources stock_;
    Tokenizer tokenizer_;
    Token cur_tok_;
    Token next_tok_;
};

}  // namespace

Scene::Ptr ImportRwxFile(ResourceManager::Ptr resources,
        const QString& fname, const QString& resource_name) {
  QFile file(fname);

  if (!file.open(QIODevice::ReadOnly)) {
    qDebug("Error opening file %s\n", fname.toStdString().c_str());
    return Scene::Ptr();
  }

  Parser parser(resources, resource_name, &file);
  return parser.Parse();
}

}  // namespace sceneview
