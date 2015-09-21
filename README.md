PPGSO OpenGL Examples
----

This repository contains simple OpenGL 3.3 examples for educational purposes.

All examples are written in C++11 and use [CMake build system](http://cmake.org). Additionally the following dependencies are required:

* [GLEW][1]
* [GLFW3][2]
* [GLM][3]

Installation
----

```bash
  cd PPGSO
  mkdir _build && cd _build
  cmake .. -DCMAKE_INSTALL_PREFIX=../_install
  cmake --build . --target install
```

Running
----

```bash
  cd PPGSO/_install
  ./gl_gradient
```

Links
----

[1]: http://glew.sourceforge.net
[2]: http://www.glfw.org
[3]: http://glm.g-truc.net
