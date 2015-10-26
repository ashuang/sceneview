#include "text_renderer.hpp"

#include <QColor>

using sv::TextBillboard;

namespace vis_examples {

TextRenderer::TextRenderer(const QString& name, QObject* parent) :
  Renderer(name, parent),
  timer_() {
  timer_.start();
}

void TextRenderer::InitializeGL() {
  text_billboard_ =
    TextBillboard::Create(GetViewport(), GetBaseNode());

//  text_billboard_->SetFont(QFont("URW Chancery L"));
  text_billboard_->SetLineHeight(0.4);
  text_billboard_->SetTextColor(QColor(40, 255, 40));
  text_billboard_->SetBackgroundColor(QColor(50, 50, 50, 200));
  text_billboard_->SetAlignment(TextBillboard::kLeft, TextBillboard::kTop);
  UpdateText();
  text_billboard_->Node()->SetTranslation(1, 2, -3);
}

void TextRenderer::ShutdownGL() {
  text_billboard_.reset();
}

void TextRenderer::RenderBegin() {
  UpdateText();
}

void TextRenderer::UpdateText() {
  QString text = QString(
      "Hello, world!"
      "\nABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "\nabcdefghijklmnopqrstuvwxyz"
      "\n0123456789"
      "\n[]{}|_,./\\"
      "\nelapsed: %1s").arg(timer_.elapsed() / 1000., 0, 'g', 3);
  text_billboard_->SetText(text);
}

}  // namespace vis_examples
