// Task 6 - Generate a Bezier surface of variable density with UV coordinates.
//        - Confine the Bezier data and associated methods into a reusable class.
//        - Define a modelMatrix that uses position, rotation and scale.
//        - Render the generated mesh with texturing applied.
//        - Animate rotation.

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ppgso/ppgso.h>

#include <shaders/texture_vert_glsl.h>
#include <shaders/texture_frag_glsl.h>

using namespace std;
using namespace glm;
using namespace ppgso;

const unsigned int SIZE = 512;

// Object to represent Bezier patch
class BezierPatch {
private:
  // 3D vectors define points/vertices of the shape
  vector<vec3> vertices;

  // Texture coordinates
  vector<vec2> texCoords;

  // Indices define triangles that index into vertices
  vector<GLuint> indices;

  // These will hold the data and object buffers
  GLuint vao, vbo, tbo, ibo;
  mat4 modelMatrix;

  vec3 bezierPoint(const vec3 *controlPoints, float t) {
    auto a = lerp(controlPoints[0], controlPoints[1], t);
    auto b = lerp(controlPoints[1], controlPoints[2], t);
    auto c = lerp(controlPoints[2], controlPoints[3], t);

    auto d = lerp(a, b, t);
    auto e = lerp(b, c, t);

    return lerp(d, e, t);
  }

  Shader program{texture_vert_glsl, texture_frag_glsl};
  Texture texture{image::loadBMP("lena.bmp")};
public:
  // Public attributes that define position, color ..
  vec3 position{0,0,0};
  vec3 rotation{0,0,0};
  vec3 scale{1,1,1};

  // Initialize object data buffers
  BezierPatch(const vec3 controlPoints[4][4]) {
    // Generate Bezier patch points and incidences
    unsigned int PATCH_SIZE = 10;
    for(unsigned int i = 0; i < PATCH_SIZE ; i++) {
      for (unsigned int j = 0; j < PATCH_SIZE; j++) {
        float u = (float)i/(float)(PATCH_SIZE-1);
        float v = (float)j/(float)(PATCH_SIZE-1);

        vec3 temp[4]{
          bezierPoint(controlPoints[0], u),
          bezierPoint(controlPoints[1], u),
          bezierPoint(controlPoints[2], u),
          bezierPoint(controlPoints[3], u),
        };
        vertices.push_back(bezierPoint(temp, v));
        texCoords.push_back(vec2{u,1.0f-v});
      }
    }
    // Generate indices
    for(unsigned int i = 1; i < PATCH_SIZE; i++) {
      for (unsigned int j = 1; j < PATCH_SIZE; j++) {
        // Face 1
        indices.push_back((i-1)+PATCH_SIZE*(j-1));
        indices.push_back((i)+PATCH_SIZE*(j));
        indices.push_back((i)+PATCH_SIZE*(j-1));
        // Face 2
        indices.push_back((i-1)+PATCH_SIZE*(j-1));
        indices.push_back((i)+PATCH_SIZE*(j));
        indices.push_back((i-1)+PATCH_SIZE*(j));
      }
    }

    // Copy data to OpenGL
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copy positions to gpu
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto position_attrib = program.getAttribLocation("Position");
    glEnableVertexAttribArray(position_attrib);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Copy texture positions to gpu
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);

    // Set vertex program inputs
    auto texCoord_attrib = program.getAttribLocation("TexCoord");
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
  void update() {
    // Compute transformation by scaling, rotating and then translating the shape
    modelMatrix = glm::translate(mat4{}, position)
                 * glm::orientate4(rotation)
                 * glm::scale(mat4{}, scale);
  }

  // Draw polygons
  void render(){
    // Update transformation and color uniforms in the shader
    program.use();

    // Initialize projection
    // Create projection matrix (field of view (radians), aspect ratio, near plane distance, far plane distance)
    // You can think of this as the camera objective settings
    auto projection = perspective( (PI/180.f) * 60.0f, 1.0f, 0.1f, 10.0f);
    program.setUniform("ProjectionMatrix", projection);

    // Create view matrix (translate camera a bit backwards, so we can see the geometry)
    // This can be seen as the camera position/rotation in space
    auto view = translate(mat4{}, {0.0f, 0.0f, -3.0f});
    program.setUniform("ViewMatrix", view);

    // Set model position
    program.setUniform("ModelMatrix", modelMatrix);

    // Bind texture
    program.setUniform("Texture", texture);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
  };
};

class BezierSurfaceWindow : public Window {
private:
  // Define 16 control points
  vec3 controlPoints[4][4]{
      { {-1,1,0}, {-0.5,1,0}, {.5,1,0}, {1,1,3}, },
      { {-1,.5,0}, {-0.5,.5,0}, {.5,.5,0}, {1,.5,0}, },
      { {-1,-.5,0}, {-0.5,-.5,0}, {.5,-.5,0}, {1,-.5,-1}, },
      { {-1,-1,3}, {-0.5,-1,0}, {.5,-1,0}, {1,-1,0}, },
  };

  BezierPatch bezier{controlPoints};
public:
  BezierSurfaceWindow() : Window{"task6_bezier_surface", SIZE, SIZE} {
    // Initialize OpenGL state
    // Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }

  void onIdle() {
    // Set gray background
    glClearColor(.1f,.1f,.1f,1.0f);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Move and Render shape
    float time = (float) glfwGetTime();
    bezier.rotation = {0,0,time};
    bezier.update();
    bezier.render();
  }
};

int main() {
  // Create new window
  auto window = BezierSurfaceWindow{};

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
