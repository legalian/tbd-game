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
//#include "render.h"
#include "glm/gtx/string_cast.hpp"
#include <boost/filesystem.hpp>

class Environment {
private:
//    std::vector<std::pair<Location*,Structure*>> loadqueue;
//    std::vector<GeomTerrain*> bakequeue;
    std::vector<Structure*> structures;
//    boost::filesystem::path savedir = boost::filesystem::path("testworld");
//    glm::vec4 view = glm::vec4(0,0,0,1);
    
    pthread_t loadingthread;
//    Generator gen;
    double epix = 0;
    double epiy = 0;
    double epiz = 0;
    int shards = 0;
    
    
public:
    bool keepexecution = true;
//    Environment();
    void opensavedirectory();
    void loadnextchunk();
    void beginthread();
    void addstructure(Structure* newst);
    void draw();
    void checkup(double x,double y,double z);
    Structure* getstructure(std::string targetid);
    void cleanup();
    std::string nextshardname();
};
//
void* loaderthread(void*);

#endif /* defined(__Total_Control__environment__) */
