/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <assimp/types.h>

/* CLASS: Texture */
class Texture
{
public:
    /* Default constructor */
    Texture()
        : level(0), border(0), type("diffuse"), path("./textures"), unit(0)
    { // Do nothing here
    }

    /* Load image and bind texture */
    Texture(const char* image_path)
    { // Constructor
        reload(image_path);
    }

    /* Reload image and rebind this texture */
    void reload(const char* image_path)
    {
        unsigned char* image = SOIL_load_image(image_path, &width, &height, 0, SOIL_LOAD_RGBA);
        // Make room for our texture
        glGenTextures(1, &id);
        // Tell OpenGL which texture to edit and map the image to the texture
        glBindTexture(GL_TEXTURE_2D, id);

        // ----------------------------------------------------------------------------------------
        // Specify a two-dimensional texture image
        // Learn more about this function:
        // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
        // ----------------------------------------------------------------------------------------
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA,
                     width, height, border,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);

        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        path = image_path;
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /* The function binds this texture */
    void bind(const int& index = 0)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    /* The function unbinds this texture */
    void unbind()
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /* Returns the private members
    ** We set the texture settings private, because they are not supposed to be
    ** editted easily. If they need to be editted, call the `set*` functions, which makes
    ** sure you edit them on purpose, instead of unconsciously. */
    const GLuint getID() { return id; }
    const std::string getType() { return type; }
    const aiString getPath() { return path; }
    const GLuint getUnit() { return unit; }
    const GLint getWidth() { return width; }
    const GLint getHeight() { return height; }

    /* Set some privat members */
    void setID(const GLuint& _id) { id = _id; }
    void setType(const std::string& _type) { type = _type; }
    void setPath(const aiString& _path) { path = _path; }
    void setUnit(const GLuint& _unit) { unit = _unit; }

private:

    /* PRIVATE MEMBER:
    ** The ID of this texture
    ** The most IMPORTANT member! */
    GLuint id;

    /* PRIVATE MEMBER:
    ** Specifies the level-of-detail number.
    ** Level 0 is the base image level.
    ** Level n is the nth mipmap reduction image.
    ** DEFAULT: 0 */
    GLint level;

    /* PRIVATE MEMBER:
    ** The border width of texture
    ** DEFAULT: 0 */
    GLint border;

    /* PRIVATE MEMBER:
    ** The width and height of the texture */
    GLint width, height;

    /* PRIVATE MEMBER:
    ** The type of texture: diffuse, specular, etc.
    ** DEFAULT: diffuse */
    std::string type;

    /* PRIVATE MEMBER:
    ** The path of the texture file */
    aiString path;

    /* PRIVATE MEMBER:
    ** The texture unit this texture binds */
    GLuint unit;

};

GLuint TextureFromFileDir(const char* path, std::string directory, bool gamma = false)
{
        //Generate texture ID and load texture data 
        std::string filename(path);
        filename = directory + '/' + filename;
        GLuint textureID;
        glGenTextures(1, &textureID);
        int width, height;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        return textureID;
}

//filename contains its directory
GLuint TextureFromFile(const char* filename, bool gamma = false) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        int width, height;
        unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

        // Assign texture to ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, gamma ? GL_SRGB : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return textureID;
}

#endif