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
#include "geometryoctree.h"
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
typedef struct Location {
    int x;
    int y;
    int z;
    Location(int,int,int);
    Location();
    std::string tostring() const;
    friend bool operator< (const Location& l, const Location& r);
    friend bool operator== (const Location& l, const Location& r);
} Location;
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
    static EmptySampler emptysampler;
    Sampler* source = &emptysampler;
    std::string structureid;
    glm::mat4 transform;
    Octree world = Octree(transform);
//    GeometryOctree visibleworld = GeometryOctree(transform);
    std::vector<Location> queue;
    Bounds bounds;
    Location cameraloc;
    int loadstage = 0;
    
    Structure(std::string);
    void updatequeue(double,double,double);
    void attain(std::string,Location);
    void render();
    
//    int loadstage = 0;
//    Location epicenter;
//    Location lowestbounds;
//    Location upperbounds;
//    bool bounded;q
//    std::vector<Location> queue;
    
    
    
//    GeomTerrain* createOrGet(Location);
//    uint8_t getAt(int,int,int);
//    int getLodAt(int,int,int);
//    bool existsAt(int,int,int);
//    void refreshqueue();
//    void sortqueue(glm::vec4 position);
//    void load(Location,Generator*);
//    void settransform(glm::mat4);
//    void voxPortion(Location);
//    void voxSnippets(Location);
//    void voxXskirt(int,int,int);
//    void voxYskirt(int,int,int);
//    void voxZskirt(int,int,int);
//    void voxXrow(int,int,int);
//    void voxYrow(int,int,int);
//    void voxZrow(int,int,int);
//    void voxXYZcorner(int,int,int);
//    void addmarch(int,int,int,GeomTerrain*);
};
//
//void MarchCube(int,int,int,GLfloat*,std::vector<glm::vec3>&);
//void SolidCube(int,int,int,bool*,std::vector<glm::vec3>&);
//
#endif /* defined(__Total_Control__vox__) */

