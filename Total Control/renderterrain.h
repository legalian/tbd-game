//
//  Organizational.h
//  Total Control
//
//  Created by Parker Lawrence on 12/13/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__Organizational__
#define __Total_Control__Organizational__

#include <stdio.h>


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>


#include </usr/include/GL/glew.h>

#include <glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "texture.h"


class GeomTerrain {
public:
    int size = 0;
    bool baked = false;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_normals;
    GLuint vertexbuffer;
    GLuint normalbuffer;
    
    void addVert(double,double,double);
    void addNormal(double,double,double);
    void addVert(glm::vec3);
    void addNormal(glm::vec3);
    
    void bake();
//    void unbake();
//    void rebake();
    
//    uint8_t material;
    glm::mat4* matrix;
};

class ShaderTerrain {
private:
    GLuint programID;
    GLuint matrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexNormal_modelspaceID;
    
public:
    void mountshaders();
    
    void cleanup();
    void draw(GeomTerrain*);
    void open();
    void close();
};




#endif /* defined(__Total_Control__Organizational__) */
