// Example raw4_raster
// - This example implements a very simple software rasterizer that mimics parts of the OpenGL pipeline with vertex and fragment shaders
// - Some of the pipeline steps such as culling, clipping were skipped for simplicity and readability
// - Triangle rendering is realized using horizontal triangle splitting and filling is implemented using linear interpolation

#include <iostream>
#include <ppgso/ppgso.h>
#include <glm/gtx/euler_angles.hpp>

using namespace std;
using namespace glm;
using namespace ppgso;

/*!
 * Vertex structure to hold per vertex data in
 */
struct Vertex {
  vec4 position;
  vec4 normal;
  vec2 texCoord;
  vec4 color;
};

/*!
 * Vertex interpolation function that interpolates position, normal, texCoord and color vectors between two vertices
 * @param v0 First vertex
 * @param v1 Second vertex
 * @param t Interpolation amount, range <0,1>
 * @return Linear combination of v0 and v1
 */
Vertex lerp(const Vertex &v0, const Vertex &v1, float t) {
  auto z = lerp(1.0f / v0.position.z, 1.0f / v1.position.z, t);
  auto texCoord = lerp(v0.texCoord / v0.position.z, v1.texCoord / v1.position.z, t) / z;
  return Vertex{
      lerp(v0.position, v1.position, t),
      lerp(v0.normal, v1.normal, t),
      texCoord,
      lerp(v0.color, v1.color, t)
  };
}

/*!
 * Face structure to hold three vertices that form a triangle/face
 */
struct Face {
  Vertex v0, v1, v2;
};

class Program {
public:
  /*!
   * Program constructor that expects texture reference
   */
  Program(Image &texture) : texture{texture} {};

  // Uniform inputs common for all vertices
  Image &texture;
  mat4 modelMatrix;
  mat4 viewMatrix;
  mat4 projectionMatrix;

  /*!
   * Vertex shader is a program that can manipulate vertex data, typically changing the vertex position using a perspective projection matrix.
   * @param vertex Vertex to manipulate.
   * @return Output vertex, position on screen is expected to be in the <-1,1> range for x and y coordinates.
   */
  Vertex vertexShader(const Vertex &vertex) {
    // Transform the vertex position to world coordinates
    vec4 worldCoordinates = modelMatrix * vertex.position;
    // Transform the position to camera coordinates
    vec4 cameraCoordinates = viewMatrix * worldCoordinates;
    // Project the camera coordinates to screen coordinates
    vec4 screenCoordinates = projectionMatrix * cameraCoordinates;
    // Multiply normal with modelMatrix so that normals are always in world coordinates.
    // Pass on color and texture coordinates unchanged.
    return Vertex{screenCoordinates, modelMatrix * vertex.normal, vertex.texCoord, vertex.color};
  };

  /*!
   * Fragment shader is a program that is responsible for generating the final output color for each fragment, in this case we have 1 fragment per pixel.
   * @param varying Varying vertex data that is interpolated from the triangle vertices
   * @return Fragment color
   */
  vec4 fragmentShader(const Vertex &varying) {
    // Simple directional light
    float lighting = 1; //std::max(0.0f, (float) dot(varying.normal, vec4{.5f, .5f, .5f, 0.0f}));
    // Compute output color
    return varying.color * lighting * sample(texture, varying.texCoord);
  };
private:
  /*!
   * Get a color sample from image for given normalized texture coordinates
   * @param image Image to obtain raw color information from.
   * @param textCoord Normalized 2D coordinates to get color sample from.
   * @return
   */
  vec4 sample(Image &image, vec2 textCoord) {
    // Get the appropriate pixel for given texture coordinates.
    textCoord = clamp(textCoord, 0.0f, 1.0f);
    auto x = (int) (textCoord.x * (image.width - 1));
    auto y = (int) (textCoord.y * (image.height - 1));
    // NOTE: The coordinates are vertically inverted for compatibility with object files generated using Blender 3D.
    auto pixel = image.getPixel(x, image.height-y);
    // Return normalized color vector
    return vec4{pixel.r / 255.0f, pixel.g / 255.0f, pixel.b / 255.0f, 1.0};
  }
};

