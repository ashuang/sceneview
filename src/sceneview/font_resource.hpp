// Copyright [2015] Albert Huang

#ifndef SCENEVIEW_FONT_RESOURCE_HPP__
#define SCENEVIEW_FONT_RESOURCE_HPP__

#include <memory>

#include <QFont>

class QOpenGLTexture;

namespace sceneview {

/**
 * A font texture map, suitable for rendering fonts using texture-mapped quads.
 *
 * Only ASCII characters are supported.
 */
class FontResource {
  public:
    /**
     * Describes how to draw a character from the font texture map.
     *
     * The relevant quantities are:
     * - Texture coordinates
     *   - u0, v0 : top left of character
     *   - u1, v1 : bottom right of character
     * - Vertex offsets
     *   - x0, y0 : top left vertex offset
     *   - x1, y1 : bottom right vertex offset
     * - anchor : the nominal 3D position where the character is to be rendered
     * - line_height : the nominal line height in Cartesian space coordinates.
     *   Note that this is _not_ the line height in texture coordinates.
     *
     * To draw the character, draw a quad with:
     *
     * - top left = (anchor.x + x0 * line_height,
     *               anchor.y + y0 * line_height,
     *               0)
     * - bottom right = (anchor.x + x0 * line_height,
     *                   anchor.y + y0 * line_height,
     *                   0)
     *
     * Then, set anchor = (anchor.x + width_to_height * line_height, anchor.y, 0)
     * to draw the next character.
     */
    struct CharData {
      // Texture left coordinate
      float u0;

      // Texture top coordinate
      float v0;

      // Texture right coordinate
      float u1;

      // Texture bottom coordinate
      float v1;

      // Character width, relative to line height
      float width_to_height;

      // Vertex left offset (relative to line height)
      float x0;

      // Vertex top offset (relative to line height)
      float y0;

      // Vertex right offset (relative to line height)
      float x1;

      // Vertex bottom offset (relative to line height)
      float y1;
    };

  public:
    typedef std::shared_ptr<FontResource> Ptr;

    /**
     * Retrieve the texture.
     */
    QOpenGLTexture* Texture() { return texture_.get(); }

    /**
     * Retrieve parameters on how to draw the specified character.
     */
    const CharData& GetCharData(int c) {
      return char_data_[c];
    }

  private:
    friend class ResourceManager;

    static Ptr Create(const QFont& font);

    explicit FontResource(const QFont& font);

    void Build(const QFont& font);

    int num_rows_;
    int num_cols_;
    int block_size_;

    CharData char_data_[256];

    std::unique_ptr<QOpenGLTexture> texture_;
};

}  // namespace sceneview

#endif  // SCENEVIEW_FONT_RESOURCE_HPP__
