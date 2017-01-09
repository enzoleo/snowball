#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include <GL/glew.h>

/* CLASS: Shadow map */
class ShadowMap
{
public:

    /* Default constructor & Constructor */
    ShadowMap(GLuint _width,
              GLuint _height)
        : width(_width),
          height(_height)
    { // Do initialization
        init();
    }

    /* The function to bind the shadow map */
    void bind()
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    /* The function to unbind the shadow map */
    void unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /* Returns the private members
    ** We set the shadow map settings private, because they are not supposed to be
    ** editted easily. If they need to be editted, call the `set*` functions, which makes
    ** sure you edit them on purpose, instead of unconsciously. */
    const GLuint getWidth() { return width; }
    const GLuint getHeight() { return height; }
    const GLuint getDepthMap() { return depthMap; }

private:

    /* PRIVATE MEMBER
    ** The function to initiate parameters */
    void init()
    {
        // Generate frame buffers
        glGenFramebuffers(1, &depthMapFBO);

        // Generate billboard texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     width, height, 0, GL_DEPTH_COMPONENT,
                     GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /* PRIVATE MEMBERS
    ** The width and height of the shadow map */
    GLuint width, height;

    /* PRIVATE MEMBERS
    ** Members related to depth map */
    GLuint depthMap, depthMapFBO;
};

#endif
