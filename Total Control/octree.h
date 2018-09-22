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

#include <map>
#include "constants.h"
//#include "materials.h"
#include "renderterrain.h"
#include <fstream>
#include <set>

struct Octree;
struct OctreeSegment;
struct OctreeBranch;
struct OctreePortionAwareBranch;
class Environment;


#define XYZINDEX [(x&(1<<depth))>>depth][(y&(1<<depth))>>depth][(z&(1<<depth))>>depth]

extern thread_local int g_lod;
extern thread_local int* g_olods;
extern thread_local OctreeSegment* g_world;
extern thread_local std::map<uint8_t,GeomTerrain>* g_geometry;
extern thread_local std::vector<glm::vec3>* g_vertecies;
extern thread_local std::vector<glm::vec3>* g_normals;
extern thread_local std::ostream* s_file;

glm::vec3 readvoxvert(int,int,int,uint8_t,uint8_t,uint8_t,int);
uint8_t savevoxvert(int,float,int);
glm::vec3 readvoxnorm(int8_t,int8_t,int8_t);
int8_t savevoxnorm(float);

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
struct MatrixCarriage {
    QefPerc Vx;
    QefPerc Vy;
    QefPerc Vz;
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
    MatrixCarriage(QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,QefPerc,int);
    MatrixCarriage translate(QefPerc,QefPerc,QefPerc);
    glm::vec3 evaluate(int);
    glm::vec3 evaluatenormal();
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

struct OctreeSegment {
    bool dataloadedat(BlockLoc,BlockLoc,BlockLoc);
    bool dataexistsat(BlockLoc,BlockLoc,BlockLoc);
    bool datapsuedoat(BlockLoc,BlockLoc,BlockLoc);

    virtual void testcoherence();
//    virtual MatrixCarriage getqef();
    virtual MatrixCarriage vertify(BlockLoc,BlockLoc,BlockLoc);
    virtual void geomify(BlockLoc,BlockLoc,BlockLoc);
//    virtual void hermitify(BlockLoc,BlockLoc,BlockLoc);
    virtual OctreePortionAwareBranch* getvoxunit(BlockLoc,BlockLoc,BlockLoc);
    virtual OctreeSegment* indivref(BlockLoc,BlockLoc,BlockLoc);
    virtual BlockId getser(BlockLoc,BlockLoc,BlockLoc);
    virtual void manifestgeom(BlockLoc,BlockLoc,BlockLoc);
    virtual void manifestgeomsnippets(BlockLoc,BlockLoc,BlockLoc,int);
    
    virtual void prepare(BlockLoc,BlockLoc,BlockLoc);
    
    virtual void filesave();
    virtual void worldfilesave(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc);
    
    virtual int feat(BlockLoc,BlockLoc,BlockLoc);
    virtual glm::vec3& getfeature(BlockLoc,BlockLoc,BlockLoc);
    virtual glm::vec3& getnormal(BlockLoc,BlockLoc,BlockLoc);
    
    
    virtual void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&);
    
    virtual void determinelod(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,bool,OctreeSegment*);
    
    virtual int getlod(BlockLoc,BlockLoc,BlockLoc);
    
    virtual void testconnected(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*,BranchRegistry*);
    virtual OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&);
    virtual uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc);
    virtual uint8_t giveimconflag();
};

struct OctreeFeature : OctreeSegment {
//    MatrixCarriage qef;
    int point;
    BlockId fillvalue;
    uint8_t conflag;
    OctreeFeature(BlockId,uint8_t);
//    MatrixCarriage getqef() override;
    virtual int feat(BlockLoc,BlockLoc,BlockLoc) override;
    OctreeSegment* indivref(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    MatrixCarriage vertify(BlockLoc,BlockLoc,BlockLoc) override;
    void filesave() override;
    uint8_t giveimconflag() override;
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId,uint8_t);
    OctreeLeaf(BlockId,uint8_t,int8_t[12]);
    void manifestgeom(BlockLoc,BlockLoc,BlockLoc) override;
    void manifestgeomsnippets(BlockLoc,BlockLoc,BlockLoc,int) override;
    void filesave() override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeBud : OctreeSegment {
    BlockId fillvalue;
    OctreeBud(BlockId);
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void filesave() override;
    void worldfilesave(BlockLoc,BlockLoc,BlockLoc) override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
    OctreeSegment* indivref(BlockLoc,BlockLoc,BlockLoc) override;
    uint8_t giveimconflag() override;
};
struct OctreeBranch : OctreeSegment {
    int depth;
    uint8_t connections = 0;
    
