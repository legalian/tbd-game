//
//  GeometryOctree.cpp
//  Total Control
//
//  Created by Parker on 3/19/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "geometryoctree.h"


void GeometryOctreeSegment::render(const glm::mat4& matr) {throw;}
void GeometryOctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self) {
    throw;
}
bool GeometryOctreeSegment::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}


GeometryOctreeSegment::GeometryOctreeSegment(BlockLoc i,BlockLoc j,BlockLoc k,int rec) : xs(i),ys(j),zs(k),recurs(rec) {}

bool GeometryOctreeSegment::checkbounds(const glm::mat4& matr) {
    return true;
}
GeometryOctreeBranch::GeometryOctreeBranch(GeometryOctreeSegment* a,GeometryOctreeSegment* b,
                                           GeometryOctreeSegment* c,GeometryOctreeSegment* d,
                                           GeometryOctreeSegment* e,GeometryOctreeSegment* f,
                                           GeometryOctreeSegment* g,GeometryOctreeSegment* h,
                                           BlockLoc i,BlockLoc j,BlockLoc k,int rec) : GeometryOctreeSegment(i,j,k,rec) {
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = b;
    subdivisions[0][1][0] = c;
    subdivisions[1][1][0] = d;
    subdivisions[0][0][1] = e;
    subdivisions[1][0][1] = f;
    subdivisions[0][1][1] = g;
    subdivisions[1][1][1] = h;
}

void GeometryOctreeBranch::render(const glm::mat4& matr) {
    if (checkbounds(matr)) {
        subdivisions[0][0][0]->render(matr);
        subdivisions[1][0][0]->render(matr);
        subdivisions[0][1][0]->render(matr);
        subdivisions[1][1][0]->render(matr);
        subdivisions[0][0][1]->render(matr);
        subdivisions[1][0][1]->render(matr);
        subdivisions[0][1][1]->render(matr);
        subdivisions[1][1][1]->render(matr);
    }
}
void GeometryOctreeBranch::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self) {
    subdivisions[x&1][y&1][z&1]->insertinto(x>>1,y>>1,z>>1,recur-1,toinsert,subdivisions[x&1][y&1][z&1]);
}
bool GeometryOctreeBranch::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions[x&1][y&1][z&1]->existsat(x>>1,y>>1,z>>1);
}

GeometryOctreeLeaf::GeometryOctreeLeaf(BlockLoc i,BlockLoc j,BlockLoc k,int rec) : GeometryOctreeSegment(i,j,k,rec) {}

void GeometryOctreeLeaf::render(const glm::mat4& matr){
    if (checkbounds(matr)) {
        
        for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
            // iterator->first = key
            // iterator->second = value
            // Repeat if you also want to iterate through the second map.
            registergeom(iterator->first,&iterator->second);
        }
    }
}
void GeometryOctreeLeaf::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self){
    delete self;
    self = toinsert;
}
bool GeometryOctreeLeaf::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return true;
}

GeometryOctreeBud::GeometryOctreeBud(BlockLoc i,BlockLoc j,BlockLoc k,int rec) : GeometryOctreeSegment(i,j,k,rec) {}

void GeometryOctreeBud::render(const glm::mat4& matr) {}
void GeometryOctreeBud::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self){
    delete self;
    if (recur==0) {
        self = toinsert;
    } else {
        BlockLoc add = 1<<(recurs-1);
        int pa = recurs-1;
        self = new GeometryOctreeBranch(new GeometryOctreeBud(xs,ys,zs,pa),new GeometryOctreeBud(xs+add,ys,zs,pa),
                                        new GeometryOctreeBud(xs,ys+add,zs,pa),new GeometryOctreeBud(xs+add,ys+add,zs,pa),
                                        new GeometryOctreeBud(xs,ys,zs+add,pa),new GeometryOctreeBud(xs+add,ys,zs+add,pa),
                                        new GeometryOctreeBud(xs,ys+add,zs+add,pa),new GeometryOctreeBud(xs+add,ys+add,zs+add,pa)
                                        ,xs,ys,zs,recurs);
        self->insertinto(x,y,z,recur,toinsert,self);
    }
}
bool GeometryOctreeBud::existsat(BlockLoc x, BlockLoc y, BlockLoc z) {
    return false;
}
bool GeometryOctreePlaceholder::existsat(BlockLoc x, BlockLoc y, BlockLoc z) {
    return true;
}

void GeometryOctree::expand(BlockLoc x,BlockLoc y,BlockLoc z) {
    int desireddepth = underpressure(x,y,z);
    while (desireddepth>depth) {
        depth++;
        BlockLoc pos = -1 * (1<<((depth-1)>>1));
        BlockLoc max = pos + (1<<(depth-1));
        
        if (depth&1) {
            data = new GeometryOctreeBranch(new GeometryOctreeBud(pos,pos,pos,depth-1),new GeometryOctreeBud(max,pos,pos,depth-1),
                                            new GeometryOctreeBud(pos,max,pos,depth-1),new GeometryOctreeBud(max,max,pos,depth-1),
                                            new GeometryOctreeBud(pos,pos,max,depth-1),new GeometryOctreeBud(max,pos,max,depth-1),
                                            new GeometryOctreeBud(pos,max,max,depth-1),data,pos,pos,pos,depth);
        } else {
            data = new GeometryOctreeBranch(data,new GeometryOctreeBud(max,pos,pos,depth-1),
                                            new GeometryOctreeBud(pos,max,pos,depth-1),new GeometryOctreeBud(max,max,pos,depth-1),
                                            new GeometryOctreeBud(pos,pos,max,depth-1),new GeometryOctreeBud(max,pos,max,depth-1),
                                            new GeometryOctreeBud(pos,max,max,depth-1),new GeometryOctreeBud(max,max,max,depth-1),
                                            pos,pos,pos,depth);
        }
    }
}

