#ifndef PPGSO_WINDOW_H
#define PPGSO_WINDOW_H

#include <string>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ppgso {
  class Window {
  private:
    class glfwInstance {
    protected:
      glfwInstance();
      ~glfwInstance();
    public:
      static glfwInstance &Init();
    };

    class glewInstance {
    private:
      glewInstance();
    public:
      static glewInstance &Init();
    };

    // Collection of windows
    static std::map<GLFWwindow*, Window*> windows;

    // Callbacks
    static void glfw_error_callback(int error, const char *description);
    static void glfw_key_callback(GLFWwindow *window, int key, int scanCode, int action, int mods);
    static void glfw_cursor_pos_callback(GLFWwindow *window, double cursorX, double cursorY);

  protected:
    GLFWwindow *window;
  public:
    std::string title;
    unsigned int width, height;

    Window(std::string title, unsigned int width, unsigned int height);

    virtual ~Window();

    virtual void onPool() {}
    virtual void onKey(int key, int scanCode, int action, int mods) {};
    virtual void onCursorPos(double cursorX, double cursorY) {};

    void ResetViewport();

    void HideCursor();

    void ShowCursor();

    int Pool();
  };

}

#endif //PPGSO_WINDOW_H
