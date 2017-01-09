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
#include "constants.h"

class GeomTerrain {
public:
    int size = 0;
    bool baked = false;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_normals;
    GLuint vertexbuffer;
    GLuint normalbuffer;
#ifdef WIREFRAMEDEBUG
    int wiresize = 0;
    std::vector<glm::vec3> wire_debug_vertices;
    std::vector<glm::vec3> wire_debug_colors;
    GLuint wire_vertexbuffer;
    GLuint wire_colorbuffer;
    void addWireVert(double,double,double);
    void addWireColor(double,double,double);
    void addWireVert(glm::vec3);
    void addWireColor(glm::vec3);
#endif
    void addVert(double,double,double);
    void addNormal(double,double,double);
    void addVert(glm::vec3);
    void addNormal(glm::vec3);
    
    void erase(int,int);
    
    void bake();
    glm::mat4* matrix;
};

class ShaderTerrain {
protected:
    GLuint programID;
    GLuint matrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexNormal_modelspaceID;
    
public:
    virtual void mountshaders();
    
    void cleanup();
    virtual void draw(GeomTerrain*);
    void open();
    void close();
};
#ifdef WIREFRAMEDEBUG
class DebugShader : public ShaderTerrain {
public:
    void mountshaders() override;
    void draw(GeomTerrain*) override;
};
#endif



#endif /* defined(__Total_Control__Organizational__) */
