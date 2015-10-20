#ifndef SCENEVIEW_TEXT_BILLBOARD__
#define SCENEVIEW_TEXT_BILLBOARD__

#include <memory>

#include <QFont>

#include <sceneview/font_resource.hpp>
#include <sceneview/geometry_resource.hpp>
#include <sceneview/material_resource.hpp>
#include <sceneview/mesh_node.hpp>
#include <sceneview/resource_manager.hpp>
#include <sceneview/scene.hpp>

namespace sceneview {

/**
 * A text display that always faces the camera.
 *
 * The text display is anchored to a point in the 3D scene, and the size of the
 * text is relative to the scene. If a perspective camera is far from the text,
 * then the text appears smaller.
 *
 * The following attributes of the text can be varied:
 * - size : SetLineHeight()
 * - font : SetFont()
 * - alignment : SetAlignment()
 * - text color : SetTextColor()
 * - background color : SetBackgroundColor()
 * - position : Node()->SetTranslation()
 */
class TextBillboard {
  public:
    enum HAlignment {
      kLeft = 4,
      kHCenter = 5,
      kRight = 6,
    };

    enum VAlignment {
      kBottom,
      kVCenter,
      kTop,
    };

    typedef std::shared_ptr<TextBillboard> Ptr;

    static Ptr Create(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene,
        GroupNode* parent);

    ~TextBillboard();

    void SetText(const QString& text);

    /**
     * Sets the font.
     *
     * The following attributes of the font are used:
     * - family
     * - weight
     * - italic
     *
     * The font size is ignored, and is determined by SetLineHeight().
     */
    void SetFont(const QFont& font);

    /**
     * Control text size by varying nominal line height.
     *
     * The height is in units of the Cartesian coordinate frame for the scene
     * node.
     */
    void SetLineHeight(float height);

    /**
     * Sets the foreground text color.
     */
    void SetTextColor(const QColor& color);

    /**
     * Sets the background color.
     *
     * To remove the background color, pass in a QColor with the alpha value
     * set to 0.
     */
    void SetBackgroundColor(const QColor& color);

    /**
     * Sets the text alignment.
     *
     * The specified portion of the rendered text is anchored to the
     * billboard's scene node. For example, if the scene node is positioned at
     * (10, 11, 12) in world coordinates, and the alignment is (kRight, kTop),
     * then the top right corner of the text is positioned at (10, 11, 12) in
     * world coordinates.
     */
    void SetAlignment(HAlignment horizontal, VAlignment vertical);

    GroupNode* Node() { return node_; }

  private:
    TextBillboard(const ResourceManager::Ptr& resources,
        const Scene::Ptr& scene,
        GroupNode* parent);

    void Recompute();

    ResourceManager::Ptr resources_;

    Scene::Ptr scene_;

    GroupNode* parent_;
    GroupNode* node_;

    FontResource::Ptr font_resource_;
    MaterialResource::Ptr bg_material_;
    GeometryResource::Ptr rect_geom_;
    MaterialResource::Ptr text_material_;
    GeometryResource::Ptr text_geom_;
    MaterialResource::Ptr depth_write_material_;
    MeshNode* mesh_;

    VAlignment v_align_;
    HAlignment h_align_;
    float line_height_;

    float margin_top_;
    float margin_left_;
    float margin_bottom_;
    float margin_right_;

    QFont qfont_;
    std::string text_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_TEXT_BILLBOARD__
