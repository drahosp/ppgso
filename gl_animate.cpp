// This example dynamically generates framebuffer content that is then
// fed to OpenGL for display as texture.
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SIZE = 512;

struct Pixel {
  unsigned char r,g,b;
};

Pixel framebuffer[SIZE][SIZE];

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
    // x, y
    1.0f, 1.0f,
      -1.0f, 1.0f,
      1.0f, -1.0f,
      -1.0f, -1.0f
  };

  // Generate a vertex buffer object
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), vertex_buffer.data(), GL_STATIC_DRAW);

  // Setup vertex array lookup
  auto position_attrib = glGetAttribLocation(program_id, "Position");
  glVertexAttribPointer(position_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(position_attrib);

  // Generate another vertex buffer object for texture coordinates
  std::vector<GLfloat> texcoord_buffer {
    // u, v
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

// Update framebuffer
void UpdateFramebuffer(float time) {
  // draw something to the buffer
  float cx = std::sin(time/20.0f);
  float cy = std::cos(time/19.0f);

  for(unsigned int x=0; x<SIZE; x++) {
    for(unsigned int y=0; y<SIZE; y++) {
      float fx = x/512.0f-.5;
      float fy = y/512.0f-.5;
      float dist = std::sqrt(std::pow(fx-cx, 2) + std::pow(fy-cy, 2));

      framebuffer[x][y].r = std::sin(dist*45.0f)*127+128;
      framebuffer[x][y].g = std::sin(dist*44.0f)*127+128;
      framebuffer[x][y].b = std::sin(dist*46.0f)*127+128;
    }
  }
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
  auto program_id = ShaderProgram("gl_texture.vert", "gl_texture.frag");
  glUseProgram(program_id);

  InitializeGeometry(program_id);

  // Create texture
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Bind texture
  auto texture_attrib = glGetUniformLocation(program_id, "Texture");
  glUniform1i(texture_attrib, 0);
  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Time counter
  float time = 0;

  // Main execution loop
  while (!glfwWindowShouldClose(window)) {
    // Update Framebuffer
    UpdateFramebuffer(time++);

    // Update texture data with framebuffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw triangles using the program
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
