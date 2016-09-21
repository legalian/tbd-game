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
#include "constants.h"
#include "materials.h"
#include "geometryoctree.h"
#include "qef.h"
#include <fstream>
#include <set>
#include <boost/filesystem.hpp>

struct Octree;
struct OctreeSegment;
struct OctreeBranch;
struct OctreePortionAwareBranch;
class Environment;



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
    std::vector<PathTesterBucket*> buckets;
    PathTesterPool(Location,int);
    ~PathTesterPool();
    int hash(Location);
    bool contains(Location,int);
    void add(Location,int);
    int getpromising();
    Location poppromising(int);
};
struct BranchRegistry {
    std::vector<std::vector<std::pair<Location,OctreeBranch*>>> registry;
    Environment& capsulepointer;
    BranchRegistry(Environment&);
    void add(Location,OctreeBranch*);
    void prune(OctreeSegment*);
    bool contains(OctreeBranch*);
};
struct Feature {
    uint8_t x;
    uint8_t y;
    uint8_t z;
//    glm::vec3 TESTTESTTEST;
    glm::vec3 get(int);
//    void operator=(const glm::vec3&);
    Feature();
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
struct Edgedat {
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
//    glm::vec3 getpointx();
//    glm::vec3 getpointy();
//    glm::vec3 getpointz();
};


struct OctreeSegment {
//    static OctreeSegment* makeOctree(std::istream&,int);
    //    bool isfilled;
    virtual MatrixCarriage getqef();
    virtual void vertify(BlockLoc,BlockLoc,BlockLoc,int);
    virtual bool isvbaked(BlockLoc,BlockLoc,BlockLoc);
    virtual void geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,int);
    virtual void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*);
    virtual OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z);
    virtual BlockId getser(BlockLoc,BlockLoc,BlockLoc);
    virtual BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc);
//    virtual int getlodat(BlockLoc,BlockLoc,BlockLoc);
//    virtual void setlodat(BlockLoc,BlockLoc,BlockLoc,int);
    
//    virtual void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&);
//    virtual void collapse(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*&);
//    virtual bool uniqueat(long,long,long,int);
    virtual void filesave(std::ostream&);
    virtual Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc);
    virtual glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int);
    virtual glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc,int);
//    virtual BlockId getsimpid();
    virtual void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&);
//    virtual bool featuresque(BlockLoc,BlockLoc,BlockLoc);
    
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
//    BlockId getsimpid() override;
    
    MatrixCarriage getqef() override;
    
#ifdef WIREFRAMEDEBUG
    void geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,int) override;
#endif
    glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int) override;
    glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc,int) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
//    void hermitifyselection(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    void vertify(BlockLoc,BlockLoc,BlockLoc,int target) override;
//    void vertifyselection(BlockLoc,BlockLoc,BlockLoc,int target) override;
//    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
    void filesave(std::ostream&) override;
    //    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
    
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId,uint8_t);
    OctreeLeaf(BlockId,uint8_t,int8_t[12]);
    
    //    bool uniqueat(long,long,long,int) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,int) override;
//    void geomifyselectionX(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
//    void geomifyselectionY(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
//    void geomifyselectionZ(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
    
//    bool giveXflag(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*) override;
//    bool giveYflag(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*) override;
//    bool giveZflag(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*) override;
//    bool giveWflag(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*) override;
};
struct OctreeBud : OctreeSegment {
    BlockId fillvalue;
    
    OctreeBud(BlockId);
    
    
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
//    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void filesave(std::ostream&) override;
//    BlockId getsimpid() override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
//    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
    
    //    void runseperation(BlockLoc x,BlockLoc y,BlockLoc z,int,OctreeSegment*) override;
    
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
};
struct OctreeBranch : OctreeSegment {
    int depth;
    uint8_t connections = 0;
    uint8_t DEBUGDEBUG = 0;
//    uint8_t DEBUGDEBUGDEBUG = 0;
    
    Feature point;
    OctreeSegment* subdivisions[2][2][2];
    
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int);
    //    void geomifyportion(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    MatrixCarriage getqef() override;
//    void drawgeometry(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*);
    void vertify(BlockLoc,BlockLoc,BlockLoc,int) override;
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,int) override;
    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
