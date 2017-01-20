/*******************************************************************************
** Software License Agreement (GNU GENERAL PUBLIC LICENSE)
**
** Copyright 2016-2017  Peiyu Liao (enzoliao95@gmail.com). All rights reserved.
** Copyright 2016-2017  Yaohong Wu (wuyaohongdio@gmail.com). All rights reserved.
**
** LICENSE INFORMATION (GPL)
** SEE `LICENSE` FILE.
*******************************************************************************/

#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include <GL/glew.h>

/* CLASS: Showmap */
class ShadowMap
{
public:
    ShadowMap(GLuint w, GLuint h)
    {
        width = w;
        height = h;

        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

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
    ~ShadowMap() {}

    void Bind()
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint getDepthMap() {
        return depthMap;
    }

private:

    GLuint width;
    GLuint height;

    GLuint depthMapFBO;
    GLuint depthMap;
};

#endif
