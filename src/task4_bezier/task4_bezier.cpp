// Task 4 - Implement a function to generate bezier curve points
//        - Analyze gl1_gradient example to find out how to define and render OpenGL geometry
//        - Draw an alphabet letter using bezier curves
//        - Render the letter using OpenGL

#include <iostream>
#include <vector>

#include <ppgso/ppgso.h>

#include <shaders/color_vert_glsl.h>
#include <shaders/color_frag_glsl.h>

const unsigned int SIZE = 512;

using namespace std;
using namespace glm;
using namespace ppgso;

class BezierWindow : public Window {
private:

  // Control points for the bezier curves
  // First curve is 4 control points
  // Rest of the curves are 3 control points, each reusing the last curve end
  // Defines the letter "R"
  vector<vec2> controlPoints = {
      { 0,  -1},
      { 0, -.3},
      { 0,  .3},
      { 0,   1},
      {.3,   1},
      {.5,   1},
      {.5,  .5},
      {.5,   0},
      {.3,   0},
      { 0,   0},
      {.3, -.3},
      {.5, -.5},
      {.5,  -1},
  };

  // This will hold the bezier curve geometry once we generate it
  vector<vec3> points;

  // GLSL Program to use for rendering
  Shader program = {color_vert_glsl, color_frag_glsl};

  // These numbers are used to pass buffer data to OpenGL
  GLuint vao = 0, vbo = 0;

  // Compute points for Bezier curve using 4 control points
  vec2 bezierPoint(const vec2 &p0, const vec2 &p1, const vec2 &p2, const vec2 &p3, const float t) {
    vec2 a = lerp(p0, p1, t);
    vec2 b = lerp(p1, p2, t);
    vec2 c = lerp(p2, p3, t);
    vec2 ab = lerp(a, b, t);
    vec2 bc = lerp(b, c, t);
    return lerp(ab, bc, t);
  }

  // Compute points for a sequence of Bezier curves defined by a vector of control points
  // Each bezier curve will reuse the end point of the previous curve
  void bezierShape(int count) {
    for(int i = 1; i < (int) controlPoints.size(); i+=3) {
      for (int j = 0; j <= count; j++) {
        float t = (float) j / (float) count;
        vec2 point = bezierPoint(controlPoints[i - 1], controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], t);
        points.emplace_back(point, 0);
      }
    }
  }

public:
  BezierWindow() : Window{"task3_bezier", SIZE, SIZE} {
    // Generate Bezier curve points
    bezierShape(15);

    // Generate a vertex array object
    // This keeps track of what attributes are associated with buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), points.data(), GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "Position" input
    auto position_attrib = program.getAttribLocation("Position");
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(position_attrib);

    // Set model matrix to identity
    program.setUniform("ModelMatrix", mat4{});
    program.setUniform("ViewMatrix", mat4{});
    program.setUniform("ProjectionMatrix", mat4{});

    // Set the color uniform
    program.setUniform("OverallColor", vec3{1.0f,1.0f,1.0f});
  }

  ~BezierWindow() final {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void onRefresh() final {
    // Set gray background
    glClearColor(0.5f,0.5f,0.5f,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw shape
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) points.size());
  }
};

int main() {
  // Create our window
  BezierWindow window;

  // Main execution loop
  while (window.pollEvents()) {}

  return EXIT_SUCCESS;
}