/*!
 * Simple rasterizer class that can render triangles into an image
 */
class Rasterizer {
private:
  Program &program;
  Image &image;
  vector<float> depthBuffer;

  /*!
   * Transform a vertex from screen coordinates to viewport/image coordinates
   * @param vertex Vertex to transform to viewport. The visible range is <-1,1> for x and y coordinates
   * @return Vertex that has position transformed to viewport/image coordinates
   */
  Vertex toViewport(const Vertex &vertex) {
    // Matrix that aligns the screen coordinates to viewport coordinates
    static const mat4 viewportMatrix = glm::translate(glm::scale(mat4{1.0f}, vec3{image.width / 2.0, -image.height / 2.0, 1.0}), vec3{1, -1, 0});
    // First convert homogeneous coordinates to cartesian and transform to viewport
    vec4 viewportCoordinates = viewportMatrix * (vertex.position / vertex.position.w);
    // Copy rest of the data without change
    return Vertex{viewportCoordinates, vertex.normal, vertex.texCoord, vertex.color};
  }

  /*!
   * Set the pixel in the output using the varying data stored in Vertex
   * @param x Fragment horizontal position
   * @param y Fragment vertical position
   * @param varying Varying vertex data to pass to fragment shader which will generate the pixel color
   */
  void setFragment(const Vertex &varying) {
    int x = (int) varying.position.x;
    int y = (int) varying.position.y;
    // Do not render pixels outside of the image
    if (x < 0 || y < 0 || x >= image.width || y >= image.height)
      return;

    // Check and update the depth buffer
    if (depthBuffer[x + y * image.width] < varying.position.z)
      return;

    depthBuffer[x + y * image.height] = varying.position.z;

    // Compute the fragment color and limit the output
    vec4 color = clamp(program.fragmentShader(varying), 0.0f, 1.0f);
    image.setPixel(x, y, color.r, color.g, color.b);
  }

  /*!
   * Fill the top portion of a triangle that has split horizontally by using interpolation between vertices
   * @param v0 - Top vertex of the triangle
   * @param v1 - First bottom vertex
   * @param v2 - Second bottom vertex
   */
  void renderTopTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
    for (int y = 0; v0.position.y + y <= v2.position.y; ++y) {
      float yt = v0.position.y >= v2.position.y ? 0 : y / (v2.position.y - v0.position.y);
      Vertex a = lerp(v0, v1, yt);
      Vertex b = lerp(v0, v2, yt);
      if (a.position.x > b.position.x) swap(a, b);
      for (int x = 0; a.position.x + x <= b.position.x; ++x) {
        float xt = a.position.x >= b.position.x ? 0 : x / (b.position.x - a.position.x);
        Vertex varying = lerp(a, b, xt);
        setFragment(varying);
      }
    }
  }

  /*!
   * Fill the bottom portion of a triangle that has split horizontally by using interpolation between vertices
   * @param v0 - First top vertex of the triangle
   * @param v1 - Second top vertex
   * @param v2 - Bottom vertex
   */
  void renderBottomTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2) {
    for (int y = 0; v0.position.y + y <= v2.position.y; ++y) {
      float yt =  v0.position.y >= v2.position.y ? 0 : y / (v2.position.y - v0.position.y);
      Vertex a = lerp(v0, v2, yt);
      Vertex b = lerp(v1, v2, yt);
      if (a.position.x > b.position.x) swap(a, b);
      for (int x = 0; a.position.x + x <= b.position.x; ++x) {
        float xt = a.position.x >= b.position.x ? 0 : x / (b.position.x - a.position.x);
        Vertex varying = lerp(a, b, xt);
        setFragment(varying);
      }
    }
  }

