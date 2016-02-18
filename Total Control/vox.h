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
class Portion {
public:
    unsigned int lod = 1;
    virtual unsigned int getAt(int,int,int);
};
class SolidPortion : public Portion {
public:
    unsigned int fillId;
    unsigned int getAt(int,int,int) override;
    SolidPortion(unsigned int);
};
class DetailedPortion : public Portion {
public:
    unsigned int data[CHSIZE][CHSIZE][CHSIZE];
    unsigned int getAt(int,int,int) override;
};
class PortionPointer {
private:
    Portion* pointer;
    int* reference;
public:
    PortionPointer(Portion*);
    PortionPointer(const PortionPointer& sp);
    PortionPointer();
    ~PortionPointer();
    Portion& operator* ();
    Portion* operator-> ();
    PortionPointer& operator= (const PortionPointer& sp);
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
    std::map<Location,PortionPointer> portions;
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
