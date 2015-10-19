#include "object_3d.h"

Object3D::Object3D(GLuint program_id, const std::string &obj_file, const std::string &image_file, unsigned int width, unsigned int height) {
  this->program_id = program_id;

  this->initGeometry(obj_file);
  this->initTexture(image_file, width, height);
}

void Object3D::initGeometry(const std::string &obj_file) {
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
  glUseProgram(this->program_id);

  // Generate a vertex array object
  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  // --- Vertex positions ---
  std::vector<GLfloat> vertex_buffer;
  for (int i = 0; i < (int) mesh.positions.size() / 3; i++) {
    vertex_buffer.push_back(mesh.positions[3 * i + 0]); // X
    vertex_buffer.push_back(mesh.positions[3 * i + 1]); // Y
    vertex_buffer.push_back(mesh.positions[3 * i + 2]); // Z
  }

  // Generate and upload a buffer with vertex positions to GPU
  glGenBuffers(1, &this->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "Position" attribute in program
  GLint position_attrib = glGetAttribLocation(this->program_id, "Position");
  glEnableVertexAttribArray(position_attrib);
  glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

  // --- Texture coordinates ---
  std::vector<GLfloat> texcoord_buffer;
  for (int i = 0; i < (int) mesh.texcoords.size() / 2; i++) {
    texcoord_buffer.push_back(mesh.texcoords[2 * i + 0]); // U
    texcoord_buffer.push_back(mesh.texcoords[2 * i + 1]); // V
  }

  // Generate and upload a buffer with texture coordinates to GPU
  glGenBuffers(1, &this->tbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->tbo);
  glBufferData(GL_ARRAY_BUFFER, texcoord_buffer.size() * sizeof(GLfloat), texcoord_buffer.data(), GL_STATIC_DRAW);

  // Bind the buffer to "TexCoord" attribute in program
  if (mesh.texcoords.size() > 0) {
    GLint texcoord_attrib = glGetAttribLocation(this->program_id, "TexCoord");
    glEnableVertexAttribArray(texcoord_attrib);
    glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  } else {
    std::cout << "Warning: OBJ file " << obj_file << " has no texture coordinates!" << std::endl;
  }

  // --- Indices (define which triangles consists of which vertices) ---
  std::vector<GLuint> index_data;
  this->mesh_indices_count = (int) mesh.indices.size();
  for (int i = 0; i < (int) mesh.indices.size(); i++) {
    index_data.push_back(mesh.indices[i]);
  }

  // Generate and upload a buffer with indices to GPU
  glGenBuffers(1, &this->ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(GLuint), index_data.data(), GL_STATIC_DRAW);

  // Complete the vertex array object
  glBindVertexArray(NULL);
}

// Load a new image from a raw RGB file directly into OpenGL memory
void Object3D::initTexture(const std::string &image_file, unsigned int width, unsigned int height) {
  // Create new texture object
  glGenTextures(1, &this->texture_id);
  glBindTexture(GL_TEXTURE_2D, this->texture_id);

  // Set mipmaps
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Read raw data
  std::ifstream image_stream(image_file, std::ios::binary);

  // Setup buffer for pixels (r,g,b bytes), since we will not manipulate the image just keep it as char
  std::vector<char> buffer(width * height * 3);
  image_stream.read(buffer.data(), buffer.size());
  image_stream.close();

  // Upload texture to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
}

void Object3D::render(glm::mat4 modelMatrix) {
  // Activate the program
  glUseProgram(this->program_id);

  // Bind the texture to "Texture" uniform in program
  GLint texture_attrib = glGetUniformLocation(this->program_id, "Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, this->texture_id);

  // Upload model matrix to GPU
  GLint model_uniform = glGetUniformLocation(this->program_id, "ModelMatrix");
  glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

  // Draw object
  glBindVertexArray(this->vao);
  glDrawElements(GL_TRIANGLES, this->mesh_indices_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(NULL);
}
