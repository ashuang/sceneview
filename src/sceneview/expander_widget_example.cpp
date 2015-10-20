#include <sceneview/expander_widget.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>

using sceneview::ExpanderWidget;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow window;
  ExpanderWidget* expander = new ExpanderWidget(&window);

  QWidget* contents = new QWidget(expander);
  QVBoxLayout* layout = new QVBoxLayout(contents);
  QPlainTextEdit* text_edit = new QPlainTextEdit("Text edit...", contents);
  layout->addWidget(text_edit);
  QPushButton* button = new QPushButton("Button", contents);
  layout->addWidget(button);

  expander->SetWidget(contents);
  expander->SetTitle("Expander test");

  window.setCentralWidget(expander);
  window.show();

  return app.exec();
}
