////
////  vox.h
////  Total Control
////
////  Created by Parker Lawrence on 12/21/15.
////  Copyright (c) 2015 Parker Lawrence. All rights reserved.
////
//
#ifndef __Total_Control__vox__
#define __Total_Control__vox__
//
#include <stdio.h>
#include "octree.h"
//#include <map>
//#include <vector>
#include <string>
#include "samplers.h"
//#include "generators.h"
//#include <sys/stat.h>
//#include <fstream>
//
////#include "boost/interprocess/sync/interprocess_mutex.hpp"
//#include "constants.h"
//
//inline int MOD(int,int);
//inline int TRUNC_DIV(int,int);
//
//class Generator;
//
class Sampler;
//
typedef struct Bounds {
    int lx;
    int ux;
    int ly;
    int uy;
    int lz;
    int uz;
    bool bounded;
    Bounds(int,int,int,int,int,int);
    Bounds();
} Bounds;
//
class Structure {
public:
    Sampler* source = NULL;
    std::string structureid;
    glm::mat4 transform = glm::mat4(1.0f);
    ChunkDistancePool geomloaded = ChunkDistancePool();
    Location cameraloc;
    int loadstage = 0;
    bool priorityFlip=false;
    bool loadin = false;
    
    Structure(std::string,Environment&,bool);
    void update();
    void updatelod(double,double,double);
    void updatequeue(double,double,double);
    bool attain(Location,Location);
    void render();
    
    void expand(BlockLoc,BlockLoc,BlockLoc);
    void expandchunk(BlockLoc,BlockLoc,BlockLoc);
    void expandarbit(BlockLoc,BlockLoc,BlockLoc,int);
    void expand(int);
    int underpressure(BlockLoc,BlockLoc,BlockLoc);
    int depth = CHPOWER;
    int popular;
    
    BranchRegistry currenttests;
    OctreeSegment* data=NULL;// = new OctreeBud(0);
//    std::map<uint8_t,GeomTerrain> hardloadgroups[MAX_WORLDFILE_GEOMSAVE+1-MIN_WORLDFILE_GEOMSAVE];
    
    void loadportion(BlockLoc,BlockLoc,BlockLoc,BlockId (*)[CHSIZE+1][CHSIZE+1]);

    void filepullportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    void filepushportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    
};
//
//void MarchCube(int,int,int,GLfloat*,std::vector<glm::vec3>&);
//void SolidCube(int,int,int,bool*,std::vector<glm::vec3>&);
//
#endif /* defined(__Total_Control__vox__) */

