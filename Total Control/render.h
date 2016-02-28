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

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "texture.h"

class Geom {
protected:
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_normals;
public:
    GLuint vertexbuffer;
    GLuint normalbuffer;
    glm::mat4* matrix;// = glm::mat4(1.0f);
    unsigned long size;
    bool baked = false;
    bool changed = false;
    
    void addVert(double,double,double);
    void addNormal(double,double,double);
    void addVert(glm::vec3);
    void addNormal(glm::vec3);
    void dumpverts();
    void assumeNormals();
    void bake();
    void unbake();
    void rebake();
};
class GeomVNC : public Geom {
private:
    std::vector<glm::vec3> indexed_colors;
public:
    GLuint colorbuffer;
    void addColor(double,double,double);
    void addColor(glm::vec3);
    void bake();
    void unbake();
};
class GeomTerrain : public Geom {
private:
    std::vector<uint8_t> indexed_reference;
public:
    GLuint referencebuffer;
    void addReference(uint8_t);
    void bake();
    void unbake();
    
};
class ShaderObject {
protected:
    GLuint programID;
    GLuint matrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexNormal_modelspaceID;
public:
    glm::mat4 cam;
    void mountshaders();
    void setcam(glm::mat4);
    void open();
    void close();
    void cleanup();
};
class ShaderVNC : public ShaderObject {
private:
    GLuint vertexcolorID;
public:
    void mountshaders();
    void draw(GeomVNC*);
    void draw(Geom*);
    void open();
    void close();
};
class ShaderTerrain : public ShaderObject {
private:
    GLuint textureID;
    GLuint referenceID;
    
    GLuint texture;
public:
    void mountshaders();
    void draw(GeomTerrain*);
    void open();
    void close();
    
    
};




#endif /* defined(__Total_Control__Organizational__) */
