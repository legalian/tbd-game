//
//  octree.h
//  Total Control
//
//  Created by Parker Lawrence on 2/29/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__octree__
#define __Total_Control__octree__

#include <stdio.h>

//#include <stdio.h>
//#include "render.h"
//#include <map>
//#include <vector>
//#include <string>
//#include "generators.h"
//#include <sys/stat.h>
#include <map>
#include "constants.h"
#include "materials.h"
#include "qef.h"
#include <fstream>
#include <set>
#include <boost/filesystem.hpp>

struct Octree;
struct OctreeSegment;
struct OctreeBranch;
struct OctreePortionAwareBranch;
class Environment;


#define XYZINDEX [(x&(1<<depth))>>depth][(y&(1<<depth))>>depth][(z&(1<<depth))>>depth]

struct Location {
    int x;
    int y;
    int z;
    Location(int,int,int);
    Location();
    std::string tostring() const;
    int roughdistanceto(Location&);
    friend bool operator< (const Location&, const Location&);
    friend bool operator== (const Location&, const Location&);
};
struct PathTesterBucket {
    PathTesterBucket* next;
    Location node;
    PathTesterBucket(Location);
    void addthing(Location);
    bool contains(Location);
};
struct PathTesterPool {
    Location destination;
    int recur;
    std::vector<PathTesterBucket*> buckets = std::vector<PathTesterBucket*>(5);
    PathTesterPool(Location,int);
    ~PathTesterPool();
    int hash(Location);
    bool contains(Location,int);
    void add(Location,int);
    int getpromising();
    Location poppromising(int);
};
struct ChunkDistancePool {
    PathTesterBucket** buckets = new PathTesterBucket*[MAX_WORLDFILE_GEOMSAVE+1];
    ChunkDistancePool();
    int hash(Location,Location);
    void add(Location,int);
};
struct BranchRegistry {
    std::vector<std::vector<std::pair<Location,OctreeBranch*>>> registry;
    Environment& capsulepointer;
    BranchRegistry(Environment&);
    void add(Location,OctreeBranch*);
    void prune(OctreeSegment*);
    bool contains(OctreeBranch*);
};
struct __attribute__((packed)) Feature {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    glm::vec3 get(int);
    Feature();
    Feature(uint8_t x,uint8_t y,uint8_t z);
    Feature(const glm::vec3&,int);
    inline float coef(int);
    inline float invcoef(int);
};
struct MatrixCarriage {
//    double bb;
    
    QefPerc Mx;
    QefPerc My;
    QefPerc Mz;
    int Mt;
    QefPerc Abx;
    QefPerc Aby;
    QefPerc Abz;
    QefPerc AAxx;
    QefPerc AAyy;
    QefPerc AAzz;
    QefPerc AAxy;
    QefPerc AAxz;
    QefPerc AAyz;
    friend MatrixCarriage operator+(const MatrixCarriage&, const MatrixCarriage&);
    MatrixCarriage();
    MatrixCarriage(QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,int);
    MatrixCarriage translate(QefPerc,QefPerc,QefPerc);
    Feature evaluate(int);
};
struct __attribute__((packed)) Edgedat {
    uint8_t t;
    
    int8_t x;
    int8_t y;
    int8_t z;
    Edgedat();
    Edgedat(float,float,float,float);
    Edgedat(uint8_t ct,int8_t cx,int8_t cy,int8_t cz);
    
    glm::vec3 getnorm();
    float getoffset();
    MatrixCarriage calcxqef();
    MatrixCarriage calcyqef();
    MatrixCarriage calczqef();
    inline MatrixCarriage calcqef(QefPerc,QefPerc,QefPerc,QefPerc);
};
struct __attribute__((packed)) MaterialPolyCount {
    union {
        MaterialPolyCount* nextpoint;
        int sizes[2];
    } data;
    union {
        int flags;
        BlockId ids[2];
    } descriptor;
    void addoffsets(int,std::map<uint8_t,int>*);
    int add(int,uint8_t,int);
    int get(int,uint8_t);
    int jointo(int,int,MaterialPolyCount*);
    void destroy(int);
    void deletesegment(int,std::map<uint8_t,GeomTerrain>*,std::map<uint8_t,int>*);

};
struct MaterialPolyCountPackage {
    int flagalone=0;
    MaterialPolyCount* data=NULL;
//    inline void add(uint8_t,int);
    MaterialPolyCountPackage();
    MaterialPolyCountPackage(int,MaterialPolyCount*);
};

struct OctreeSegment {
//    virtual std::pair<Location,bool> getneedsaload(int,int,int,int,int,int);
    virtual MatrixCarriage getqef();
    virtual MatrixCarriage vertifyall();
    virtual MatrixCarriage vertifysmall();
    virtual bool isvbaked(BlockLoc,BlockLoc,BlockLoc);
    virtual MaterialPolyCountPackage geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,std::map<uint8_t,int>*,OctreeSegment*,bool);
//    virtual void geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,int);
    virtual void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*);
    virtual OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z);
    virtual BlockId getser(BlockLoc,BlockLoc,BlockLoc);
