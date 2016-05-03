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
#include <fstream>
#include <boost/filesystem.hpp>



struct Edgedat {
    uint8_t t;
    
    uint8_t x;
    uint8_t y;
    uint8_t z;
    Edgedat();
    Edgedat(float,float,float,float);
    Edgedat(uint8_t ct,uint8_t cx,uint8_t cy,uint8_t cz);
    
    glm::vec3 getnorm();
    float getoffset();
//    glm::vec3 getpointx();
//    glm::vec3 getpointy();
//    glm::vec3 getpointz();
};
struct Feature {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    glm::vec3 get();
    void operator=(const glm::vec3&);
};
struct MatrixCarriage {
    double bb;
    double Ab1;
    double Ab2;
    double Ab3;
    double AA1;
    double AA2;
    double AA3;
    double AA4;
    double AA5;
    double AA6;
    friend MatrixCarriage operator+(const MatrixCarriage&, const MatrixCarriage&);
    MatrixCarriage();
    MatrixCarriage(double,double,double,double,double,double,double,double,double,double);
};

//uint8_t gtable[2][6][2];



struct OctreeSegment {
//    static OctreeSegment* makeOctree(std::istream&,int);
    //    bool isfilled;
    virtual void geomify(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*);
    virtual void geomifyselection(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*);
    virtual void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*);
    virtual void hermitifyselection(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*);
    virtual OctreeSegment* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z);
    virtual BlockId getser(BlockLoc,BlockLoc,BlockLoc);
    virtual void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&);
    virtual void collapse(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*&);
//    virtual bool uniqueat(long,long,long,int);
    virtual void filesave(std::ostream&);
    virtual Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc);
    virtual Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc);
    virtual Feature& feat(BlockLoc,BlockLoc,BlockLoc);
    virtual BlockId getsimpid();
    virtual void insertinto(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,OctreeSegment*&);
    virtual bool featuresque(BlockLoc,BlockLoc,BlockLoc);
};
struct OctreeFeature : OctreeSegment {
    BlockId fillvalue;
    Feature point;
    
    OctreeFeature(BlockId);
    BlockId getsimpid() override;
    
    Feature& feat(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    void hermitifyselection(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
    void filesave(std::ostream&) override;
    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId);
    OctreeLeaf(BlockId,uint8_t[12]);
    
    //    bool uniqueat(long,long,long,int) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    void geomifyselection(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeBud : OctreeSegment {
    BlockId fillvalue;
    OctreeBud(BlockId);
    
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void filesave(std::ostream&) override;
    BlockId getsimpid() override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,OctreeSegment*&) override;
    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeBranch : OctreeSegment {
    int depth;
    OctreeSegment* subdivisions[2][2][2];
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,int);
    
    
    //    void geomifyportion(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    void geomify(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    void hermitify(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    void geomifyselection(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*,OctreeSegment*) override;
    void hermitifyselection(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
//    void hermitifyportion(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*) override;
    OctreeSegment* getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) override;
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
    
//    void collapse(long,long,long,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void filesave(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
    Feature& feat(BlockLoc,BlockLoc,BlockLoc) override;
    BlockId getsimpid() override;
    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,OctreeSegment*,OctreeSegment*&) override;
    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
};
class Octree {
private:
    void expand(BlockLoc,BlockLoc,BlockLoc);
    void expandchunk(BlockLoc,BlockLoc,BlockLoc);
    void expand(int);
    int underpressure(BlockLoc,BlockLoc,BlockLoc);
    int depth = 0;
    int bakeddetails = 0;
public:
    GeometryOctree realworld;
    Octree(glm::mat4&);
    OctreeSegment* data = new OctreeBud(0);
    void loadportion(BlockLoc,BlockLoc,BlockLoc,BlockId (*)[CHSIZE+1][CHSIZE+1]);
//    void loadfromfile(BlockLoc,BlockLoc,BlockLoc,int,std::string);
    void render();
    void manifest(BlockLoc,BlockLoc,BlockLoc);
    bool existsat(BlockLoc,BlockLoc,BlockLoc);
    void filepullportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    void filepushportion(std::string,BlockLoc,BlockLoc,BlockLoc);
    bool dataexists(std::string,BlockLoc,BlockLoc,BlockLoc);
//    void push
//    void setAt(BlockLoc,BlockLoc,BlockLoc,BlockId);
//    BlockId getAt(BlockLoc,BlockLoc,BlockLoc);
//    Edgedat& conx(BlockLoc,BlockLoc,BlockLoc);
//    Edgedat& cony(BlockLoc,BlockLoc,BlockLoc);
//    Edgedat& conz(BlockLoc,BlockLoc,BlockLoc);
//    Feature& feat(BlockLoc,BlockLoc,BlockLoc);
    void hermitify(BlockLoc,BlockLoc,BlockLoc);
    
    void voxXskirt(BlockLoc,BlockLoc,BlockLoc);
    void voxYskirt(BlockLoc,BlockLoc,BlockLoc);
//    void voxZskirt(BlockLoc,BlockLoc,BlockLoc);
//    
//    void voxXrow(BlockLoc,BlockLoc,BlockLoc);
//    void voxYrow(BlockLoc,BlockLoc,BlockLoc);
//    void voxZrow(BlockLoc,BlockLoc,BlockLoc);
//    
//    void voxXYZcorner(BlockLoc,BlockLoc,BlockLoc);
    void voxsnippets(BlockLoc,BlockLoc,BlockLoc);
};


glm::vec3 omgqef(glm::vec3[],glm::vec3[],int);
OctreeSegment* makeOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);

#endif /* defined(__Total_Control__octree__) */
