Memory management {#memory_management}
========

Sceneview has several different memory ownership models, largely to reflect
differing use cases.

In general, you should never need to explicitly call `new` or `delete` on any
Sceneview classes.

- All classes derived from QObject or QWidget adhere to the [Qt 5 ownership
  model](http://doc.qt.io/qt-5/objecttrees.html).
- All nodes in a scene graph are owned directly by the scene itself (the
  sv::Scene object).
- All resources (Material, Geometry, Shader, Font) are managed with
  reference-counted shared pointers.
