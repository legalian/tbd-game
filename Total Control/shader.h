//
//  shader.h
//  Total Control
//
//  Created by Parker Lawrence on 8/7/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__shader__
#define __Total_Control__shader__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include </usr/include/GL/glew.h>

#include <glfw3.h>

//GLint Result = GL_FALSE;
//int InfoLogLength;


void LoadSingleShader(GLint,const char *,GLint &,int &);

GLuint LoadShaders(const char *,const char *);
GLuint LoadShaders(const char *,const char *,const char *);


#endif /* defined(__Total_Control__shader__) */