public:
  /*!
   * Initialize the rasterizer
   * @param image Image to render to
   * @param program Program to use for rendering
   */
  Rasterizer(Image &image, Program &program) : program{program}, image{image} {
    clear();
  };

  /*!
   * Clear depth buffer and image
   */
  void clear() {
    // Clear the depth buffer
    depthBuffer = vector<float>((unsigned long) (image.width * image.height), numeric_limits<float>::max());
    // Clear the image
    image.clear({128,128,128});
  }

  /*!
   * Render a face into the image
   * @param face Face to render
   */
  void render(const Face &face) {
    // transform vertices
    Vertex t0 = toViewport(program.vertexShader(face.v0));
    Vertex t1 = toViewport(program.vertexShader(face.v1));
    Vertex t2 = toViewport(program.vertexShader(face.v2));
    // Sort the vertices
    if (t0.position.y > t1.position.y) swap(t0, t1);
    if (t0.position.y > t2.position.y) swap(t0, t2);
    if (t1.position.y > t2.position.y) swap(t1, t2);
    // Split the triangle into top/bottom sections
    float t = t0.position.y >= t2.position.y ? 0 : (t1.position.y - t0.position.y) / (t2.position.y - t0.position.y);
    Vertex tm = lerp(t0, t2, t);

    // Render
    renderTopTriangle(t0, tm, t1);
    renderBottomTriangle(t1, tm, t2);
  }
};

/*!
 * Load Wavefront obj file data as vector of faces for simplicity
 * @return vector of Faces that can be rendered
 */
vector<Face> loadObjFile(const string filename) {
  // Using tiny obj loader from ppgso lib
  vector<tinyobj::shape_t> shapes;
  vector<tinyobj::material_t> materials;
  string err = tinyobj::LoadObj(shapes, materials, filename.c_str());

  // Will only convert 1st shape to Faces
  auto &mesh = shapes[0].mesh;

  // Collect data in vectors
  vector<vec4> positions;
  for (int i = 0; i < (int) mesh.positions.size() / 3; ++i)
    positions.emplace_back(mesh.positions[3 * i], mesh.positions[3 * i + 1], mesh.positions[3 * i + 2], 1);

  vector<vec4> normals;
  for (int i = 0; i < (int) mesh.normals.size() / 3; ++i)
    normals.emplace_back(mesh.normals[3 * i], mesh.normals[3 * i + 1], mesh.normals[3 * i + 2], 1);

  vector<vec2> texcoords;
  for (int i = 0; i < (int) mesh.texcoords.size() / 2; ++i)
    texcoords.emplace_back(mesh.texcoords[2 * i], mesh.texcoords[2 * i + 1]);

  // Fill the vector of Faces with data
  vector<Face> faces(mesh.indices.size() / 3);
  for (int i = 0; i < (int) faces.size(); i++) {
    faces[i] = Face{
        {
            positions[mesh.indices[i * 3]],
            normals[mesh.indices[i * 3]],
            texcoords[mesh.indices[i * 3]],
            {1, 1, 1, 1}
        }, {
            positions[mesh.indices[i * 3 + 1]],
            normals[mesh.indices[i * 3 + 1]],
            texcoords[mesh.indices[i * 3 + 1]],
            {1, 1, 1, 1}
        }, {
            positions[mesh.indices[i * 3 + 2]],
            normals[mesh.indices[i * 3 + 2]],
            texcoords[mesh.indices[i * 3 + 2]],
            {1, 1, 1, 1}
        }
    };
  }
  return faces;
};

int main() {
  // Image to store the rendering to
  Image image{512, 512};
  // Vector of faces loaded from Wavefront obj file
  auto faces = loadObjFile("corsair.obj");
  // Image to use as texture in the shader program
  Image texture{image::loadBMP("corsair.bmp")};
  // Shader program to use
  Program program{texture};
  // Set program uniforms
  program.modelMatrix = orientate4(vec3{0,0.4,.8});
  program.viewMatrix = lookAt(vec3{0,.7,.7}, vec3{0,0,0}, vec3{.5, .5, 0});
  program.projectionMatrix = perspective((PI / 180.f) * 60.0f, (float)image.width / (float)image.height, 1.0f, 15.0f);

  // Rasterizer instance
  Rasterizer rasterizer{image, program};

  // Render all faces
  for (auto &face : faces)
    rasterizer.render(face);

  // Save the image
  image::saveBMP(image, "raw4_raster.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}
