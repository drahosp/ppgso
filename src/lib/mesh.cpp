#include "mesh.h"
#include "tiny_obj_loader.h"

Mesh::Mesh(ShaderPtr program, const std::string &obj_file) {
  this->program = program;
  this->initGeometry(obj_file);
}

Mesh::Mesh(ShaderPtr program, const std::string &obj_file, const TexturePtr texture) {
  Mesh(program, obj_file);
  this->texture = texture;
}

void Mesh::initGeometry(const std::string &obj_file) {
  // Load OBJ file
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err = tinyobj::LoadObj(shapes, materials, obj_file.c_str());

  if (!err.empty()) {
    std::cerr << err << std::endl;
    std::cerr << "Failed to load OBJ file " << obj_file << "!" << std::endl;
    return;
  }
  tinyobj::mesh_t mesh = shapes[0].mesh;

  // Activate the program
  program->Use();

  // Generate a vertex array object
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  // --- Vertex positions ---
  std::vector<GLfloat> vertex_buffer;
  for (int i = 0; i < (int)mesh.positions.size() / 3; i++) {
    vertex_buffer.push_back(mesh.positions[3 * i + 0]); // X
    vertex_buffer.push_back(mesh.positions[3 * i + 1]); // Y
    vertex_buffer.push_back(mesh.positions[3 * i + 2]); // Z
  }

  // Generate and upload a buffer with vertex positions to GPU
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat),
               vertex_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "Position" attribute in program
  auto position_attrib = program->GetAttribLocation("Position");
  glEnableVertexAttribArray(position_attrib);
  glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // --- Texture coordinates ---
  std::vector<GLfloat> texcoord_buffer;
  for (int i = 0; i < (int)mesh.texcoords.size() / 2; i++) {
    texcoord_buffer.push_back(mesh.texcoords[2 * i + 0]); // U
    texcoord_buffer.push_back(mesh.texcoords[2 * i + 1]); // V
  }

  // Generate and upload a buffer with texture coordinates to GPU
  glGenBuffers(1, &this->tbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->tbo);
  glBufferData(GL_ARRAY_BUFFER, texcoord_buffer.size() * sizeof(GLfloat),
               texcoord_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "TexCoord" attribute in program
  if (mesh.texcoords.size() > 0) {
    auto texcoord_attrib = program->GetAttribLocation("TexCoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    std::cout << "Warning: OBJ file " << obj_file
              << " has no texture coordinates!" << std::endl;
  }

  // --- Normals ---
  std::vector<GLfloat> normal_buffer;
  for (int i = 0; i < (int)mesh.normals.size() / 3; i++) {
    normal_buffer.push_back(mesh.normals[3 * i + 0]); // X
    normal_buffer.push_back(mesh.normals[3 * i + 1]); // Y
    normal_buffer.push_back(mesh.normals[3 * i + 2]); // Z
  }

  // Generate and upload a buffer with texture coordinates to GPU
  glGenBuffers(1, &this->nbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
  glBufferData(GL_ARRAY_BUFFER, normal_buffer.size() * sizeof(GLfloat),
               normal_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "Normal" attribute in program
  if (mesh.normals.size() > 0) {
    auto normal_attib = program->GetAttribLocation("Normal");
    glEnableVertexAttribArray(normal_attib);
    glVertexAttribPointer(normal_attib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    std::cout << "Warning: OBJ file " << obj_file
    << " has no normals!" << std::endl;
  }

  // --- Indices (define which triangles consists of which vertices) ---
  std::vector<GLuint> index_data;
  this->mesh_indices_count = (int)mesh.indices.size();
  for (int i = 0; i < (int)mesh.indices.size(); i++) {
    index_data.push_back(mesh.indices[i]);
  }

  // Generate and upload a buffer with indices to GPU
  glGenBuffers(1, &this->ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(GLuint),
               index_data.data(), GL_STATIC_DRAW);

  // Complete the vertex array object
//  glBindVertexArray(0);
}

void Mesh::Render() {
  // Draw object
  glBindVertexArray(this->vao);
  glDrawElements(GL_TRIANGLES, this->mesh_indices_count, GL_UNSIGNED_INT, 0);
//  glBindVertexArray(0);
}
