// Task 6 - Generate a Bezier surface of variable density with UV coordinates.
//        - Render the generated mesh with texturing applied.
//        - Animate rotation.

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include "task6_bezier_surface_vert.h"
#include "task6_bezier_surface_frag.h"

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
  // 3D vectors define points/vetrices of the shape
  vector<vec3> vetrices;

  // Texture coordinates
  vector<vec2> texCoords;

  // Indices define triangles that index into vertices
  vector<GLuint> indices;

  // These will hold the data and object buffers
  GLuint vao, vbo, tbo, ibo;
  mat4 modelMatrix;

  vec3 BezierPoint(const vec3 controlPoints[4], float t) {
    auto a = lerp(controlPoints[0], controlPoints[1], t);
    auto b = lerp(controlPoints[1], controlPoints[2], t);
    auto c = lerp(controlPoints[2], controlPoints[3], t);

    auto d = lerp(a, b, t);
    auto e = lerp(b, c, t);

    return lerp(d, e, t);
  }

  Shader program{task6_bezier_surface_vert, task6_bezier_surface_frag};
  Texture texture{"lena.rgb", 512, 512};
public:
  // Public attributes that define position, color ..
  vec3 position{0,0,0};
  vec3 rotation{0,0,0};
  vec3 scale{1,1,1};

  // Initialize object data buffers
  BezierPatch(const vec3 controlPoints[4][4]) {
    // Generate Bezier patch points and incidences
    unsigned int PATCH_SIZE = 30;
    for(unsigned int i = 0; i < PATCH_SIZE ; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE; j++) {
        float u = (float)i/(float)PATCH_SIZE;
        float v = (float)j/(float)PATCH_SIZE;

        vec3 temp[4] = {
          BezierPoint(controlPoints[0], u),
          BezierPoint(controlPoints[1], u),
          BezierPoint(controlPoints[2], u),
          BezierPoint(controlPoints[3], u),
        };
        vetrices.push_back(BezierPoint(temp, v));
        texCoords.push_back(vec2{u,v});
      }
    }
    // Generate indices
    for(unsigned int i = 0; i < PATCH_SIZE - 1; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE - 1; j++) {
        // Face 1
        indices.push_back((i+0)+PATCH_SIZE*(j+0));
        indices.push_back((i+1)+PATCH_SIZE*(j+1));
        indices.push_back((i+1)+PATCH_SIZE*(j+0));
        // Face 2
        indices.push_back((i+0)+PATCH_SIZE*(j+0));
        indices.push_back((i+1)+PATCH_SIZE*(j+1));
        indices.push_back((i+0)+PATCH_SIZE*(j+1));
      }
    }

    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vetrices.size() * sizeof(vec3), vetrices.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.GetAttribLocation("position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy texture positions to gpu
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto texCoord_attrib = program.GetAttribLocation("texCoord");
    glEnableVertexAttribArray(texCoord_attrib);
    glVertexAttribPointer(texCoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy indices to gpu
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  };
  // Clean up
  ~BezierPatch() {
    // Delete data from OpenGL
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }
  // Set the object transformation matrix
  void Update() {
    // Compute transformation by scaling, rotating and then translating the shape
    modelMatrix = glm::translate(mat4{}, position)
                 * glm::orientate4(rotation)
                 * glm::scale(mat4{}, scale);
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
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
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

  Camera camera{75.0f, (float)width/(float)height, 1.0f, 30.0f};
  BezierPatch bezier{controlPoints};
public:
  BezierSurfaceWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  }

  void onPool() {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Move and Render shape
    float time = (float) glfwGetTime();
    bezier.rotation = {0,0,time};
    bezier.Update();
    bezier.Render(camera);
  }
};

int main() {
  // Create new window
  auto window = BezierSurfaceWindow{"task6_bezier_surface", SIZE, SIZE};

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
