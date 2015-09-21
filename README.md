PPGSO OpenGL Examples
----

This repository contains simple OpenGL 3.3 examples for educational purposes.

All examples are written in C++11 and use [CMake build system](http://cmake.org). Additionally the following dependencies are required:

* [GLEW][1]
* [GLFW3][2]
* [GLM][3]

Building
----

Recommended for OSX and Linux. For Windows see CMake setup.

```bash
  cd PPGSO/src
  make
  ./gl_gradient
```

Running
----
Generating projects using CMake
----

Using CMake from commandline you can generate the project files as shown below. The placeholder [YOUR_GENERATOR] should be replaced with the generator appropriate for your IDE/environment. Usually removing the option entirely will generate the default for the given platform. To find out all available generators just run `cmake --help`

```bash
  cd PPGSO
  mkdir _build && cd _build
  cmake .. -G[YOUR_GENERATOR] -DCMAKE_INSTALL_PREFIX=../_install
  cmake --build . --target install
```

When building on Windows CMake might not be able to automatically find all the needed dependencies and will rerquire additional pointers. This is done by setting CMake variables such as GLEW_INCLUDE_DIRS which should point to the headers of the glew library that you want to use. Same principle applies for other dependencies. You can alternaticely use cmake GUI and point it to the work directory, it will allow you to edit the variables more comfortably.

```bash
  cd PPGSO/_build
  cmake .. -DGLFW_INCLUDE_DIRS="C:\libs\glfw3\include" -DGLFW_LIBRARIES="C:\libs\glfw3\glfw3.dll" -DGLEW_INCLUDE_DIRS=...
  cmake --build . --target install
```


After installation the files should be installed into a new `_install` subdirectory. You can then run the examples as follows:

```bash
  cd PPGSO/_install
  ./gl_gradient
```

Links
----

[1]: http://glew.sourceforge.net
[2]: http://www.glfw.org
[3]: http://glm.g-truc.net
