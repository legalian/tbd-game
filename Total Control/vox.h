//
//  vox.h
//  Total Control
//
//  Created by Parker Lawrence on 12/21/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__vox__
#define __Total_Control__vox__

#include <stdio.h>
#include "render.h"
#include <map>
#include <vector>
#include <string>
#include "generators.h"
#include <sys/stat.h>
#include <fstream>

//#include "boost/interprocess/sync/interprocess_mutex.hpp"

#define CHSIZE 128
#define CHPOWER  7

inline int MOD(int,int);
inline int TRUNC_DIV(int,int);

class Generator;

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

struct OctreeSegment {
    OctreeSegment(int (*)[CHSIZE][CHSIZE],int,int,int,int);
    OctreeSegment();
    unsigned int fillvalue;
    bool isfilled;
    OctreeSegment* subdivisions[2][2][2];
    unsigned int getser(const int&,const int&,int const&);
    bool uniqueat(const int&,const int&,int const&,int const&);
    void savetofile(std::ostream&);
    OctreeSegment(std::istream&);
};
class OctreePortion {
public:
    int lod = 0;
    OctreeSegment data;
    OctreePortion(int (*)[CHSIZE][CHSIZE]);
    OctreePortion(std::string);
    unsigned int getAt(int,int,int);
    std::pair<Location,Location> voxbounds();
    bool tryvox();
    bool uniqueat(int,int,int);
    int flipbits(int);
    void save(std::string);
};

class Structure {
public:
    std::string structureid = "";
    int loadstage = 0;
    Location epicenter;
    Location lowestbounds;
    Location upperbounds;
    bool bounded;
    glm::mat4 transform;
    std::vector<Location> queue;
    std::map<Location,OctreePortion*> portions;
    std::map<Location,GeomTerrain> geoms;
    GeomTerrain* createOrGet(Location);
    unsigned int getAt(int,int,int);
    unsigned int getLodAt(int,int,int);
    bool existsAt(int,int,int);
    void refreshqueue();
//    void sortqueue(glm::vec4 position);
    void load(Location,Generator*);
    void settransform(glm::mat4);
    void voxPortion(Location);
    void voxSnippets(Location);
    void voxXskirt(int,int,int);
    void voxYskirt(int,int,int);
    void voxZskirt(int,int,int);
    void voxXrow(int,int,int);
    void voxYrow(int,int,int);
    void voxZrow(int,int,int);
    void voxXYZcorner(int,int,int);
    void addmarch(int,int,int,GeomTerrain*);
};

void MarchCube(int,int,int,GLfloat*,std::vector<glm::vec3>&);
void SolidCube(int,int,int,GLfloat*,std::vector<glm::vec3>&);

#endif /* defined(__Total_Control__vox__) */
