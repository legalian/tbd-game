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
#include <stdint.h>
#include <fstream>
#include <climits>

typedef uint8_t BlockId;
typedef int BlockLoc;

#define ALTERNATOR (((INT_MAX/3)<<1)|1)
#define ALTALTERNATOR (ALTERNATOR<<1)

struct Edgedat {
    uint8_t t;
    
    uint8_t x;
    uint8_t y;
    uint8_t z;
    Edgedat();
    Edgedat(float,float,float,float);
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



struct OctreeSegment {
//    static OctreeSegment* makeOctree(std::istream&,int);
    //    bool isfilled;
    virtual BlockId getser(BlockLoc,BlockLoc,BlockLoc);
    virtual void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&);
    virtual void collapse(BlockLoc,BlockLoc,BlockLoc,OctreeSegment*&);
//    virtual bool uniqueat(long,long,long,int);
    virtual void savetofile(std::ostream&);
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
    void setser(BlockLoc,BlockLoc,BlockLoc,uint8_t,int,OctreeSegment*&) override;
    void savetofile(std::ostream&) override;
    bool featuresque(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(BlockId);
    
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
    Edgedat& xcon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& ycon(BlockLoc,BlockLoc,BlockLoc) override;
    Edgedat& zcon(BlockLoc,BlockLoc,BlockLoc) override;
};
struct OctreeBud : OctreeSegment {
    uint8_t fillvalue;
    OctreeBud(uint8_t);
    
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
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
    
    BlockId getser(BlockLoc,BlockLoc,BlockLoc) override;
    void setser(BlockLoc,BlockLoc,BlockLoc,BlockId,int,OctreeSegment*&) override;
    
//    void collapse(long,long,long,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
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
    glm::vec3 omgqef(glm::vec3[],glm::vec3[],int);
    OctreeSegment* data = new OctreeBud(0);
public:
    void loadportion(BlockLoc,BlockLoc,BlockLoc,BlockId (*)[CHSIZE+1][CHSIZE+1]);
    void loadfromfile(BlockLoc,BlockLoc,BlockLoc,int,std::string);
    void setAt(BlockLoc,BlockLoc,BlockLoc,BlockId);
    BlockId getAt(BlockLoc,BlockLoc,BlockLoc);
    Edgedat& conx(BlockLoc,BlockLoc,BlockLoc);
    Edgedat& cony(BlockLoc,BlockLoc,BlockLoc);
    Edgedat& conz(BlockLoc,BlockLoc,BlockLoc);
    Feature& feat(BlockLoc,BlockLoc,BlockLoc);
    void hermitify(BlockLoc,BlockLoc,BlockLoc);
    void save(std::string);
};


OctreeSegment* makeOctree(BlockId (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);

#endif /* defined(__Total_Control__octree__) */
