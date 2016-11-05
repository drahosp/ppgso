// Task 3 - Implement a function to generate bezier curve points
//        - Draw an alphabet letter using bezier curves
//        - Render the letter using OpenGL

#include <iostream>
#include <vector>
#include <array>

#include <ppgso/ppgso.h>

#include "task3_bezier_vert.h"
#include "task3_bezier_frag.h"

const unsigned int SIZE = 512;

using namespace std;
using namespace glm;
using namespace ppgso;

template<typename T>
T lerp(T &v0, T &v1, float t){
  return v0*(1.0f-t)+v1*t;
}

class BezierWindow : public Window {
private:

  // Control points for the bezier curves
  // First curve is 4 control points
  // Rest of the curves are 3 control points, each reusing the last curve end
  // Defines the letter "R"
  vector<vec2> cpoints{
      {0, -1},
      {0, -.3},
      {0, .3},
      {0, 1},
      {.3, 1},
      {.5, 1},
      {.5, .5},
      {.5, 0},
      {.3, 0},
      {0, 0},
      {.3, -.3},
      {.5, -.5},
      {.5, -1},
  };

  // This will hold the bezier curve geometry once we generate it
  vector<vec2> points;

  // GLSL Program to use for rendering
  Shader program{task3_bezier_vert, task3_bezier_frag};

  // These numbers are used to pass buffer data to OpenGL
  GLuint vao, vbo, cvao, cvbo;

  // Compute points for Bezier curve using 4 control points
  void BezierCurve(vector<vec2> &points, const vec2 &p0, const vec2 &p1, const vec2 &p2, const vec2 &p3, int count) {
    // Reserve space for additional points
    points.reserve(points.size() + count);

    // Generate points
    for(int i = 0; i < count; i++) {
      float t = (float)i/(count-1);
      auto a = lerp(p0, p1, t);
      auto b = lerp(p1, p2, t);
      auto c = lerp(p2, p3, t);
      auto ab = lerp(a, b, t);
      auto bc = lerp(b, c, t);
      auto point = lerp(ab, bc, t);
      points.push_back(point);
    }
  }

  // Compute points for a sequence of Bezier curves defined by a vector of control points
  // Each bezier curve will reuse the end point of the previous curve
  void Bezier(vector<vec2> &points, const vector<vec2> &cpoints, int count) {
    for(int i = 1; i < (int) cpoints.size(); i+=3)
      BezierCurve(points, cpoints[i - 1], cpoints[i], cpoints[i + 1], cpoints[i + 2], count);
  }
public:
  BezierWindow(string title, unsigned int width, unsigned int height) : Window{title, width, height} {
    // Generate Bezier curve points
    Bezier(points, cpoints, 15);

    // Generate a vertex array object
    // This keeps track of what attributes are associated with buffers
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec2), points.data(), GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "position" input
    auto position_attrib = program.GetAttribLocation("position");
    glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_attrib);

    // Generate a vertex array object
    // This keeps track of what attributes are associated with buffers
    glGenVertexArrays(1, &cvao);
    glBindVertexArray(cvao);

    // Generate a vertex buffer object, this will feed data to the vertex shader
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, cpoints.size() * sizeof(vec2), cpoints.data(), GL_STATIC_DRAW);

    // Setup vertex array lookup, this tells the shader how to pick data for the "position" input
    auto cposition_attrib = program.GetAttribLocation("position");
    glVertexAttribPointer(cposition_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(cposition_attrib);
  }

  ~BezierWindow() {
    glDeleteBuffers(1, &cvbo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &cvao);
    glDeleteVertexArrays(1, &vao);
  }

  void onPool() {
    // Set gray background
    glClearColor(0,0,0,0);

    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw control points, using red color
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glPointSize(10.0);
    glLineWidth(1.0);

    glBindVertexArray(cvao);
    program.SetVector(vec3{1,0,0}, "color");
    glDrawArrays(GL_POINTS, 0, (GLsizei) cpoints.size());
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) cpoints.size());

    // Draw lines using the program
    glLineWidth(10.0);
    glBindVertexArray(vao);
    program.SetVector(vec3{1,1,1}, "color");
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) points.size());

  }
};

int main() {
  // Create our window
  auto window = BezierWindow("tast3_bezier", SIZE, SIZE);

  // Main execution loop
  while (window.Pool()) {}

  return EXIT_SUCCESS;
}
