//#ifndef SHADER_H
//#define SHADER_H
//
//#include <GL/glew.h>
//
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <iostream>
//
//class Shader
//{
//public:
//    GLuint Program;
//	Shader() {}
//    // Constructor generates the shader on the fly
//    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr)
//    {
//        // 1. Retrieve the vertex/fragment source code from filePath
//        std::string vertexCode;
//        std::string fragmentCode;
//        std::string geometryCode;
//        std::ifstream vShaderFile;
//        std::ifstream fShaderFile;
//        std::ifstream gShaderFile;
//        // ensures ifstream objects can throw exceptions:
//        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//        try 
//        {
//            // Open files
//            vShaderFile.open(vertexPath);
//            fShaderFile.open(fragmentPath);
//            std::stringstream vShaderStream, fShaderStream;
//            // Read file's buffer contents into streams
//            vShaderStream << vShaderFile.rdbuf();
//            fShaderStream << fShaderFile.rdbuf();		
//            // close file handlers
//            vShaderFile.close();
//            fShaderFile.close();
//            // Convert stream into string
//            vertexCode = vShaderStream.str();
//            fragmentCode = fShaderStream.str();			
//			// If geometry shader path is present, also load a geometry shader
//			if(geometryPath != nullptr)
//			{
//                gShaderFile.open(geometryPath);
//                std::stringstream gShaderStream;
//				gShaderStream << gShaderFile.rdbuf();
//				gShaderFile.close();
//				geometryCode = gShaderStream.str();
//			}
//        }
//        catch (std::ifstream::failure e)
//        {
//            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//        }
//        const GLchar* vShaderCode = vertexCode.c_str();
//        const GLchar * fShaderCode = fragmentCode.c_str();
//        // 2. Compile shaders
//        GLuint vertex, fragment;
//        GLint success;
//        GLchar infoLog[512];
//        // Vertex Shader
//        vertex = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vertex, 1, &vShaderCode, nullptr);
//        glCompileShader(vertex);
//        checkCompileErrors(vertex, "VERTEX");
//        // Fragment Shader
//        fragment = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragment, 1, &fShaderCode, nullptr);
//        glCompileShader(fragment);
//		checkCompileErrors(fragment, "FRAGMENT");
//		// If geometry shader is given, compile geometry shader
//		GLuint geometry;
//		if(geometryPath != nullptr)
//		{
//			const GLchar * gShaderCode = geometryCode.c_str();
//			geometry = glCreateShader(GL_GEOMETRY_SHADER);
//			glShaderSource(geometry, 1, &gShaderCode, nullptr);
//			glCompileShader(geometry);
//			checkCompileErrors(geometry, "GEOMETRY");
//		}
//        // Shader Program
//        this->Program = glCreateProgram();
//        glAttachShader(this->Program, vertex);
//        glAttachShader(this->Program, fragment);
//		if(geometryPath != nullptr)
//			glAttachShader(this->Program, geometry);
//        glLinkProgram(this->Program);
//        checkCompileErrors(this->Program, "PROGRAM");
//        // Delete the shaders as they're linked into our program now and no longer necessery
//        glDeleteShader(vertex);
//        glDeleteShader(fragment);
//		if(geometryPath != nullptr)
//			glDeleteShader(geometry);
//
//    }
//    // Uses the current shader
//    void Use() { glUseProgram(this->Program); }
//	void Load(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr) 
//	{
//		// 1. Retrieve the vertex/fragment source code from filePath
//		std::string vertexCode;
//		std::string fragmentCode;
//		std::string geometryCode;
//		std::ifstream vShaderFile;
//		std::ifstream fShaderFile;
//		std::ifstream gShaderFile;
//		// ensures ifstream objects can throw exceptions:
//		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//		try
//		{
//			// Open files
//			vShaderFile.open(vertexPath);
//			fShaderFile.open(fragmentPath);
//			std::stringstream vShaderStream, fShaderStream;
//			// Read file's buffer contents into streams
//			vShaderStream << vShaderFile.rdbuf();
//			fShaderStream << fShaderFile.rdbuf();
//			// close file handlers
//			vShaderFile.close();
//			fShaderFile.close();
//			// Convert stream into string
//			vertexCode = vShaderStream.str();
//			fragmentCode = fShaderStream.str();
//			// If geometry shader path is present, also load a geometry shader
//			if (geometryPath != nullptr)
//			{
//				gShaderFile.open(geometryPath);
//				std::stringstream gShaderStream;
//				gShaderStream << gShaderFile.rdbuf();
//				gShaderFile.close();
//				geometryCode = gShaderStream.str();
//			}
//		}
//		catch (std::ifstream::failure e)
//		{
//			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//		}
//		const GLchar* vShaderCode = vertexCode.c_str();
//		const GLchar * fShaderCode = fragmentCode.c_str();
//		// 2. Compile shaders
//		GLuint vertex, fragment;
//		GLint success;
//		GLchar infoLog[512];
//		// Vertex Shader
//		vertex = glCreateShader(GL_VERTEX_SHADER);
//		glShaderSource(vertex, 1, &vShaderCode, nullptr);
//		glCompileShader(vertex);
//		checkCompileErrors(vertex, "VERTEX");
//		// Fragment Shader
//		fragment = glCreateShader(GL_FRAGMENT_SHADER);
//		glShaderSource(fragment, 1, &fShaderCode, nullptr);
//		glCompileShader(fragment);
//		checkCompileErrors(fragment, "FRAGMENT");
//		// If geometry shader is given, compile geometry shader
//		GLuint geometry;
//		if (geometryPath != nullptr)
//		{
//			const GLchar * gShaderCode = geometryCode.c_str();
//			geometry = glCreateShader(GL_GEOMETRY_SHADER);
//			glShaderSource(geometry, 1, &gShaderCode, nullptr);
//			glCompileShader(geometry);
//			checkCompileErrors(geometry, "GEOMETRY");
//		}
//		// Shader Program
//		this->Program = glCreateProgram();
//		glAttachShader(this->Program, vertex);
//		glAttachShader(this->Program, fragment);
//		if (geometryPath != nullptr)
//			glAttachShader(this->Program, geometry);
//		glLinkProgram(this->Program);
//		checkCompileErrors(this->Program, "PROGRAM");
//		// Delete the shaders as they're linked into our program now and no longer necessery
//		glDeleteShader(vertex);
//		glDeleteShader(fragment);
//		if (geometryPath != nullptr)
//			glDeleteShader(geometry);
//	}
//
//	GLuint getProgram() { return Program; }
//
//	void install() { glUseProgram(Program); }
//	void uninstall() { glUseProgram(0); }
//
//private:
//    void checkCompileErrors(GLuint shader, std::string type)
//	{
//		GLint success;
//		GLchar infoLog[1024];
//		if(type != "PROGRAM")
//		{
//			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//			if(!success)
//			{
//				glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
//                std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
//			}
//		}
//		else
//		{
//			glGetProgramiv(shader, GL_LINK_STATUS, &success);
//			if(!success)
//			{
//				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
//                std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
//			}
//		}
//	}
//};
//
//#endif

