//
//  GeometryOctree.hpp
//  Total Control
//
//  Created by Parker on 3/19/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef GeometryOctree_hpp
#define GeometryOctree_hpp

#include <stdio.h>

#include "renderterrain.h"
#include <climits>
#include "constants.h"
#include "octree.h"
#include <map>
//#include "glm"

struct GeometryOctreeSegment {
    virtual void render(const glm::mat4&);
    virtual void insertinto(long,long,long,int,GeometryOctreeSegment*,GeometryOctreeSegment*&);
    virtual bool existsat(long,long,long);
    long xs;
    long ys;
    long zs;
    int recurs;
    GeometryOctreeSegment(long,long,long,int);
    bool checkbounds(const glm::mat4&);
};
struct GeometryOctreeBranch : GeometryOctreeSegment {
    GeometryOctreeSegment* subdivisions[2][2][2];
    GeometryOctreeBranch(GeometryOctreeSegment*,GeometryOctreeSegment*,
                         GeometryOctreeSegment*,GeometryOctreeSegment*,
                         GeometryOctreeSegment*,GeometryOctreeSegment*,
                         GeometryOctreeSegment*,GeometryOctreeSegment*,long,long,long,int);
    void render(const glm::mat4&) override;
    void insertinto(long,long,long,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
    bool existsat(long,long,long) override;
};
struct GeometryOctreeLeaf : GeometryOctreeSegment {
    GeometryOctreeLeaf(long,long,long,int);
    std::map<uint8_t,GeomTerrain> geometry;
    void render(const glm::mat4&) override;
    void insertinto(long,long,long,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
    bool existsat(long,long,long) override;
};
struct GeometryOctreeBud : GeometryOctreeSegment {
    GeometryOctreeBud(long,long,long,int);
    void render(const glm::mat4&) override;
    void insertinto(long,long,long,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
    bool existsat(long,long,long) override;
};
struct GeometryOctreePlaceholder : GeometryOctreeBud {
    bool existsat(long,long,long) override;
};
class GeometryOctree {
private:
    long flipbits(long);
    void expand(long,long,long);
    int underpressure(long,long,long);
    int depth = 0;
    GeometryOctreeSegment* data = new GeometryOctreeBud(0,0,0,0);
    glm::mat4& matrix;
    Octree& voxes;
public:
    GeometryOctree(glm::mat4&,Octree&);
    void render();
    bool existsat(long,long,long);
    void manifest(long,long,long);
//    void draw(long,long,long,long,long,long,Octree*);
};


#endif /* GeometryOctree_hpp */
