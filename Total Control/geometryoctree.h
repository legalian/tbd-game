////
////  GeometryOctree.hpp
////  Total Control
////
////  Created by Parker on 3/19/16.
////  Copyright © 2016 Parker Lawrence. All rights reserved.
////
//
//#ifndef GeometryOctree_hpp
//#define GeometryOctree_hpp
//
//#include <stdio.h>
//
//#include "renderterrain.h"
//#include <climits>
//#include "constants.h"
//#include "materials.h"
////#include "octree.h"
//#include <map>
////#include "glm"
//class GeometryOctreeLeaf;
//
//struct GeometryOctreeSegment {
//    virtual void debugprinttrace(int offset);
//    virtual void render(const glm::mat4&);
//    virtual void insertinto(BlockLoc,BlockLoc,BlockLoc,int,GeometryOctreeSegment*,GeometryOctreeSegment*&);
//    virtual bool existsat(BlockLoc,BlockLoc,BlockLoc);
//    virtual GeometryOctreeLeaf* getgeomat(BlockLoc,BlockLoc,BlockLoc);
//    BlockLoc xs;
//    BlockLoc ys;
//    BlockLoc zs;
//    int recurs;
//    GeometryOctreeSegment(BlockLoc,BlockLoc,BlockLoc,int);
//    bool checkbounds(const glm::mat4&);
//};
//struct GeometryOctreeBranch : GeometryOctreeSegment {
//    GeometryOctreeSegment* subdivisions[2][2][2];
//    void debugprinttrace(int offset) override;
//    GeometryOctreeBranch(GeometryOctreeSegment*,GeometryOctreeSegment*,
//                         GeometryOctreeSegment*,GeometryOctreeSegment*,
//                         GeometryOctreeSegment*,GeometryOctreeSegment*,
//                         GeometryOctreeSegment*,GeometryOctreeSegment*,BlockLoc,BlockLoc,BlockLoc,int);
//    void render(const glm::mat4&) override;
//    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
//    bool existsat(BlockLoc,BlockLoc,BlockLoc) override;
//    GeometryOctreeLeaf* getgeomat(BlockLoc,BlockLoc,BlockLoc) override;
//};
//struct GeometryOctreeLeaf : GeometryOctreeSegment {
//    GeometryOctreeLeaf(BlockLoc,BlockLoc,BlockLoc,int);
//    std::map<uint8_t,GeomTerrain>* geometry = NULL;
//    std::map<uint8_t,GeomTerrain>* nextpasscleanup = NULL;
//    void render(const glm::mat4&) override;
//    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
//    bool existsat(BlockLoc,BlockLoc,BlockLoc) override;
//    GeometryOctreeLeaf* getgeomat(BlockLoc,BlockLoc,BlockLoc) override;
//    
//    void matrixmap(glm::mat4*);
////    void securecore();
////    void preparesnippets();
////    void removeseal();
////    void erase();
//};
//struct GeometryOctreeBud : GeometryOctreeSegment {
//    GeometryOctreeBud(BlockLoc,BlockLoc,BlockLoc,int);
//    void render(const glm::mat4&) override;
//    void insertinto(BlockLoc,BlockLoc,BlockLoc,int,GeometryOctreeSegment*,GeometryOctreeSegment*&) override;
//    bool existsat(BlockLoc,BlockLoc,BlockLoc) override;
//};
////struct GeometryOctreePlaceholder : GeometryOctreeBud {
////    bool existsat(BlockLoc,BlockLoc,BlockLoc) override;
////};
//class GeometryOctree {
//private:
//    int underpressure(BlockLoc,BlockLoc,BlockLoc);
////    Octree& voxes;
//    BlockLoc reduce = 0;
//public:
//    int depth = 0;
//    GeometryOctreeSegment* data = new GeometryOctreeBud(0,0,0,0);
//    glm::mat4& matrix;
//    BlockLoc flipbits(BlockLoc);
//    void expand(BlockLoc,BlockLoc,BlockLoc);
//    GeometryOctree(glm::mat4&);
//    void render();
//    GeometryOctreeLeaf* getgeomat(BlockLoc,BlockLoc,BlockLoc);
//    bool existsat(BlockLoc,BlockLoc,BlockLoc);
//    void insertinto(BlockLoc,BlockLoc,BlockLoc,GeometryOctreeLeaf*);
////    void manifest(BlockLoc,BlockLoc,BlockLoc);
////    void draw(long,long,long,long,long,long,Octree*);
//};
//
//
//#endif /* GeometryOctree_hpp */