/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 


enum shaderFuncType
{
	NORMAL,
	DEPTH,
	PARTICLE,
	DEBUG,
	BILLBOARD
};

/* ENUM TYPE
** For compilation option
** in private member function `compileErrLog`*/
enum ShaderCompileOption
{
	PROGRAM,
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

/* Shader type definition */
typedef GLuint shaderProgType;
typedef bool shaderInstallType;

/* CLASS: Shader
** Used in GLSL-binding */
class Shader
{
public:

	/* Default constructor */
	Shader()
		: install_flag(false)
	{ // Do nothing here
	}

	/* Constructor with parameters which generates the shader */
	Shader(const char* vertex_shader_path,
		const char* fragment_shader_path,
		const char* geometry_shader_path = nullptr)
		: install_flag(false)
	{
		reload(vertex_shader_path,
			fragment_shader_path,
			geometry_shader_path);
	}

	/* Reload function
	** If you need to load new shaders or reload shaders from other paths,
	** especially when you declare a new shader object by default constructor
	** (that means no parameter), this function is provided.*/
	void reload(const char* vertex_shader_path,
		const char* fragment_shader_path,
		const char* geometry_shader_path = nullptr)
	{
		// Declare file variables
		// These variables are supposed to retrieve the source GLSL code
		char* vert_shader_code = loadCode(vertex_shader_path, VERTEX);
		char* frag_shader_code = loadCode(fragment_shader_path, FRAGMENT);
		//char* geom_shader_code = loadCode(geometry_shader_path, GEOMETRY);

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

		// If geometry shader exists
		/*if (geom_shader_code)
		{
			shaderProgType geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &geom_shader_code, nullptr);
			glCompileShader(geometry);
			compileErrLog(geometry, GEOMETRY);
			glAttachShader(program, geometry);
			glDeleteShader(geometry);
		}
*/
		// Link shader program and compile GLSL program
		// If error occurs, report it to console
		glLinkProgram(program);
		compileErrLog(program, PROGRAM);
	}

