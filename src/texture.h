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
#include <assimp/types.h>
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Data type redefinition */
//typedef GLuint textureIDtype;

/* CLASS: Texture */
class Texture
{
public:
    /* Default constructor */
    Texture() : level(0), border(0), type("texture_diffuse"), path("./textures")
    { // Do nothing here
    }

    /* Load image and bind texture */
    Texture(const char* image_path)
    { // Constructor
        //SDL_Surface *surface = IMG_Load(image_path);
        int width, height;
        unsigned char* image = SOIL_load_image(image_path, &width, &height, 0, SOIL_LOAD_RGB);
        // Make room for our texture
        glGenTextures(1, &id);
        // Tell OpenGL which texture to edit and map the image to the texture
        glBindTexture(GL_TEXTURE_2D, id);

        // --------------------------------------------------------------------------------
        // Specify a two-dimensional texture image
        // Learn more about this function:
        // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
        // --------------------------------------------------------------------------------
        glTexImage2D(GL_TEXTURE_2D, // Specifies the target texture of the active texture unit
                     level,  // Specifies the level-of-detail number. Here the basic level is needed
                     GL_RGB, // Specifies the internal format of the texture
                     width, // Image width
                     height, // Image height
                     border, // The border width of the image
                     GL_RGB, // GL_RGB, because pixels are stored in RGB format
                     GL_UNSIGNED_BYTE, // TYPE (Specifies the data type of the texel data) = GL_UNSIGNED_BYTE
                     image); // The pixel data of this texture
        glGenerateMipmap(GL_TEXTURE_2D);
        this->path = image_path;
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /* Reload image and rebind this texture */
    void reload(const char* image_path)
    { 
        int width, height;
        unsigned char* image = SOIL_load_image(image_path, &width, &height, 0, SOIL_LOAD_RGB);
        // Regenerate the texture
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height,
                     border, GL_RGB, GL_UNSIGNED_BYTE, image);
        this->path = image_path;
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /* Read the value of texture id */
    GLuint getID()
    { // Return a private member
        return id;
    }

    std::string getType() {
        return type;
    }

    aiString getPath() {
        return path;
    }

    void setID(const GLuint& _id) {
        id = _id;
    }

    void setType(const std::string& _type) {
        type = _type;
    }

    void setPath(const aiString& _path) {
        path = _path;
    }

private:

    /* The ID of this texture 
    ** The most IMPORTANT member! */
    GLuint id;

    /* Specifies the level-of-detail number.
    ** Level 0 is the base image level. 
    ** Level n is the nth mipmap reduction image. 
    ** DEFAULT: 0 */
    GLint level;

    /* The border width of texture
    ** DEFAULT: 0 */
    GLint border;

    /* The type of texture: diffuse, specular, etc.
    ** DEFAULT: diffuse */
    std::string type;

    /* The path of the texture file */
    aiString path;

    /* Specify a pointer to the image data in memory. */
    //SDL_Surface *surface;
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
