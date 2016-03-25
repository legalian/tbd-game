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

#define ALTERNATOR (((LONG_MAX/3)<<1)|1)

struct Edgedat {
    uint8_t t;
    
    uint8_t x;
    uint8_t y;
    uint8_t z;
    Edgedat();
    Edgedat(uint8_t,uint8_t,uint8_t,uint8_t);
    glm::vec3 getnorm();
    float getoffset();
};
struct Feature {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    glm::vec3 get();
    void operator=(const glm::vec3&);
};

struct OctreeSegment {
    static OctreeSegment* makeOctree(uint8_t (*)[CHSIZE+1][CHSIZE+1],int,int,int,int);
//    static OctreeSegment* makeOctree(std::istream&,int);
    //    bool isfilled;
    virtual uint8_t getser(long,long,long);
    virtual void setser(long,long,long,uint8_t,int,OctreeSegment*&);
    virtual void collapse(long,long,long,OctreeSegment*&);
//    virtual bool uniqueat(long,long,long,int);
    virtual void savetofile(std::ostream&);
    virtual Edgedat& xcon(long,long,long);
    virtual Edgedat& ycon(long,long,long);
    virtual Edgedat& zcon(long,long,long);
    virtual Feature& feat(long,long,long);
    virtual uint8_t getsimpid();
    virtual void insertinto(long,long,long,int,OctreeSegment*,OctreeSegment*&);
    virtual bool featuresque(long,long,long);
};
struct OctreeFeature : OctreeSegment {
    uint8_t fillvalue;
    Feature point;
    
    OctreeFeature(uint8_t);
    uint8_t getsimpid() override;
    
    Feature& feat(long,long,long) override;
    uint8_t getser(long,long,long) override;
    void setser(long,long,long,uint8_t,int,OctreeSegment*&) override;
    void savetofile(std::ostream&) override;
    bool featuresque(long,long,long) override;
};
struct OctreeLeaf : OctreeFeature {
    Edgedat xcondat;
    Edgedat ycondat;
    Edgedat zcondat;
    OctreeLeaf(uint8_t);
    
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
    Edgedat& xcon(long,long,long) override;
    Edgedat& ycon(long,long,long) override;
    Edgedat& zcon(long,long,long) override;
};
struct OctreeBud : OctreeSegment {
    uint8_t fillvalue;
    OctreeBud(uint8_t);
    
    uint8_t getser(long,long,long) override;
    void setser(long,long,long,uint8_t,int,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
    uint8_t getsimpid() override;
    void insertinto(long,long,long,int,OctreeSegment*,OctreeSegment*&) override;
    bool featuresque(long,long,long) override;
};
struct OctreeBranch : OctreeSegment {
    OctreeSegment* subdivisions[2][2][2];
    OctreeBranch(OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*,
                 OctreeSegment*,OctreeSegment*);
    
    uint8_t getser(long,long,long) override;
    void setser(long,long,long,uint8_t,int,OctreeSegment*&) override;
    
//    void collapse(long,long,long,OctreeSegment*&) override;
//    bool uniqueat(long,long,long,int) override;
    void savetofile(std::ostream&) override;
    Edgedat& xcon(long,long,long) override;
    Edgedat& ycon(long,long,long) override;
    Edgedat& zcon(long,long,long) override;
    Feature& feat(long,long,long) override;
    uint8_t getsimpid() override;
    void insertinto(long,long,long,int,OctreeSegment*,OctreeSegment*&) override;
    bool featuresque(long,long,long) override;
};
class Octree {
private:
    long flipbits(long);
    void expand(long,long,long);
    void expand(long,long,long,int);
    void expand(int);
    int underpressure(long,long,long);
    int depth = 0;
    glm::vec3 omgqef(glm::vec3[],glm::vec3[],int);
    OctreeSegment* data = new OctreeBud(0);
public:
    void loadportion(long,long,long,uint8_t (*)[CHSIZE+1][CHSIZE+1]);
    void loadfromfile(long,long,long,int,std::string);
    void setAt(long,long,long,uint8_t);
    uint8_t getAt(long,long,long);
    Edgedat& conx(long,long,long);
    Edgedat& cony(long,long,long);
    Edgedat& conz(long,long,long);
    Feature& feat(long,long,long);
    void hermitify(long,long,long);
    void save(std::string);
};



#endif /* defined(__Total_Control__octree__) */