int GeometryOctree::underpressure(BlockLoc x,BlockLoc y,BlockLoc z) {
    x = (x+ALTERNATOR)^ALTERNATOR;
    y = (y+ALTERNATOR)^ALTERNATOR;
    z = (z+ALTERNATOR)^ALTERNATOR;
    BlockLoc mo = std::max(std::max(x,y),z);
    int r;
    for (r=0;(1<<r)<=mo;r++) {}
    return r;
}
BlockLoc GeometryOctree::flipbits(BlockLoc f) {
    BlockLoc k = 0;
    BlockLoc dist = f+ALTERNATOR;
    for (int w=0;w<depth;w++) {
        //        k|= (dist&(1<<w))>>(desireddepth-1-(w<<1));
//        if(dist & (1 << w)) {
//            k |= 1 << ((depth - 1) - w);
//        }
        k<<=1;
        k|=dist&1;
        dist>>=1;
    }
    return k;
}


void GeometryOctree::render() {data->render(matrix);}
bool GeometryOctree::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (underpressure(x,y,z)>depth) {return false;}
    return data->existsat(flipbits(x),flipbits(y),flipbits(z));
}
void GeometryOctree::manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    GeometryOctreeLeaf* newguy = new GeometryOctreeLeaf(x,y,z,0);
    expand(x,y,z);
    data->insertinto(flipbits(x),flipbits(y),flipbits(z),depth,newguy,data);
//    newguy->geometry.material = 2;
//    newguy->geometry.matrix = &matrix;
    uint8_t gtable[2][6][2] = {
        {{0,0},{1,1},{1,0},{1,1},{0,0},{0,1}},
        {{1,1},{0,0},{1,0},{0,0},{1,1},{0,1}}
    };
    extern uint8_t materialprops[];
    for (int xi=0;xi<GEOSIZE-1;xi++) {
        for (int yi=0;yi<GEOSIZE-1;yi++) {
            for (int zi=0;zi<GEOSIZE-1;zi++) {
                BlockLoc xt = xi+ (x<<GEOPOWER);
                BlockLoc yt = yi+ (y<<GEOPOWER);
                BlockLoc zt = zi+ (z<<GEOPOWER);
                uint8_t an = voxes.getAt(xt,yt,zt);
                uint8_t anx = voxes.getAt(xt+1,yt,zt);
                uint8_t any = voxes.getAt(xt,yt+1,zt);
                uint8_t anz = voxes.getAt(xt,yt,zt+1);
                if (materialprops[an]>materialprops[anx]) {
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[an].addVert(glm::vec3(xt,yt-gtable[1][ik][0],zt-gtable[1][ik][1])+
                                                    voxes.feat(xt,yt-gtable[1][ik][0],zt-gtable[1][ik][1]).get());
                    }
                } else if (materialprops[an]<materialprops[anx]) {
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[anx].addVert(glm::vec3(xt,yt-gtable[0][ik][0],zt-gtable[0][ik][1])+
                                                     voxes.feat(xt,yt-gtable[0][ik][0],zt-gtable[0][ik][1]).get());
                    }
                }
                if (materialprops[an]>materialprops[any]) {
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[an].addVert(glm::vec3(xt-gtable[0][ik][0],yt,zt-gtable[0][ik][1])+
                                                    voxes.feat(xt-gtable[0][ik][0],yt,zt-gtable[0][ik][1]).get());
                    }
                } else if (materialprops[an]<materialprops[any]) {
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[any].addVert(glm::vec3(xt-gtable[1][ik][0],yt,zt-gtable[1][ik][1])+
                                                     voxes.feat(xt-gtable[1][ik][0],yt,zt-gtable[1][ik][1]).get());
                    }
                }
                if (materialprops[an]>materialprops[anz]) {
//                    std::cout<<"made a face. z";
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[an].addVert(glm::vec3(xt-gtable[1][ik][0],yt-gtable[1][ik][1],zt)+
                                                    voxes.feat(xt-gtable[1][ik][0],yt-gtable[1][ik][1],zt).get());
                    }
                } else if (materialprops[an]<materialprops[anz]) {
                    for (int ik=0;ik<6;ik++) {
                        newguy->geometry[anz].addVert(glm::vec3(xt-gtable[0][ik][0],yt-gtable[0][ik][1],zt)+
                                                     voxes.feat(xt-gtable[0][ik][0],yt-gtable[0][ik][1],zt).get());
                    }
                }
            }
        }
    }
    
    for(auto iterator = newguy->geometry.begin(); iterator != newguy->geometry.end(); iterator++) {
        // iterator->first = key
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
//        iterator->second.material = iterator->first;
        iterator->second.matrix = &matrix;
        std::cout<<"created geometry with material "<<(int)iterator->first<<"\n";
    }
    std::cout<<"finished generation.\n";
}

GeometryOctree::GeometryOctree(glm::mat4& matr,Octree& vox) : matrix(matr),voxes(vox) {}

