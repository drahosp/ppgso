#include <glm/glm.hpp>

#include "mesh.h"
#include "tiny_obj_loader.h"

using namespace std;
using namespace glm;
using namespace ppgso;

Mesh::Mesh(shared_ptr<Shader> &program, const string &obj_file) : program{*program.get()} {
  initGeometry(obj_file);
}

Mesh::Mesh(Shader &program, const string &obj_file) : program{program} {
  initGeometry(obj_file);
}

void Mesh::initGeometry(const string &obj_file) {
  // Load OBJ file
  vector<tinyobj::shape_t> shapes;
  vector<tinyobj::material_t> materials;
  string err = tinyobj::LoadObj(shapes, materials, obj_file.c_str());

  if (!err.empty()) {
    cerr << err << endl;
    cerr << "Failed to load OBJ file " << obj_file << "!" << endl;
    return;
  }
  tinyobj::mesh_t mesh = shapes[0].mesh;

  // Activate the program
  program.Use();

  // Generate a vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // --- Vertex positions ---
  vector<vec3> vertex_buffer;
  for (int i = 0; i < (int)mesh.positions.size() / 3; i++)
    vertex_buffer.push_back(vec3{mesh.positions[3 * i], mesh.positions[3 * i + 1], mesh.positions[3 * i + 2]});

  // Generate and upload a buffer with vertex positions to GPU
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(vec3), vertex_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "Position" attribute in program
  auto position_attrib = program.GetAttribLocation("Position");
  glEnableVertexAttribArray(position_attrib);
  glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // --- Texture coordinates ---
  vector<vec2> texcoord_buffer;
  for (int i = 0; i < (int)mesh.texcoords.size() / 2; i++) {
    texcoord_buffer.push_back(vec2{mesh.texcoords[2 * i], mesh.texcoords[2 * i + 1]});
  }

  // Generate and upload a buffer with texture coordinates to GPU
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, texcoord_buffer.size() * sizeof(vec2), texcoord_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "TexCoord" attribute in program
  if (mesh.texcoords.size() > 0) {
    auto texcoord_attrib = program.GetAttribLocation("TexCoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    cout << "Warning: OBJ file " << obj_file << " has no texture coordinates!" << endl;
  }

  // --- Normals ---
  vector<vec3> normal_buffer;
  for (int i = 0; i < (int)mesh.normals.size() / 3; i++) {
    normal_buffer.push_back(vec3{mesh.normals[3 * i], mesh.normals[3 * i + 1], mesh.normals[3 * i + 2]});
  }

  // Generate and upload a buffer with texture coordinates to GPU
  glGenBuffers(1, &nbo);
  glBindBuffer(GL_ARRAY_BUFFER, nbo);
  glBufferData(GL_ARRAY_BUFFER, normal_buffer.size() * sizeof(vec3), normal_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "Normal" attribute in program
  if (mesh.normals.size() > 0) {
    auto normal_attib = program.GetAttribLocation("Normal");
    glEnableVertexAttribArray(normal_attib);
    glVertexAttribPointer(normal_attib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    cout << "Warning: OBJ file " << obj_file << " has no normals!" << endl;
  }

  // --- Indices (define which triangles consists of which vertices) ---
  vector<GLuint> index_data;
  mesh_indices_count = (int)mesh.indices.size();
  for (int i = 0; i < (int)mesh.indices.size(); i++) {
    index_data.push_back(mesh.indices[i]);
  }

  // Generate and upload a buffer with indices to GPU
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(GLuint), index_data.data(), GL_STATIC_DRAW);
}

void Mesh::Render() {
  // Draw object
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, mesh_indices_count, GL_UNSIGNED_INT, 0);
}
