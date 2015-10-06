// Example of 3D projection
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SIZE = 512;

GLuint ShaderProgram(const std::string &vertex_shader_file, const std::string &fragment_shader_file) {
  // Create shaders
  auto vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  auto fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  auto result = GL_FALSE;
  auto info_length = 0;

  // Load shader code
  std::ifstream vertex_shader_stream(vertex_shader_file);
  std::string vertex_shader_code((std::istreambuf_iterator<char>(vertex_shader_stream)), std::istreambuf_iterator<char>());

  std::ifstream fragment_shader_stream(fragment_shader_file);
  std::string fragment_shader_code((std::istreambuf_iterator<char>(fragment_shader_stream)), std::istreambuf_iterator<char>());

  // Compile vertex shader
  std::cout << "Compiling Vertex Shader ..." << std::endl;
  auto vertex_shader_code_ptr = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_shader_code_ptr, NULL);
  glCompileShader(vertex_shader_id);

  // Check vertex shader log
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string vertex_shader_log((unsigned int)info_length, ' ');
    glGetShaderInfoLog(vertex_shader_id, info_length, NULL, &vertex_shader_log[0]);
    std::cout << vertex_shader_log << std::endl;
  }

  // Compile fragment shader
  std::cout << "Compiling Fragment Shader ..." << std::endl;
  auto fragment_shader_code_ptr = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_shader_code_ptr, NULL);
  glCompileShader(fragment_shader_id);

  // Check fragment shader log
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string fragment_shader_log((unsigned long)info_length, ' ');
    glGetShaderInfoLog(fragment_shader_id, info_length, NULL, &fragment_shader_log[0]);
    std::cout << fragment_shader_log << std::endl;
  }

  // Create and link the program
  std::cout << "Linking Shader Program ..." << std::endl;
  auto program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glBindFragDataLocation(program_id, 0, "FragmentColor");
  glLinkProgram(program_id);

  // Check program log
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_length);
    std::string program_log((unsigned long)info_length, ' ');
    glGetProgramInfoLog(program_id, info_length, NULL, &program_log[0]);
    std::cout << program_log << std::endl;
  }
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  return program_id;
}

void InitializeGeometry(GLuint program_id) {
  // Generate a vertex array object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Setup geometry
  std::vector<GLfloat> vertex_buffer {
    // x, y, z
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,

    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
  };

  // Generate a vertex buffer object
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Setup vertex array lookup
  auto position_attrib = glGetAttribLocation(program_id, "Position");
  glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attrib);

  // Generate another vertex buffer object for texture coordinates
  std::vector<GLfloat> texcoord_buffer {
    // u, v
    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
  };

  GLuint tbo;
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, texcoord_buffer.size() * sizeof(GLfloat), texcoord_buffer.data(), GL_STATIC_DRAW);

  auto texcoord_attrib = glGetAttribLocation(program_id, "TexCoord");
  glVertexAttribPointer(texcoord_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(texcoord_attrib);
}

void InitializeProjection(GLuint program_id) {
  // Create matrices
  // Create projection matrix (field of view, aspect ratio, near plane distance, far plane distance)
  glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 10.0f);

  // Send projection matrix value to program
  auto projection_uniform = glGetUniformLocation(program_id, "ProjectionMatrix");
  glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(Projection));

  // Create view matrix (translate camera a bit backwards, so we can see the geometry)
  glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

  // Send view matrix value to program
  auto view_uniform = glGetUniformLocation(program_id, "ViewMatrix");
  glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(View));

  // Set up OpenGL options
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
}

// Load a new image from a raw RGB file directly into OpenGL memory
GLuint LoadImage(const std::string &image_file, unsigned int width, unsigned int height) {
  // Create new texture object
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Set mipmaps
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Read raw data
  std::ifstream image_stream(image_file, std::ios::binary);

  // Setup buffer for pixels (r,g,b bytes), since we will not manipulate the image just keep it as char
  std::vector<char> buffer(width*height*3);
  image_stream.read(buffer.data(), buffer.size());
  image_stream.close();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

  return texture_id;
}

void UpdateModelMatrix(GLuint program_id, float time) {
  float rotation = time / 180.0f;

  // Create model matrix (rotation around Y axis)
  glm::mat4 Model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));

  // Send model matrix value to program
  auto model_uniform = glGetUniformLocation(program_id, "ModelMatrix");
  glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(Model));
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
  auto window = glfwCreateWindow( SIZE, SIZE, "OpenGL", NULL, NULL);
  if (window == NULL) {
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

  // Load shaders
  auto program_id = ShaderProgram("gl_projection.vert", "gl_projection.frag");
  glUseProgram(program_id);

  InitializeGeometry(program_id);

  // Load and bind texture
  auto texture_id = LoadImage("lena.rgb", SIZE, SIZE);
  auto texture_attrib = glGetUniformLocation(program_id, "Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  InitializeProjection(program_id);

  // Time counter
  float time = 0;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Update model matrix (model rotation)
    UpdateModelMatrix(program_id, time++);

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
