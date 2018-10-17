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

#include <OpenCL/opencl.h>


struct Octree;
struct OctreeSegment;
struct OctreeBranch;
struct OctreeLeaf;
struct OctreePortionAwareBranch;
class Environment;
class Sampler;

#define XYZINDEX [(x&(1<<depth))>>depth][(y&(1<<depth))>>depth][(z&(1<<depth))>>depth]

extern thread_local int g_lod;
extern thread_local int* g_olods;
extern thread_local OctreeSegment* g_world;
extern thread_local std::map<uint8_t,GeomTerrain>* g_geometry;
extern thread_local std::vector<glm::vec3>* g_vertecies;
extern thread_local std::vector<glm::vec3>* g_normals;
extern cl_context gcl_context;
extern cl_command_queue gcl_commands;
extern cl_device_id gcl_device_id;

//extern thread_local Sampler* g_sampler;
//extern thread_local float* g_samplefield;
//extern thread_local BlockId* g_idfield;

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
    MatrixCarriage& operator+=(const MatrixCarriage& o);
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
    Edgedat(glm::vec3,float);
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
    bool vertsloadedat(int,int,int);
    bool serloadedat(int,int,int);
//    bool datapsuedoat(BlockLoc,BlockLoc,BlockLoc);

    virtual void testcoherence();
//    virtual MatrixCarriage getqef();
    virtual MatrixCarriage vertify(int,int,int);
    virtual void geomify(int,int,int);
//    virtual void hermitify(BlockLoc,BlockLoc,BlockLoc);
    virtual OctreePortionAwareBranch* getvoxunit(int,int,int);
    virtual std::pair<BlockId,OctreeLeaf*> indivref(int,int,int);
    virtual OctreeLeaf* getleaf(int,int,int);
    virtual BlockId getser(int,int,int);
    virtual void manifestgeom(int,int,int);
    virtual void manifestgeomsnippets(int,int,int,int);
    
    virtual void prepare(int,int,int);
    
    virtual void filesave();
    virtual void worldfilesave(int,int,int);
    virtual Edgedat& xcon(int,int,int);
    virtual Edgedat& ycon(int,int,int);
    virtual Edgedat& zcon(int,int,int);
    
    virtual int feat(int,int,int);
    virtual glm::vec3& getfeature(int,int,int);
    virtual glm::vec3& getnormal(int,int,int);
    
    
    virtual void insertinto(int,int,int,int,int,OctreeSegment*,OctreeSegment*&);
    
    virtual void determinelod(int,int,int,int,int,int,bool,OctreeSegment*);
    
    virtual int getlod(int,int,int);
    
    virtual void testconnected(int,int,int,OctreeSegment*,BranchRegistry*);
    virtual OctreeSegment* pullaway(int,int,int,int,OctreeSegment*&);
    virtual uint8_t giveconflag(int,int,int);
    virtual uint8_t giveimconflag();
};
typedef union {
    float f;
    long l;
    int i[2];
} floatint;

struct OctreeFeature : OctreeSegment {
    floatint data[4];
    int point;
    BlockId fillvalue;
    uint8_t conflag;
    OctreeFeature(BlockId,uint8_t);
//    MatrixCarriage getqef() override;
    virtual int feat(int,int,int) override;
    std::pair<BlockId,OctreeLeaf*> indivref(int,int,int) override;
    BlockId getser(int,int,int) override;
    MatrixCarriage vertify(int,int,int) override;
    void filesave() override;
    uint8_t giveimconflag() override;
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId,uint8_t);
    OctreeLeaf(BlockId,uint8_t,int8_t[12]);
    std::pair<BlockId,OctreeLeaf*> indivref(int,int,int) override;
    OctreeLeaf* getleaf(int,int,int) override;
    void manifestgeom(int,int,int) override;
    void manifestgeomsnippets(int,int,int,int) override;
    MatrixCarriage vertify(int,int,int) override;
    void filesave() override;
    Edgedat& xcon(int,int,int) override;
    Edgedat& ycon(int,int,int) override;
    Edgedat& zcon(int,int,int) override;
};
struct OctreeBud : OctreeSegment {
    BlockId fillvalue;
    OctreeBud(BlockId);
    BlockId getser(int,int,int) override;
    void filesave() override;
    void worldfilesave(int,int,int) override;
    void insertinto(int,int,int,int,int,OctreeSegment*,OctreeSegment*&) override;
    std::pair<BlockId,OctreeLeaf*> indivref(int,int,int) override;
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
    
    void determinelod(int,int,int,int,int,int,bool,OctreeSegment*) override;

    int getlod(int,int,int) override;
//    MatrixCarriage getqef() override;
    MatrixCarriage vertify(int,int,int) override;
    void worldfilesave(int,int,int) override;
    void geomify(int,int,int) override;
    void manifestgeom(int,int,int) override;
    void manifestgeomsnippets(int,int,int,int) override;
    
    void testcoherence() override;

    void prepare(int,int,int) override;

//    void hermitify(BlockLoc,BlockLoc,BlockLoc) override;
    OctreePortionAwareBranch* getvoxunit(int x,int y,int z) override;
    std::pair<BlockId,OctreeLeaf*> indivref(int,int,int) override;
    OctreeLeaf* getleaf(int,int,int) override;
    BlockId getser(int,int,int) override;

    void filesave() override;
    Edgedat& xcon(int,int,int) override;
    Edgedat& ycon(int,int,int) override;
    Edgedat& zcon(int,int,int) override;

    virtual int feat(int,int,int) override;
    virtual glm::vec3& getfeature(int,int,int) override;
    virtual glm::vec3& getnormal(int,int,int) override;

    void insertinto(int,int,int,int,int,OctreeSegment*,OctreeSegment*&) override;
    void testconnected(int x,int y,int z,OctreeSegment*,BranchRegistry*) override;
    bool phase2check(int x,int y,int z,OctreeSegment*,Environment*);
    
    OctreeSegment* pullaway(int,int,int,int,OctreeSegment*&) override;
    
    uint8_t giveconflag(int,int,int) override;
    uint8_t giveimconflag() override;
};
struct OctreePortionAwareBranch : OctreeBranch {
//    bool changed    = false;
//    float* grdata = 0;
    

    int otherlods[7] = {-1,-1,-1,-1,-1,-1,-1};
    int lod = -1;
//    bool softload = true;
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
    
//    OctreePortionAwareBranch(float*);
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
    
    void worldfilesave(int,int,int) override;
    
    void prepare(int,int,int) override;
    int getlod(int,int,int) override;
    void determinelod(int,int,int,int,int,int,bool,OctreeSegment*) override;
    void geomify(int,int,int) override;
    uint8_t giveconflag(int,int,int) override;
    OctreePortionAwareBranch* getvoxunit(int,int,int) override;
    OctreeSegment* pullaway(int,int,int,int,OctreeSegment*&) override;
    
    glm::vec3& getfeature(int,int,int) override;
    glm::vec3& getnormal(int,int,int) override;
    
//    void updatevisual(BlockLoc x,BlockLoc y,BlockLoc z);
    bool surroundinglod(int x,int y,int z);
    
    void bake();
};

void glassert();




OctreeSegment* loadWorldFile(std::ifstream& file,int x,int y,int z,int recur);
void tearaway(int,int,int,int,OctreeSegment*,Environment*);
bool testConnection(int,int,int,int,int,int,int,OctreeSegment*,Environment*);
OctreeSegment* makeOctree(BlockId*,int,int,int,int);

OctreeSegment* makeOctree(std::ifstream&,int);

#endif /* defined(__Total_Control__octree__) */
