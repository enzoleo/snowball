/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdio.h>
#include <stdlib.h>
#include "GL/glew.h"

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
           const char* geometry_shader_path = NULL)
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
                const char* geometry_shader_path = NULL)
    {
        // Declare file variables
        // These variables are supposed to retrieve the source GLSL code
        char* vert_shader_code = loadCode(vertex_shader_path, VERTEX);
        char* frag_shader_code = loadCode(fragment_shader_path, FRAGMENT);
        char* geom_shader_code = loadCode(geometry_shader_path, GEOMETRY);
        
        // Create vertex and fragment Shader from source code string
        // Compile GLSL code and report error.
        shaderProgType vertex = glCreateShader(GL_VERTEX_SHADER);
        shaderProgType fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertex, 1, &vert_shader_code, NULL);
        glShaderSource(fragment, 1, &frag_shader_code, NULL);
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
        free(vert_shader_code);
        free(frag_shader_code);

        // If geometry shader exists
        if (geom_shader_code)
        {
            shaderProgType geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &geom_shader_code, NULL);
            glCompileShader(geometry);
            compileErrLog(geometry, GEOMETRY);
            glAttachShader(program, geometry);
            glDeleteShader(geometry);
            free(geom_shader_code);
        }

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
        char* shader_code = NULL;
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
                glGetShaderInfoLog(shader_program, sizeof(err_info_log), NULL, err_info_log);
                printf("ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
            }
        } // Else compile vertex or fragment or geometry shader
        else
        {
            glGetProgramiv(shader_program, GL_LINK_STATUS, &compile_err);
            if (!compile_err)
            { // Output error log
                glGetProgramInfoLog(shader_program, 1024, NULL, err_info_log);
                printf("ERROR: %s\n%s\n", enum_type_map[compile_option], err_info_log);
            }
        }
    }

    /* Shader program (id) */
    shaderProgType program;

    /* Determines whether the shader has been installed */
    shaderInstallType install_flag;
};

#endif
