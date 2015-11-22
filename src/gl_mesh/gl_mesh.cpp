// Example gl_mesh
// - Displays geometry that is loaded from OBJ files encapsuladed in a mesh object
// - Demonstrates basic use of keyboard events (press A to start/stop animation)
// - Implements object transformation based on mouse movement
// - Combines parallel and orthographic camera projection

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"

#include "gl_mesh_frag.h"
#include "gl_mesh_vert.h"

#define PI 3.14159265358979323846f

const unsigned int SIZE = 512;

bool animationEnabled = true;
double mousePosX = 0.0;
double mousePosY = 0.0;

// Keyboard press event handler
void OnKeyPress(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */) {
  if (key == GLFW_KEY_A && action == GLFW_PRESS) {
    animationEnabled = !animationEnabled;
  }
}

// Mouse move event handler
void OnMouseMove(GLFWwindow* /* window */, double xpos, double ypos) {
  mousePosX = (xpos / ((double) SIZE) * 2) - 1;
  mousePosY = -((ypos / ((double) SIZE) * 2) - 1);
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup OpenGL context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create a window
  auto window = glfwCreateWindow(SIZE, SIZE, "PPGSO gl_mesh", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Finalize window setup
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3")) {
    std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Add keyboard and mouse handlers
  glfwSetKeyCallback(window, OnKeyPress);
  glfwSetCursorPosCallback(window, OnMouseMove);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide mouse cursor

  // Load shaders
  auto program = ShaderPtr(new Shader{gl_mesh_vert, gl_mesh_frag});
  program->Use();

  // Load textures
  auto sphereTexture = TexturePtr(new Texture{"sphere.rgb", 256, 256});
  auto cursorTexture = TexturePtr(new Texture{"lena.rgb", 512, 512});

  // Load mesh data
  auto sphere = Mesh{
          program,      // Associate Position and TexCoord inputs with this program)
          "sphere.obj", // OBJ file to load data from
  };
  auto cursor = Mesh{program, "quad.obj"};

  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  float time = 0;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (animationEnabled) time = (float) glfwGetTime();

    // Create object matrices
    auto centerSphereMat = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.5f, 1.0f, 0.0f));
    auto smallSphereMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(time), cos(time), 0));
    smallSphereMat = glm::scale(smallSphereMat, glm::vec3(0.5f, 0.5f, 0.5f));

    // Camera position/rotation - for example, translate camera a bit backwards (positive value in Z axis), so we can see the objects
    auto cameraMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));
    program->SetMatrix(cameraMat, "ViewMatrix");

    // Update camera position with perspective projection
    program->SetMatrix(glm::perspective((PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f), "ProjectionMatrix");

    // Render objects
    program->SetTexture(sphereTexture, "Texture");

    // Central sphere
    program->SetMatrix(centerSphereMat, "ModelMatrix");
    sphere.Render();

    // Smaller sphere
    program->SetMatrix(smallSphereMat, "ModelMatrix");
    sphere.Render();

    // Draw cursor using orthographic projection
    // Update camera with orthographic projection
    program->SetMatrix(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1000.0f, 1000.0f), "ProjectionMatrix");

    // Create object matrix
    auto cursorMat = glm::translate(glm::mat4(1.0f), glm::vec3(mousePosX, mousePosY, 0.0f));
    cursorMat = glm::scale(cursorMat, glm::vec3(0.1f, 0.1f, 0.1f));

    // Render objects
    program->SetTexture(cursorTexture, "Texture");
    program->SetMatrix(cursorMat, "ModelMatrix");
    cursor.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
