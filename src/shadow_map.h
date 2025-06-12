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
class ShadowMap {
 public:
  /* Default constructor & Constructor */
  ShadowMap(GLuint _width, GLuint _height)
      : width(_width),
        height(_height) {  // Do initialization
    init();
  }

  /* Bind the buffer (shadow map) */
  void bind() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  /* Unbind the buffer (shadow map) */
  void unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  /* Returns the private members
   ** We set the shadow map settings private, because they are not supposed to be
   ** editted easily. If they need to be editted, call the `set*` functions, which makes
   ** sure you edit them on purpose, instead of unconsciously. */
  const GLuint getDepthMap() { return depthMap; }
  const GLuint getWidth() { return width; }
  const GLuint getHeight() { return height; }

 private:
  /* PRIVATE MEMBER:
  ** Do Initialization */
  void init() {
    // Generate frame buffers
    glGenFramebuffers(1, &depthMapFBO);

    // Generate and bind textures
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, 0);

    // Texture parameters settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, white);

    // Bind and draw frame buffer (then unbind it)
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  /* PRIVATE MEMBERS:
  ** The width and height of the shadow map */
  GLuint width, height;

  /* PRIVATE MEMBERS
  ** The depthMap and its FBO */
  GLuint depthMapFBO, depthMap;
};

#endif
