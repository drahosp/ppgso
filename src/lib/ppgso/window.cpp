#include <cstdlib>
#include <iostream>

#include "window.h"

using namespace std;
using namespace ppgso;

int Window::Pool() {
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

  #ifdef DEBUG
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  #endif

  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window)
    throw std::runtime_error("Failed to initialize GLFW Window!");

  glfwSetKeyCallback(window, glfw_key_callback);
  glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);

  glfwMakeContextCurrent(window);

  // Initialize glew
  glewInstance::Init();

  windows.insert({window, this});
}

Window::~Window() {
  windows.erase(window);
  glfwDestroyWindow(window);
}

void Window::glfw_key_callback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  windows[window]->onKey(key, scanCode, action, mods);
}

void Window::glfw_error_callback(int error, const char *description) {
  throw std::runtime_error(description);
}

void Window::ResetViewport() {
  int fbWidth, fbHeight;
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  glViewport(0, 0, fbWidth, fbHeight);
}

void Window::ShowCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::HideCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window::glfw_cursor_pos_callback(GLFWwindow *window, double cursorX, double cursorY) {
  windows[window]->onCursorPos(cursorX, cursorY);
}

Window::glewInstance &Window::glewInstance::Init() {
  static glewInstance instance;
  return instance;
}

Window::glewInstance::glewInstance() {
  glewExperimental = GL_TRUE;
  glewInit();

  if (!glewIsSupported("GL_VERSION_3_3"))
    throw std::runtime_error("Failed to initialize GLEW with OpenGL 3.3!");
}

// Store window instances for callbacks
std::map<GLFWwindow *, Window *> Window::windows;

Window::glfwInstance::glfwInstance() {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW!");
}

Window::glfwInstance::~glfwInstance() {
  glfwTerminate();
}

Window::glfwInstance &Window::glfwInstance::Init() {
  static glfwInstance instance;
  return instance;
}
