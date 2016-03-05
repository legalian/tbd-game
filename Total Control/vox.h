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
#define SCALE 1

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

struct edgedat {
    uint8_t t;
    
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

struct OctreeSegment {
    static OctreeSegment* makeOctree(uint8_t (*)[CHSIZE][CHSIZE],int,int,int,int);
    static OctreeSegment* makeOctree(std::istream&,int);
//    bool isfilled;
    virtual uint8_t getser(const int&,const int&,int const&);
    virtual bool uniqueat(const int&,const int&,int const&,int const&);
    virtual void savetofile(std::ostream&);
    virtual void setxcon(int,int,int,const edgedat&);
    virtual void setycon(int,int,int,const edgedat&);
    virtual void setzcon(int,int,int,const edgedat&);
    virtual edgedat getxcon(int,int,int);
    virtual edgedat getycon(int,int,int);
    virtual edgedat getzcon(int,int,int);
    
};
struct OctreeLeaf : OctreeSegment {
    uint8_t fillvalue;
    edgedat** xplane;
    edgedat** yplane;
    edgedat** zplane;
    
    OctreeLeaf(int);
    
    uint8_t getser(const int&,const int&,int const&) override;
    bool uniqueat(const int&,const int&,int const&,int const&) override;
    void savetofile(std::ostream&) override;
    
    void setxcon(int,int,int,const edgedat&) override;
    void setycon(int,int,int,const edgedat&) override;
    void setzcon(int,int,int,const edgedat&) override;
    edgedat getxcon(int,int,int) override;
    edgedat getycon(int,int,int) override;
    edgedat getzcon(int,int,int) override;
};
struct OctreeBranch : OctreeSegment {
    OctreeSegment* subdivisions[2][2][2];
    
    uint8_t getser(const int&,const int&,int const&) override;
    bool uniqueat(const int&,const int&,int const&,int const&) override;
    void savetofile(std::ostream&) override;
    
    void setxcon(int,int,int,const edgedat&) override;
    void setycon(int,int,int,const edgedat&) override;
    void setzcon(int,int,int,const edgedat&) override;
    edgedat getxcon(int,int,int) override;
    edgedat getycon(int,int,int) override;
    edgedat getzcon(int,int,int) override;
};
class OctreePortion {
public:
    int lod = 0;
    OctreeSegment* data;
    OctreePortion(uint8_t (*)[CHSIZE][CHSIZE]);
    OctreePortion(std::string);
    uint8_t getAt(int,int,int);
//    std::pair<Location,Location> voxbounds();
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
    uint8_t getAt(int,int,int);
    int getLodAt(int,int,int);
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
void SolidCube(int,int,int,bool*,std::vector<glm::vec3>&);

#endif /* defined(__Total_Control__vox__) */
