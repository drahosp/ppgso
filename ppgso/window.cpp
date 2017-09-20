#include <cstdlib>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"

using namespace std;
using namespace ppgso;

bool Window::pollEvents() {
  onIdle();
  glfwSwapBuffers(window);
  glfwPollEvents();
  return !glfwWindowShouldClose(window);
}

Window::Window(std::string title, unsigned int width, unsigned int height) : title{title}, width{width}, height{height} {
  // Set up glfw
  glfwInstance::Init();

  glfwSetErrorCallback(glfw_error_callback);

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window)
    throw runtime_error("Failed to initialize GLFW Window!");

  glfwSetKeyCallback(window, glfw_key_callback);
  glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
  glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
  glfwSetWindowRefreshCallback(window, glfw_window_refresh_callback);

  glfwMakeContextCurrent(window);

  // Initialize glew
  glewInstance::Init();

  windows.insert({window, this});

#ifndef NDEBUG
  // Basic OpenGL information to print
  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
#endif
}

Window::~Window() {
  windows.erase(window);
  glfwDestroyWindow(window);
}

void Window::glfw_key_callback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) windows[window]->close();

  windows[window]->onKey(key, scanCode, action, mods);
}

void Window::glfw_error_callback(int error, const char *description) {
  stringstream msg;
  msg << "GLFW Error " << error << " : " << string{description} << endl;

  cerr << msg.str() << endl;

  throw runtime_error(msg.str());
}

void Window::resetViewport() {
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
}

void Window::showCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::hideCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window::glfw_cursor_pos_callback(GLFWwindow *window, double cursorX, double cursorY) {
  windows[window]->onCursorPos(cursorX, cursorY);
}

void Window::glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  windows[window]->onMouseButton(button, action, mods);
}

void Window::close() {
  glfwSetWindowShouldClose(window, true);
}

void Window::glfw_window_refresh_callback(GLFWwindow *window) {
  windows[window]->onRefresh();
}

void Window::resize(unsigned int width, unsigned int height) {
  glfwSetWindowSize(window, width, height);
}

Window::glewInstance &Window::glewInstance::Init() {
  static glewInstance instance;
  return instance;
}

Window::glewInstance::glewInstance() {
  glewExperimental = GL_TRUE;
  glewInit();

  if (!glewIsSupported("GL_VERSION_3_3"))
    throw runtime_error("Failed to initialize GLEW with OpenGL 3.3!");
}

// Store window instances for callbacks
std::map<GLFWwindow *, Window *> Window::windows;

Window::glfwInstance::glfwInstance() {
  if (!glfwInit())
    throw runtime_error("Failed to initialize GLFW!");
}

Window::glfwInstance::~glfwInstance() {
  glfwTerminate();
}

Window::glfwInstance &Window::glfwInstance::Init() {
  static glfwInstance instance;
  return instance;
}

void Window::fpsLimit(bool limit) {
  if(limit) glfwSwapInterval(1);
  glfwSwapInterval(0);
}
