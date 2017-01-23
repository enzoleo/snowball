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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assimp/types.h>

/* The texture type
** Determined by image extension name
** Attention: The enum options after TEXTURE_JPG
**     all support transparency */
enum TextureImgType
{
    TEXTURE_BMP,
    TEXTURE_JPG,
    TEXTURE_PNG,
    TEXTURE_DDS,
    TEXTURE_TGA
};

/* CLASS: Texture */
class Texture
{
public:
    /* Default constructor */
    Texture(const std::string& _type = "diffuse",
            const std::string& _path = "./textures",
            TextureImgType _imgType = TEXTURE_BMP)
        : level(0),
          border(0),
          type(_type),
          unit(0),
          imgType(_imgType)
    { // Do nothing here
        path = aiString(_path);
    }

    /* Load image and bind texture */
    Texture(const char* image_path)
    { // Constructor
        reload(image_path);
    }

    /* Reload image and rebind this texture */
    void reload(const char* image_path,
                TextureImgType _imgType = TEXTURE_BMP)
    {
        // Load SDL surface
        path = image_path;
        SDL_Surface *surface = IMG_Load(image_path);

        // The width and height of the texture
        width = surface->w;
        height = surface->h;
        imgType = _imgType;

        // If the texture supports transparency
        if (imgType > TEXTURE_JPG)
        { // Convert surface format to enable transparency loading
            surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
        }
        // Make room for our texture
        glGenTextures(1, &id);
        // Tell OpenGL which texture to edit and map the image to the texture
        glBindTexture(GL_TEXTURE_2D, id);

        // ----------------------------------------------------------------------------------------
        // Specify a two-dimensional texture image
        // Learn more about this function:
        // https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
        // ----------------------------------------------------------------------------------------
        if (imgType > TEXTURE_JPG)
        { // If the texture supports transparency
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA,
                         width, height, border,
                         GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        }
        else
        { // Set RGB mode if transparency is not enabled
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB,
                         width, height, border,
                         GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        }

        // Generate texture mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        SDL_FreeSurface(surface);
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

    /* PRIVATE MEMBER
    ** The image type of texture */
    TextureImgType imgType;

};

GLuint TextureFromFileDir(const char* path, std::string directory)
{
    //Generate texture ID and load texture data 
    std::string filename(path);
    filename = directory + '/' + filename;
    int width, height;

    SDL_Surface* surface = IMG_Load(filename.c_str());
    width = surface->w;
    height = surface->h;
        
    // Assign texture to ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         width, height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
        
    SDL_FreeSurface(surface);
    return textureID;
}

//filename contains its directory
GLuint TextureFromFile(const char* filename)
{
    int width, height;
    SDL_Surface* surface = IMG_Load(filename);
    width = surface->w;
    height = surface->h;

    // Assign texture to ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                         width, height, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_FreeSurface(surface);
    return textureID;
}

#endif