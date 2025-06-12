/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

/* ENUM TYOE
** The function type pf shader */
enum shaderFuncType {
  NORMAL,
  DEPTH,
  PARTICLE,
  DEBUG,
  BILLBOARD
};

/* ENUM TYPE
** For compilation option
** in private member function `compileErrLog`*/
enum ShaderCompileOption {
  PROGRAM,
  VERTEX,
  FRAGMENT,
  GEOMETRY
};

/* Shader type definition */
typedef GLuint shaderProgType;
typedef GLboolean shaderInstallType;

/* CLASS: Shader
** Used in GLSL-binding */
class Shader {
 public:
  /* Default constructor */
  Shader()
      : install_flag(false) {  // Do nothing here
  }

  /* Constructor with parameters which generates the shader */
  Shader(const char* vertex_shader_path,
         const char* fragment_shader_path)
      : install_flag(false) {
    reload(vertex_shader_path,
           fragment_shader_path);
  }

  /* Reload function
  ** If you need to load new shaders or reload shaders from other paths,
  ** especially when you declare a new shader object by default constructor
  ** (that means no parameter), this function is provided.*/
  void reload(const char* vertex_shader_path,
              const char* fragment_shader_path) {
    // Declare file variables
    // These variables are supposed to retrieve the source GLSL code
    std::string vs_code = loadCode(vertex_shader_path, VERTEX);
    std::string fs_code = loadCode(fragment_shader_path, FRAGMENT);
    const char* vert_shader_code = vs_code.c_str();
    const char* frag_shader_code = fs_code.c_str();

    // Create vertex and fragment Shader from source code string
    // Compile GLSL code and report error.
    shaderProgType vertex = glCreateShader(GL_VERTEX_SHADER);
    shaderProgType fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex, 1, &vert_shader_code, nullptr);
    glShaderSource(fragment, 1, &frag_shader_code, nullptr);
    glCompileShader(vertex);
    glCompileShader(fragment);
    compileErrLog(vertex, VERTEX);
    compileErrLog(fragment, FRAGMENT);

    // Create shader program and attach vertex & fragment shaders
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Link shader program and compile GLSL program
    // If error occurs, report it to console
    glLinkProgram(program);
    compileErrLog(program, PROGRAM);
  }

  /* Install the current shader */
  void install() {
    glUseProgram(program);
    install_flag = true;
  }

  /* Uninstall a shader */
  void uninstall() {
    // If a shader has been installed before and not uninstalled yet
    if (install_flag) {
      glUseProgram(0);
      install_flag = false;
    }  // Else print a WARNING to console
    else
      printf("WARNING: No shader INSTALLED!\n");
  }

  /* Returns the private members
  ** We set the shader settings private, because they are not supposed to be
  ** editted easily. If they need to be editted, call the `set*` functions, which makes
  ** sure you edit them on purpose, instead of unconsciously. */
  const shaderProgType getProgram() { return program; }
  const shaderFuncType getFuncType() { return func; }
  const shaderInstallType getInstallFlag() { return install_flag; }

  /* Set some private members */
  void setFuncType(shaderFuncType _func) { func = _func; }

  /* Other settings (used in GLSL-shader) */
  /* Set uniform matrix */
  void setUniformMatrix4fv(const char* name,
                           const glm::mat4& matrix) {
    GLint matrixID = glGetUniformLocation(program, name);
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  /* Set uniform3f */
  void setUniform3f(const char* name,
                    const glm::vec3& vector) {
    GLint vectorID = glGetUniformLocation(program, name);
    glUniform3f(vectorID, vector.x, vector.y, vector.z);
  }

  /* Set uniform 3fv */
  void setUniform3fv(const char* name,
                     const glm::vec3& vector) {
    GLint vectorID = glGetUniformLocation(program, name);
    glUniform3fv(vectorID, 1, glm::value_ptr(vector));
  }

  /* Set uniform 4f */
  void setUniform4f(const char* name,
                    const glm::vec4& vector) {
    GLint vectorID = glGetUniformLocation(program, name);
    glUniform4f(vectorID, vector.x, vector.y, vector.z, vector.w);
  }

  /* Set uniform 1f */
  void setUniform1f(const char* name,
                    const float& value) {
    GLint floatID = glGetUniformLocation(program, name);
    glUniform1f(floatID, value);
  }

  /* Set uniform 1i */
  void setUniform1i(const char* name,
                    const int& value) {
    GLint intID = glGetUniformLocation(program, name);
    glUniform1i(intID, value);
  }

 private:
  /* Load code string (pointer) from files.
  ** Return a pointer!
  ** PRIVATE member only viewed inside this class. */
  std::string loadCode(const char* path, ShaderCompileOption compile_option) {
    // @param shader_code: The string of shader code
    // @param shader_file: The string stream of shader code
    std::string shader_code;
    std::ifstream shader_file;

    // Throw exceptions (if error occurs)
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {  // Open and read code string
      shader_file.open(path);
      std::stringstream shader_stream;
      shader_stream << shader_file.rdbuf();
      shader_file.close();
      shader_code = shader_stream.str();
    } catch (const std::ifstream::failure& err) {  // Vertex or fragment shader file opening error
      fprintf(stderr, "ERROR: Cannot open shader file!\n");
      exit(1);
    }

    return shader_code;
  }

  /* Check whether the compilation succeed.
  ** @param shader_program: The shader or program id
  ** @param compile_option: ENUM TYPE
  **      PROGRAM, VERTEX, FRAGMENT, GEOMETRY
  **
  ** If the compilation failed, report it to the console */
  void compileErrLog(shaderProgType shader_program, ShaderCompileOption compile_option) {
    /* Map between enum type and strings */
    const char* enum_type_map[] =
        {
            "PROGRAM",
            "VERTEX",
            "FRAGMENT",
            "GEOMETRY"};

    // Compilation variables to report error log
    GLint compile_err;
    char* err_info_log = new char[1024];

    // Compile a shader program
    if (compile_option != PROGRAM) {
      glGetShaderiv(shader_program, GL_COMPILE_STATUS, &compile_err);
      if (!compile_err) {  // Output error log
        glGetShaderInfoLog(shader_program, sizeof(err_info_log), nullptr, err_info_log);
        fprintf(stderr, "ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
      }
    }  // Else compile vertex or fragment or geometry shader
    else {
      glGetProgramiv(shader_program, GL_LINK_STATUS, &compile_err);
      if (!compile_err) {  // Output error log
        glGetProgramInfoLog(shader_program, 1024, nullptr, err_info_log);
        fprintf(stderr, "ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
      }
    }
  }

  /* Shader program (id) */
  shaderProgType program;

  /* Determines whether the shader has been installed */
  shaderInstallType install_flag;

  /* Shader function type */
  shaderFuncType func;
};

#endif