	/* Install the current shader */
	void install()
	{
		glUseProgram(program);
		install_flag = true;
	}

	/* Uninstall a shader */
	void uninstall()
	{
		// If a shader has been installed before and not uninstalled yet 
		if (install_flag)
		{
			glUseProgram(0);
			install_flag = false;
		} // Else print a WARNING to console
		else printf("WARNING: No shader INSTALLED!\n");
	}

	const shaderProgType getProgram()
	{ // Return private member @program
		return program;
	}

	const shaderFuncType getFuncType()
	{
		return func;
	}

	void setFuncType(shaderFuncType _func)
	{
		func = _func;
	}

	void setUniformMatrix4fv(const char* name, const glm::mat4& matrix) {
		GLint matrixID = glGetUniformLocation(program, name);
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void setUniform3f(const char* name, const glm::vec3& vector) {
		GLint vectorID = glGetUniformLocation(program, name);
		glUniform3f(vectorID, vector.x, vector.y, vector.z);
	}
	void setUniform4f(const char* name, const glm::vec4& vector) {
		GLint vectorID = glGetUniformLocation(program, name);
		glUniform4f(vectorID, vector.x, vector.y, vector.z, vector.w);
	}
	void setUniform1f(const char* name, const float& value) {
		GLint floatID = glGetUniformLocation(program, name);
		glUniform1f(floatID, value);
	}
	void setUniform1i(const char* name, const int& value) {
		GLint intID = glGetUniformLocation(program, name);
		glUniform1i(intID, value);
	}


private:

	/* Load code string (pointer) from files.
	** Return a pointer!
	** PRIVATE member only viewed inside this class. */
	char* loadCode(const char* path, ShaderCompileOption compile_option)
	{
		// Declare file variables
		// These variables are supposed to retrieve the source GLSL code
		FILE *shader_in = fopen(path, "rb");
		if (!shader_in && compile_option != GEOMETRY)
		{ // Vertex or fragment shader file opening error
			printf("ERROR: Cannot open VERTEX or FRAGMENT shader file.\n");
			exit(1);
		}

		// Declare the code string
		char* shader_code = nullptr;
		// The the shader exists
		if (shader_in)
		{
			// Get the length of the shader source code string
			// Malloc space of code string (pointer)
			fseek(shader_in, 0, SEEK_END);
			int shader_in_len = ftell(shader_in);
			shader_code = (char*)malloc(sizeof(char) * shader_in_len);
			memset(shader_code, 0, shader_in_len + 1);

			// Read the source code to our string (pointer)
			fseek(shader_in, 0, SEEK_SET);
			fread(shader_code, shader_in_len, sizeof(char), shader_in);
			fclose(shader_in);
		}
		return shader_code;
	}

	/* Check whether the compilation succeed.
	** @param shader_program: The shader or program id
	** @param compile_option: ENUM TYPE
	**      PROGRAM, VERTEX, FRAGMENT, GEOMETRY
	**
	** If the compilation failed, report it to the console */
	void compileErrLog(shaderProgType shader_program, ShaderCompileOption compile_option)
	{
		/* Map between enum type and strings */
		const char* enum_type_map[] =
		{
			"PROGRAM",
			"VERTEX",
			"FRAGMENT",
			"GEOMETRY"
		};

		// Compilation variables to report error log
		GLint compile_err;
		char* err_info_log = (char*)malloc(1024 * sizeof(char));

		// Compile a shader program
		if (compile_option != PROGRAM)
		{
			glGetShaderiv(shader_program, GL_COMPILE_STATUS, &compile_err);
			if (!compile_err)
			{ // Output error log
				glGetShaderInfoLog(shader_program, sizeof(err_info_log), nullptr, err_info_log);
				printf("ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
			}
		} // Else compile vertex or fragment or geometry shader
		else
		{
			glGetProgramiv(shader_program, GL_LINK_STATUS, &compile_err);
			if (!compile_err)
			{ // Output error log
				glGetProgramInfoLog(shader_program, 1024, nullptr, err_info_log);
				printf("ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
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