//    virtual BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc);
    virtual void render(const glm::mat4&);

    virtual void filesave(std::ostream&);
    virtual void worldfilesave(std::ostream&,int);
    virtual Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc);
    virtual glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int);
    virtual glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc);
    virtual void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&);
    
    virtual void determinelod(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,int,bool);
    virtual int getlod(BlockLoc,BlockLoc,BlockLoc);
    
    virtual void testconnected(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*,BranchRegistry*);
    virtual OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&);
    virtual uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int);
};

struct OctreeFeature : OctreeSegment {
    MatrixCarriage qef = MatrixCarriage();
    Feature point;
    BlockId fillvalue;
    uint8_t conflag;
    
    
    OctreeFeature(BlockId,uint8_t);
    MatrixCarriage getqef() override;
    
    glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int) override;
    glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
//    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    MatrixCarriage vertifysmall() override;
    MatrixCarriage vertifyall() override;
    void filesave(std::ostream&) override;
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
    
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId,uint8_t);
    OctreeLeaf(BlockId,uint8_t,int8_t[12]);
    
    MaterialPolyCountPackage geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,std::map<uint8_t,int>*,OctreeSegment*,bool) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeBud : OctreeSegment {
    BlockId fillvalue;
    
    OctreeBud(BlockId);
    
    
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
//    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void filesave(std::ostream&) override;
    void worldfilesave(std::ostream&,int) override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
    
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
};
struct OctreeBranch : OctreeSegment {
    int depth;
    uint8_t connections = 0;
    int lodflags1 = 0;
    int lodflags2 = 0;
    MaterialPolyCount materialpolys;
    
    Feature point;
    OctreeSegment* subdivisions[2][2][2];
    
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int);
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int,uint8_t,uint8_t,uint8_t);
    
//    std::pair<Location,bool> getneedsaload(int,int,int,int,int,int) override;
    void determinelod(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,int,bool) override;
    int getlod(BlockLoc,BlockLoc,BlockLoc) override;
    OctreeBranch(OctreeSegment*,int,uint8_t,uint8_t,uint8_t,uint8_t);
    MatrixCarriage getqef() override;
    MatrixCarriage vertifyall() override;
    void worldfilesave(std::ostream&,int) override;
    MatrixCarriage vertifysmall() override;
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
    MaterialPolyCountPackage geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,std::map<uint8_t,int>*,OctreeSegment*,bool) override;

    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
//    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
    
    void render(const glm::mat4&) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
    glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int) override;
    glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc) override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
    void testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,BranchRegistry*) override;
    bool phase2check(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,Environment*);
    
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
    
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
};
struct OctreePortionAwareBranch : OctreeBranch {
    bool changed = false;
    bool hardLoaded = true;
    std::map<uint8_t,GeomTerrain>* geometry = NULL;
    std::map<uint8_t,GeomTerrain>* nextpasscleanup = NULL;
    OctreePortionAwareBranch(OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,int,bool);
    OctreePortionAwareBranch(OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,int,uint8_t,uint8_t,uint8_t);
    OctreePortionAwareBranch(OctreeSegment*,int,uint8_t,uint8_t,uint8_t,uint8_t);
    
    void render(const glm::mat4&) override;
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
    MaterialPolyCountPackage geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,std::map<uint8_t,int>*,OctreeSegment*,bool) override;
    uint8_t giveconflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
    
    void matrixmap(glm::mat4*);
};
class Octree {
public:
    void expand(BlockLoc,BlockLoc,BlockLoc);
    void expandchunk(BlockLoc,BlockLoc,BlockLoc);
    void expandarbit(BlockLoc,BlockLoc,BlockLoc,int);
    void expand(int);
    int underpressure(BlockLoc,BlockLoc,BlockLoc);
    int depth = CHPOWER;
    int popular;
    glm::mat4& matrixRef;
    
    BranchRegistry currenttests;
    Octree(std::string,glm::mat4&,Environment&,int);
    OctreeSegment* data;// = new OctreeBud(0);
    void loadportion(BlockLoc,BlockLoc,BlockLoc,BlockId (*)[CHSIZE+1][CHSIZE+1]);
    void render();
//    void h_manifest(BlockLoc,BlockLoc,BlockLoc);
//    void v_manifest(BlockLoc,BlockLoc,BlockLoc);
//    void g_manifest(BlockLoc,BlockLoc,BlockLoc);
    bool existsat(BlockLoc,BlockLoc,BlockLoc);
    void filepullportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    void filepushportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    bool dataexists(std::string,BlockLoc,BlockLoc,BlockLoc);
};

inline void Xstitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId,MaterialPolyCount*,int&);
inline void Ystitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId,MaterialPolyCount*,int&);
inline void Zstitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId,MaterialPolyCount*,int&);

OctreeSegment* loadWorldFile(std::ifstream& file,int recur);
void tearaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
bool testConnection(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
OctreeSegment* makeOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);

OctreeSegment* makeOctree(std::ifstream&,int);

#endif /* defined(__Total_Control__octree__) */