    int point=-1;//TODO
    OctreeSegment* subdivisions[2][2][2];
    
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int);
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int,glm::vec3,glm::vec3);
    OctreeBranch(BlockId,int,glm::vec3,glm::vec3,uint8_t);
    
    void determinelod(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,bool,OctreeSegment*) override;

    int getlod(BlockLoc,BlockLoc,BlockLoc) override;
//    MatrixCarriage getqef() override;
    MatrixCarriage vertify(BlockLoc,BlockLoc,BlockLoc) override;
    void worldfilesave(BlockLoc,BlockLoc,BlockLoc) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc) override;
    void manifestgeom(BlockLoc,BlockLoc,BlockLoc) override;
    void manifestgeomsnippets(BlockLoc,BlockLoc,BlockLoc,int) override;
    
    void testcoherence() override;

    void prepare(BlockLoc,BlockLoc,BlockLoc) override;

//    void hermitify(BlockLoc,BlockLoc,BlockLoc) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    OctreeSegment* indivref(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;

    void filesave() override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;

    virtual int feat(BlockLoc,BlockLoc,BlockLoc) override;
    virtual glm::vec3& getfeature(BlockLoc,BlockLoc,BlockLoc) override;
    virtual glm::vec3& getnormal(BlockLoc,BlockLoc,BlockLoc) override;

    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
    void testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,BranchRegistry*) override;
    bool phase2check(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,Environment*);
    
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
    
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc) override;
    uint8_t giveimconflag() override;
};
struct OctreePortionAwareBranch : OctreeBranch {
//    bool changed    = false;


    int otherlods[7] = {-1,-1,-1,-1,-1,-1,-1};
    int lod = -1;
    bool hardload = false;
    bool prepared = false;
    bool voxed = false;
    bool psuedovoxed = false;
    std::vector<glm::vec3>* vertecies=0;
    std::vector<glm::vec3>* normals=0;
    
    GLuint* vertbuffer = new GLuint[MAX_WORLDFILE_GEOMSAVE+1];
    GLuint* normbuffer = new GLuint[MAX_WORLDFILE_GEOMSAVE+1];
    bool baked = false;
    bool mthreadinitialized = false;
    
    std::map<uint8_t,GeomTerrain> geometry;
    
    OctreePortionAwareBranch(OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,bool);
    OctreePortionAwareBranch(OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,bool,glm::vec3,glm::vec3);
//    OctreePortionAwareBranch(OctreeSegment*,int,uint8_t,uint8_t,uint8_t,uint8_t);
    OctreePortionAwareBranch(BlockId,glm::vec3,glm::vec3,uint8_t);
    
    void worldfilesave(BlockLoc,BlockLoc,BlockLoc) override;
    
    void prepare(BlockLoc,BlockLoc,BlockLoc) override;
    int getlod(BlockLoc,BlockLoc,BlockLoc) override;
    void determinelod(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,bool,OctreeSegment*) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc) override;
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc,BlockLoc,BlockLoc) override;
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
    
    glm::vec3& getfeature(BlockLoc,BlockLoc,BlockLoc) override;
    glm::vec3& getnormal(BlockLoc,BlockLoc,BlockLoc) override;
    
//    void updatevisual(BlockLoc x,BlockLoc y,BlockLoc z);
    bool surroundinglod(BlockLoc x,BlockLoc y,BlockLoc z);
    
    void bake();
};


OctreeSegment* loadWorldFile(std::ifstream& file,BlockLoc x,BlockLoc y,BlockLoc z,int recur);
void tearaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
bool testConnection(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
OctreeSegment* makeOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);

OctreeSegment* makeOctree(std::ifstream&,int);

#endif /* defined(__Total_Control__octree__) */
