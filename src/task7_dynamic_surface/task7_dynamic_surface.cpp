// Task 7 - Dynamically generate a Bezier surface of variable density with UV coordinates.
//        - Render the generated mesh with texturing applied.
//        - Animate rotation controlled using keyboard.

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include "task7_dynamic_surface_vert.h"
#include "task7_dynamic_surface_frag.h"

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

class Camera {
public:
  vec3 up = {0,1,0};
  vec3 position = {0,0,5};
  vec3 center = {0,0,0};

  mat4 viewMatrix;
  mat4 projectionMatrix;

  Camera(float fow = 45.0f, float ratio = 1.0f, float near = 0.1f, float far = 10.0f) {
    float fowInRad = (PI/180.0f) * fow;
    projectionMatrix = perspective(fowInRad, ratio, near, far);
    Update();
  }

  void Update() {
    viewMatrix = lookAt(position, center, up);
  }
};

// Object to represent Bezier patch
class BezierPatch {
private:
  // 3D vectors define points/vertices of the shape
  vector<vec3> vertices;

  // Texture coordinates
  vector<vec2> texCoords;

  // Indices define triangles that index into vertices
  struct face {
    GLuint v0,v1,v2;
  };
  vector<face> indices;

  // These will hold the data and object buffers
  GLuint vao, vbo, tbo, ibo;
  mat4 modelMatrix;

  inline vec3 BezierPoint(const vec3 controlPoints[4], float t) {
    auto a = lerp(controlPoints[0], controlPoints[1], t);
    auto b = lerp(controlPoints[1], controlPoints[2], t);
    auto c = lerp(controlPoints[2], controlPoints[3], t);
    auto d = lerp(a, b, t);
    auto e = lerp(b, c, t);
    return lerp(d, e, t);
  }

  inline vec3 BezierSurface(const vec3 controlPoints[4][4], float u, float v) {
    vec3 temp[4] = {
        BezierPoint(controlPoints[0], u),
        BezierPoint(controlPoints[1], u),
        BezierPoint(controlPoints[2], u),
        BezierPoint(controlPoints[3], u),
    };
    return BezierPoint(temp, v);
  }

  Shader program = {task7_dynamic_surface_vert, task7_dynamic_surface_frag};
  Texture texture = {"lena.bmp"};
public:
  // Public attributes that define position, color ..
  vec3 position{0,0,0};
  vec3 rotation{0,0,0};
  vec3 scale{1,1,1};

  void Update(const vec3 controlPoints[4][4]) {
    // Generate Bezier patch points and incidences
    unsigned int PATCH_SIZE = 10;

    // Cleat the data and reserve memory for new data
    vertices.clear();
    vertices.reserve(PATCH_SIZE*PATCH_SIZE);
    texCoords.clear();
    vertices.reserve(PATCH_SIZE*PATCH_SIZE);

    #pragma parallel for
    for(unsigned int i = 0; i < PATCH_SIZE ; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE; j++) {
        float u = (float)i/(float)(PATCH_SIZE-1);
        float v = (float)j/(float)(PATCH_SIZE-1);

        vertices.push_back({BezierSurface(controlPoints, u, v)});
        texCoords.push_back({u,v});
      }
    }
    // Generate indices
    indices.clear();
    vertices.reserve(PATCH_SIZE*PATCH_SIZE*2);

    #pragma parallel for
    for(unsigned int i = 0; i < PATCH_SIZE - 1; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE - 1; j++) {
        int index = (i + (PATCH_SIZE-1) * j ) * 2;
        // Face 1
        indices.push_back({ i+PATCH_SIZE*j, i+PATCH_SIZE*(j+1), (i+1)+PATCH_SIZE*(j+1) });
        // Face 2
        indices.push_back({ i+PATCH_SIZE*j, (i+1)+PATCH_SIZE*(j+1), (i+1)+PATCH_SIZE*j });
      }
    }

    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_DYNAMIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.GetAttribLocation("position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy texture positions to gpu
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_DYNAMIC_DRAW);

    // Set vertex program inputs
    auto texCoord_attrib = program.GetAttribLocation("texCoord");
    glEnableVertexAttribArray(texCoord_attrib);
    glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy indices to gpu
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(face), indices.data(), GL_DYNAMIC_DRAW);

    // Compute transformation by scaling, rotating and then translating the shape
    modelMatrix = glm::translate(mat4{}, position)
                * glm::orientate4(rotation)
                * glm::scale(mat4{}, scale);
  }

  // Initialize object data buffers
  BezierPatch(const vec3 controlPoints[4][4]) {
    // Generate OpenGL buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &tbo);
    glGenBuffers(1, &ibo);

    Update(controlPoints);
  };
  // Clean up
  ~BezierPatch() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  // Draw polygons
  void Render(Camera &camera){
    // Update transformation and color uniforms in the shader
    program.Use();
    program.SetMatrix(modelMatrix, "modelMatrix");
    program.SetMatrix(camera.viewMatrix, "viewMatrix");
    program.SetMatrix(camera.projectionMatrix, "projectionMatrix");
    program.SetTexture(texture, "tex");

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size()*sizeof(face), GL_UNSIGNED_INT, 0);
  };
};

class BezierSurfaceWindow : public Window {
private:
  // Define 16 control points
  vec3 controlPoints[4][4] = {
      { {-1,1,0}, {-0.5,1,0}, {.5,1,0}, {1,1,3}, },
      { {-1,.5,0}, {-0.5,.5,0}, {.5,.5,0}, {1,.5,0}, },
      { {-1,-.5,0}, {-0.5,-.5,0}, {.5,-.5,0}, {1,-.5,-1}, },
      { {-1,-1,3}, {-0.5,-1,0}, {.5,-1,0}, {1,-1,0}, },
  };

  Camera camera = {75.0f, (float)width/(float)height, 1.0f, 30.0f};
  BezierPatch bezier = {controlPoints};
  float angle = 0;
public:
  BezierSurfaceWindow() : Window{"task7_dynamic_surface", SIZE, SIZE} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }

  void onKey(int key, int scanCode, int action, int mods) override {
    // Rotate object using keyboard
    if (key == GLFW_KEY_LEFT ) bezier.rotation.z += 0.1;
    if (key == GLFW_KEY_RIGHT ) bezier.rotation.z -= 0.1;
    if (key == GLFW_KEY_UP ) bezier.rotation.x += 0.1;
    if (key == GLFW_KEY_DOWN ) bezier.rotation.x -= 0.1;
  }

  void onIdle() override {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = (float) glfwGetTime();
    // Move control points
    controlPoints[0][3].z = sin(time);
    controlPoints[3][0].z = -cos(time) * 3.0f;

    bezier.Update(controlPoints);
    bezier.Render(camera);
  }
};

int main() {
  // Create new window
  auto window = BezierSurfaceWindow{};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
