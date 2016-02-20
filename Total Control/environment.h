//
//  environment.h
//  Total Control
//
//  Created by Parker Lawrence on 1/9/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__environment__
#define __Total_Control__environment__

#include <stdio.h>
#include "vox.h"
#include "render.h"
#include "generators.h"
#include "glm/gtx/string_cast.hpp"

class Environment {
private:
    std::vector<std::pair<Location*,Structure*>> loadqueue;
    std::vector<GeomTerrain*> bakequeue;
    std::vector<Structure> structures;
    glm::vec4 view = glm::vec4(0,0,0,1);
    
    pthread_t loadingthread;
    Generator gen;
    
    
    
    bool testerbool = true;
public:
    bool keepexecution = true;
    
    Environment();
    void loadnextchunk();
    void draw(ShaderTerrain*);
    void cluein(double,double,double);
    Structure* getStruct(std::string targetid);
    void checkup();
    void cleanup();
};

void* loaderthread(void*);

#endif /* defined(__Total_Control__environment__) */
