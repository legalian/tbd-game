//
//  GeometryOctree.cpp
//  Total Control
//
//  Created by Parker on 3/19/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "geometryoctree.h"

int rv[8][3] = {{0,0,0},{1,0,0},{0,1,0},{1,1,0},
                {0,0,1},{1,0,1},{0,1,1},{1,1,1}};

void GeometryOctreeSegment::render(const glm::mat4& matr) {throw;}
void GeometryOctreeSegment::debugprinttrace(int offset) {
    for (int nu=0;nu<offset;nu++) {
        std::cout<<"  ";
    }
    std::cout<<recurs<<"\n";
}
void GeometryOctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self) {throw;}
bool GeometryOctreeSegment::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
GeometryOctreeLeaf* GeometryOctreeSegment::getgeomat(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
GeometryOctreeSegment::GeometryOctreeSegment(BlockLoc i,BlockLoc j,BlockLoc k,int rec) : xs(i),ys(j),zs(k),recurs(rec) {}
bool GeometryOctreeSegment::checkbounds(const glm::mat4& matr) {
    if (recurs>3) {return true;}
    bool hitplanes[6] = {false,false,false,false,false,false};
    extern glm::mat4 camera;
    glm::vec4 thisvert;
    
    for (int iv=0;iv<8;iv++) {
        thisvert = glm::vec4((xs + (rv[iv][0]<<recurs))<<CHPOWER,
                             (ys + (rv[iv][1]<<recurs))<<CHPOWER,
                             (zs + (rv[iv][2]<<recurs))<<CHPOWER,1);
        thisvert = (camera)*(matr)*thisvert;

        if (thisvert.x/thisvert.w<1) hitplanes[0]=true;
        if (thisvert.x/thisvert.w>-1) hitplanes[1]=true;
        if (thisvert.y/thisvert.w<1) hitplanes[2]=true;
        if (thisvert.y/thisvert.w>-1) hitplanes[3]=true;
        if (thisvert.z/thisvert.w<1) hitplanes[4]=true;
        if (thisvert.z/thisvert.w>-1) hitplanes[5]=true;
    }


//            (shader->cam)*(*it->second.matrix)*


    //it->first.x*CHSIZE,(it->first.x+1)*CHSIZE
    //it->first.y*CHSIZE,(it->first.y+1)*CHSIZE
    //it->first.z*CHSIZE,(it->first.z+1)*CHSIZE
    if (hitplanes[0] and hitplanes[1] and hitplanes[2] and hitplanes[3] and hitplanes[4] and hitplanes[5]) {
        return true;
    }
    return false;
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
void GeometryOctreeBranch::debugprinttrace(int offset) {
    for (int nu=0;nu<offset;nu++) {
        std::cout<<"  ";
    }
    std::cout<<recurs<<"\n";
    subdivisions[0][0][0]->debugprinttrace(offset+1);
    subdivisions[1][0][0]->debugprinttrace(offset+1);
    subdivisions[0][1][0]->debugprinttrace(offset+1);
    subdivisions[1][1][0]->debugprinttrace(offset+1);
    subdivisions[0][0][1]->debugprinttrace(offset+1);
    subdivisions[1][0][1]->debugprinttrace(offset+1);
    subdivisions[0][1][1]->debugprinttrace(offset+1);
    subdivisions[1][1][1]->debugprinttrace(offset+1);
}

GeometryOctreeLeaf* GeometryOctreeBranch::getgeomat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions[x&1][y&1][z&1]->getgeomat(x>>1,y>>1,z>>1);
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
    
    if (checkbounds(matr) and !open) {
        
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
bool GeometryOctreeLeaf::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {return true;}
GeometryOctreeLeaf* GeometryOctreeLeaf::getgeomat(BlockLoc x,BlockLoc y,BlockLoc z) {return this;}

GeometryOctreeBud::GeometryOctreeBud(BlockLoc i,BlockLoc j,BlockLoc k,int rec) : GeometryOctreeSegment(i,j,k,rec) {}

void GeometryOctreeBud::render(const glm::mat4& matr) {}
void GeometryOctreeBud::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,GeometryOctreeSegment* toinsert,GeometryOctreeSegment*& self){
    if (recur==0) {
        delete self;
        self = toinsert;
    } else {
        BlockLoc add = 1<<(recurs-1);
        int pa = recurs-1;
        GeometryOctreeSegment* todelete = self;
        self = new GeometryOctreeBranch(new GeometryOctreeBud(xs,ys,zs,pa)        ,new GeometryOctreeBud(xs+add,ys,zs,pa),
                                        new GeometryOctreeBud(xs,ys+add,zs,pa)    ,new GeometryOctreeBud(xs+add,ys+add,zs,pa),
                                        new GeometryOctreeBud(xs,ys,zs+add,pa)    ,new GeometryOctreeBud(xs+add,ys,zs+add,pa),
                                        new GeometryOctreeBud(xs,ys+add,zs+add,pa),new GeometryOctreeBud(xs+add,ys+add,zs+add,pa)
                                        ,xs,ys,zs,pa+1);
        delete todelete;
        self->insertinto(x,y,z,recur,toinsert,self);
    }
}
bool GeometryOctreeBud::existsat(BlockLoc x, BlockLoc y, BlockLoc z) {
    return false;
}
//bool GeometryOctreePlaceholder::existsat(BlockLoc x, BlockLoc y, BlockLoc z) {
//    return true;
//}

void GeometryOctree::expand(BlockLoc x,BlockLoc y,BlockLoc z) {
    int desireddepth = underpressure(x,y,z);
    while (desireddepth>depth) {
        depth++;
        
        if (depth&1) {
            reduce -= (1<<(depth-1));
            BlockLoc pos = reduce;
            BlockLoc mid = pos + (1<<(depth-1));
            data = new GeometryOctreeBranch(new GeometryOctreeBud(pos,pos,pos,depth-1),new GeometryOctreeBud(mid,pos,pos,depth-1),
                                            new GeometryOctreeBud(pos,mid,pos,depth-1),new GeometryOctreeBud(mid,mid,pos,depth-1),
                                            new GeometryOctreeBud(pos,pos,mid,depth-1),new GeometryOctreeBud(mid,pos,mid,depth-1),
                                            new GeometryOctreeBud(pos,mid,mid,depth-1),data,pos,pos,pos,depth);
            
        } else {
            BlockLoc pos = reduce;
            BlockLoc mid = pos + (1<<(depth-1));
            data = new GeometryOctreeBranch(data,new GeometryOctreeBud(mid,pos,pos,depth-1),
                                            new GeometryOctreeBud(pos,mid,pos,depth-1),new GeometryOctreeBud(mid,mid,pos,depth-1),
                                            new GeometryOctreeBud(pos,pos,mid,depth-1),new GeometryOctreeBud(mid,pos,mid,depth-1),
                                            new GeometryOctreeBud(pos,mid,mid,depth-1),new GeometryOctreeBud(mid,mid,mid,depth-1),
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
GeometryOctreeLeaf* GeometryOctree::getgeomat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->getgeomat(flipbits(x),flipbits(y),flipbits(z));
}
bool GeometryOctree::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (underpressure(x,y,z)>depth) {return false;}
    return data->existsat(flipbits(x),flipbits(y),flipbits(z));
}
void GeometryOctree::insertinto(BlockLoc x, BlockLoc y, BlockLoc z, GeometryOctreeLeaf *newguy) {
    data->insertinto(flipbits(x),flipbits(y),flipbits(z),depth,newguy,data);
}
void GeometryOctreeLeaf::matrixmap(glm::mat4 *matr) {
    for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
        iterator->second.matrix = matr;
    }
}
void GeometryOctreeLeaf::preparesnippets() {
    for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
        iterator->second.dumpextrabits();
    }
}
void GeometryOctreeLeaf::securecore() {
    for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
        iterator->second.markcoregeom();
    }
}
//void GeometryOctree::manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
//    GeometryOctreeLeaf* newguy = new GeometryOctreeLeaf(x,y,z,0);
//    expand(x,y,z);
//    data->insertinto(flipbits(x),flipbits(y),flipbits(z),depth,newguy,data);
////    newguy->geometry.material = 2;
////    newguy->geometry.matrix = &matrix;
//    extern uint8_t materialprops[];
//    for (int xi=0;xi<GEOSIZE-1;xi++) {
//        for (int yi=0;yi<GEOSIZE-1;yi++) {
//            for (int zi=0;zi<GEOSIZE-1;zi++) {
//                BlockLoc xo = xi+ (x<<GEOPOWER);
//                BlockLoc yo = yi+ (y<<GEOPOWER);
//                BlockLoc zo = zi+ (z<<GEOPOWER);
//                BlockLoc xt = xo ASBLOCKLOC;
//                BlockLoc yt = yo ASBLOCKLOC;
//                BlockLoc zt = zo ASBLOCKLOC;
//                uint8_t an = voxes.data->getser(xt,yt,zt);
//                uint8_t anx = voxes.data->getser(xt+1,yt,zt);
//                uint8_t any = voxes.data->getser(xt,yt+1,zt);
//                uint8_t anz = voxes.data->getser(xt,yt,zt+1);
//                if (materialprops[an]>materialprops[anx]) {
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[an].addVert(glm::vec3(xo,yo-gtable[1][ik][0],zo-gtable[1][ik][1])+
//                                              voxes.data->feat(xt,yt-gtable[1][ik][0],zt-gtable[1][ik][1]).get());
//                    }
//                } else if (materialprops[an]<materialprops[anx]) {
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[anx].addVert(glm::vec3(xo,yo-gtable[0][ik][0],zo-gtable[0][ik][1])+
//                                               voxes.data->feat(xt,yt-gtable[0][ik][0],zt-gtable[0][ik][1]).get());
//                    }
//                }
//                if (materialprops[an]>materialprops[any]) {
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[an].addVert(glm::vec3(xo-gtable[0][ik][0],yo,zo-gtable[0][ik][1])+
//                                              voxes.data->feat(xt-gtable[0][ik][0],yt,zt-gtable[0][ik][1]).get());
//                    }
//                } else if (materialprops[an]<materialprops[any]) {
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[any].addVert(glm::vec3(xo-gtable[1][ik][0],yo,zo-gtable[1][ik][1])+
//                                               voxes.data->feat(xt-gtable[1][ik][0],yt,zt-gtable[1][ik][1]).get());
//                    }
//                }
//                if (materialprops[an]>materialprops[anz]) {
////                    std::cout<<"made a face. z";
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[an].addVert(glm::vec3(xo-gtable[1][ik][0],yo-gtable[1][ik][1],zo)+
//                                              voxes.data->feat(xt-gtable[1][ik][0],yt-gtable[1][ik][1],zt).get());
//                    }
//                } else if (materialprops[an]<materialprops[anz]) {
//                    for (int ik=0;ik<6;ik++) {
//                        newguy->geometry[anz].addVert(glm::vec3(xo-gtable[0][ik][0],yo-gtable[0][ik][1],zo)+
//                                               voxes.data->feat(xt-gtable[0][ik][0],yt-gtable[0][ik][1],zt).get());
//                    }
//                }
//            }
//        }
//    }
//    
//    for(auto iterator = newguy->geometry.begin(); iterator != newguy->geometry.end(); iterator++) {
//        // iterator->first = key
//        // iterator->second = value
//        // Repeat if you also want to iterate through the second map.
////        iterator->second.material = iterator->first;
//        iterator->second.matrix = &matrix;
//        std::cout<<"created geometry with material "<<(int)iterator->first<<"\n";
//    }
//    std::cout<<"finished generation.\n";
//}

GeometryOctree::GeometryOctree(glm::mat4& matr) : matrix(matr) {}

