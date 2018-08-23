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
#include <map>


#include </usr/local/include/GL/glew.h>

#include <glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "texture.h"
#include "constants.h"

struct OctreePortionAwareBranch;

class GeomTerrain {
public:
    std::vector<int> indexed;
    std::vector<int> extreme;
    int** lodlayers;
    GLuint* primbuffer;
    int* coresize;
    int* totalsize;
    std::vector<glm::vec3> exlay;
    std::vector<glm::vec3> exlaynormal;
    GLuint exlaybuffer;
    GLuint exlaynormalbuffer;
    
    void addVert(int);
    void addVertExt(int);
    void addVertExlay(glm::vec3);
    void addVertExlayNormal(glm::vec3);
    
    void factor(int);
    void emptyExlay();
    
    GeomTerrain();
    
    void bake();
    glm::mat4* matrix=NULL;
};
struct GeomLense {
    OctreePortionAwareBranch* reference;
    GeomTerrain* target;
    int lod;
    GeomLense(OctreePortionAwareBranch*,GeomTerrain*,int);
};
class ShaderTerrain {
protected:
//    std::map<GeomTerrain*,std::pair<int,int>> endpoints;
    GLuint programID;
    GLuint matrixID;
    GLuint vertexPosition_modelspaceID;
    GLuint vertexNormal_modelspaceID;
    
public:
    virtual void mountshaders();
    
    void cleanup();
    virtual void draw(GeomLense);
    void open();
    void close();
};
#ifdef WIREFRAMEDEBUG
class DebugShader : public ShaderTerrain {
public:
    void mountshaders() override;
    void draw(GeomLense) override;
};
#endif



#endif /* defined(__Total_Control__Organizational__) */