//    int getlodat(BlockLoc,BlockLoc,BlockLoc) override;
//    void setlodat(BlockLoc,BlockLoc,BlockLoc,int) override;
//    void vertifyselection(BlockLoc,BlockLoc,BlockLoc,int target) override;
//    void geomifyselectionX(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
//    void geomifyselectionY(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
//    void geomifyselectionZ(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId) override;
//    void hermitifyselection(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
//    void hermitifyportion(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
//    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
    
//    void collapse(long,long,long,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
    glm::vec3 feat(BlockLoc,BlockLoc,BlockLoc,int) override;
    glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc,int) override;
//    BlockId getsimpid() override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,int,OctreeSegment*,OctreeSegment*&) override;
//    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
    
    void testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,BranchRegistry*) override;
    bool phase2check(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*,Environment*);
    
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
    
    uint8_t giveconflag(BlockLoc,BlockLoc,BlockLoc,int) override;
};
struct OctreePortionAwareBranch : OctreeBranch {
    bool changed = false;
    int bakeddetails = 0;
    int curlod = -1;
    int lodserial[7] = {-1,-1,-1,-1,-1,-1,-1};
    std::map<uint8_t,GeomTerrain>* geometry = NULL;
    std::map<uint8_t,GeomTerrain>* nextpasscleanup = NULL;
    OctreePortionAwareBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                             OctreeSegment*,OctreeSegment*,int);
    void vertify(BlockLoc,BlockLoc,BlockLoc,int) override;
    bool isvbaked(BlockLoc,BlockLoc,BlockLoc) override;
    glm::vec3 featwrt(BlockLoc,BlockLoc,BlockLoc,int) override;
    BlockId getserwrt(BlockLoc,BlockLoc,BlockLoc) override;
//    int getlodat(BlockLoc,BlockLoc,BlockLoc) override;
//    void setlodat(BlockLoc,BlockLoc,BlockLoc,int) override;
    OctreePortionAwareBranch* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    OctreeSegment* pullaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*&) override;
};
class Octree {
//private:
public:
    void expand(BlockLoc,BlockLoc,BlockLoc);
    void expandchunk(BlockLoc,BlockLoc,BlockLoc);
    void expandarbit(BlockLoc,BlockLoc,BlockLoc,int);
    void expand(int);
    int underpressure(BlockLoc,BlockLoc,BlockLoc);
    int depth = 0;
    int popular;
    BranchRegistry currenttests;
    
//    GeometryOctreeLeaf* opengeoforediting(BlockLoc x,BlockLoc y,BlockLoc z);
    GeometryOctree realworld;
    Octree(glm::mat4&,Environment&,int);
    OctreeSegment* data = new OctreeBud(0);
    void loadportion(BlockLoc,BlockLoc,BlockLoc,BlockId (*)[CHSIZE+1][CHSIZE+1]);
//    void loadfromfile(BlockLoc,BlockLoc,BlockLoc,int,std::string);
    void render();
    void h_manifest(BlockLoc,BlockLoc,BlockLoc);
    void v_manifest(BlockLoc,BlockLoc,BlockLoc);
    void g_manifest(BlockLoc,BlockLoc,BlockLoc);
    bool existsat(BlockLoc,BlockLoc,BlockLoc);
    void filepullportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    void filepushportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    bool dataexists(std::string,BlockLoc,BlockLoc,BlockLoc);
//    void voxsnippets(BlockLoc,BlockLoc,BlockLoc);
};

//inline void drawgeometry(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int,BlockId);
#ifdef WIREFRAMEDEBUG
void drawdebugcube(float,float,float,float,float,float,GeometryOctreeLeaf*,float,float,float);
#endif
inline void Xstitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId);
inline void Ystitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId);
inline void Zstitch(BlockLoc,BlockLoc,BlockLoc,std::map<uint8_t,GeomTerrain>*,OctreeSegment*,BlockId,BlockId);
//inline void Xstitch(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId,BlockId);
//inline void Ystitch(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId,BlockId);
//inline void Zstitch(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*,int*,BlockId,BlockId);
//glm::vec3 omgqef(glm::vec3[],glm::vec3[],int);
void tearaway(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
bool testConnection(BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,Environment*);
OctreeSegment* makeOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);
//OctreeSegment* makePortionOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);
OctreeSegment* makeOctree(std::ifstream&,int);
//OctreeSegment* makePortionOctree(std::ifstream&,int);

#endif /* defined(__Total_Control__octree__) */
