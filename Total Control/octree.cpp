//
//  octree.cpp
//  Total Control
//
//  Created by Parker Lawrence on 2/29/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"

#define XYZINDEX [(x&(1<<depth))>>depth][(y&(1<<depth))>>depth][(z&(1<<depth))>>depth]
//#define XYZINDEX [x&(1<<depth)?1:0][y&(1<<depth)?1:0][z&(1<<depth)?1:0]



uint8_t gtable[2][6][2] = {
    {{0,0},{1,1},{1,0},{1,1},{0,0},{0,1}},
    {{1,1},{0,0},{1,0},{0,0},{1,1},{0,1}}
};

Edgedat::Edgedat() {
    
}
Edgedat::Edgedat(uint8_t tc,uint8_t xc,uint8_t yc,uint8_t zc) : x(xc),y(yc),z(zc),t(tc) {}
Edgedat::Edgedat(float xi,float yi,float zi,float ti) {
    x = (uint8_t)255*xi;
    y = (uint8_t)255*yi;
    z = (uint8_t)255*zi;
    t = (uint8_t)255*ti;
}
glm::vec3 Edgedat::getnorm() {
    return glm::vec3(x/255.0,y/255.0,z/255.0);
}
float Edgedat::getoffset() {
    return t/255.0;
}
glm::vec3 Feature::get() {
    return glm::vec3(x/255.0,y/255.0,z/255.0);
}
void Feature::operator=(const glm::vec3& other) {
    x = other.x*255;
    y = other.y*255;
    z = other.z*255;
}

MatrixCarriage operator+(const MatrixCarriage &m1, const MatrixCarriage &m2)
{
    // use the Cents constructor and operator+(int, int)
    // we can access m_cents directly because this is a friend function
    return MatrixCarriage(
                          m1.bb+m2.bb,
                          m1.bb+m2.Ab1,
                          m1.bb+m2.Ab2,
                          m1.bb+m2.Ab3,
                          m1.bb+m2.AA1,
                          m1.bb+m2.AA2,
                          m1.bb+m2.AA3,
                          m1.bb+m2.AA4,
                          m1.bb+m2.AA5,
                          m1.bb+m2.AA6
    );
}

MatrixCarriage::MatrixCarriage() :
    bb(0), Ab1(0), Ab2(0), Ab3(0), AA1(0), AA2(0), AA3(0), AA4(0), AA5(0), AA6(0) {};
MatrixCarriage::MatrixCarriage(double a,double b,double c,double d,double e,double f,double g,double h,double i,double j) :
    bb(a), Ab1(b), Ab2(c), Ab3(d), AA1(e), AA2(f), AA3(g), AA4(h), AA5(i), AA6(j) {};



void OctreeSegment::geomifyselection(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {}
void OctreeSegment::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
void OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {}
//void OctreeSegment::geomifyportion(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {}
void OctreeSegment::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
OctreeSegment* OctreeSegment::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {return this;}
//void OctreeSegment::hermitifyportion(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
BlockId OctreeSegment::getser(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
void OctreeSegment::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {throw;}
void OctreeSegment::collapse(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*& self) {}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeSegment::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Feature& OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
void OctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
void OctreeSegment::filesave(std::ostream& file) {throw;}
BlockId OctreeSegment::getsimpid() {throw;}
bool OctreeSegment::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}

OctreeFeature::OctreeFeature(BlockId fill) : fillvalue(fill) {}
BlockId OctreeFeature::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
void OctreeFeature::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {fillvalue = newid;}
Feature& OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z)  {return point;}
BlockId OctreeFeature::getsimpid() {return 0;}
void OctreeFeature::filesave(std::ostream& file) {
    char towrite = 'f';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
bool OctreeFeature::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return true;}

void OctreeFeature::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    int size = 0;
    glm::vec3 points[12];
    glm::vec3 normals[12];
    BlockId buf[2][2][2];
    for (int xl=0;xl<2;xl++) {
        for (int yl=0;yl<2;yl++) {
            for (int zl=0;zl<2;zl++) {
                buf[xl][yl][zl] = world->getser(x+xl,y+yl,z+zl);
            }
        }
    }
    for (int xl=0;xl<2;xl++) {
        for (int yl=0;yl<2;yl++) {
            if (buf[xl][yl][0]!=buf[xl][yl][1]) {
                Edgedat& koka = world->zcon(x+xl,y+yl,z);
                points[size] = glm::vec3(xl,yl,koka.getoffset());
                normals[size] = koka.getnorm();
                size++;
            }
        }
    }
    for (int xl=0;xl<2;xl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[xl][0][zl]!=buf[xl][1][zl]) {
                Edgedat& koka = world->ycon(x+xl,y,z+zl);
                points[size] = glm::vec3(xl,koka.getoffset(),zl);
                normals[size] = koka.getnorm();
                size++;
            }
        }
    }
    for (int yl=0;yl<2;yl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[0][yl][zl]!=buf[1][yl][zl]) {
                Edgedat& koka = world->xcon(x,y+yl,z+zl);
                points[size] = glm::vec3(koka.getoffset(),yl,zl);
                normals[size] = koka.getnorm();
                size++;
            }
        }
    }
    point = omgqef(normals, points, size);
}
void OctreeFeature::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) ) {
        hermitifyselection(x, y, z, world);
    }
}

OctreeLeaf::OctreeLeaf(BlockId fill) : OctreeFeature(fill) {}
OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t n[12]) : OctreeFeature(fill),
    xcondat(n[0],n[1],n[2],n[3]),
    ycondat(n[4],n[5],n[6],n[7]),
    zcondat(n[8],n[9],n[10],n[11])  {}
//bool OctreeLeaf::uniqueat(long x,long y,long z,int recur) {return true;}
void OctreeLeaf::geomifyselection(BlockLoc x, BlockLoc y, BlockLoc z,GeometryOctreeLeaf* geometry, OctreeSegment *world) {
    extern uint8_t gtable[2][6][2];
    extern uint8_t materialprops[];
    uint8_t anx = world->getser(x+1,y,z);
    uint8_t any = world->getser(x,y+1,z);
    uint8_t anz = world->getser(x,y,z+1);
    BlockLoc xo = x - (ASBLOCKLOC(0));
    BlockLoc yo = y - (ASBLOCKLOC(0));
    BlockLoc zo = z - (ASBLOCKLOC(0));
    if (materialprops[fillvalue]>materialprops[anx]) {
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[fillvalue].addVert(glm::vec3(xo,yo-gtable[1][ik][0],zo-gtable[1][ik][1])+
                                                world->feat(x,y-gtable[1][ik][0],z-gtable[1][ik][1]).get());
        }
    } else if (materialprops[fillvalue]<materialprops[anx]) {
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[anx].addVert(glm::vec3(xo,yo-gtable[0][ik][0],zo-gtable[0][ik][1])+
                                          world->feat(x,y-gtable[0][ik][0],z-gtable[0][ik][1]).get());
        }
    }
    if (materialprops[fillvalue]>materialprops[any]) {
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[fillvalue].addVert(glm::vec3(xo-gtable[0][ik][0],yo,zo-gtable[0][ik][1])+
                                                world->feat(x-gtable[0][ik][0],y,z-gtable[0][ik][1]).get());
        }
    } else if (materialprops[fillvalue]<materialprops[any]) {
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[any].addVert(glm::vec3(xo-gtable[1][ik][0],yo,zo-gtable[1][ik][1])+
                                          world->feat(x-gtable[1][ik][0],y,z-gtable[1][ik][1]).get());
        }
    }
    if (materialprops[fillvalue]>materialprops[anz]) {
        //                    std::cout<<"made a face. z";
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[fillvalue].addVert(glm::vec3(xo-gtable[1][ik][0],yo-gtable[1][ik][1],zo)+
                                                world->feat(x-gtable[1][ik][0],y-gtable[1][ik][1],z).get());
        }
    } else if (materialprops[fillvalue]<materialprops[anz]) {
        for (int ik=0;ik<6;ik++) {
            geometry->geometry[anz].addVert(glm::vec3(xo-gtable[0][ik][0],yo-gtable[0][ik][1],zo)+
                                          world->feat(x-gtable[0][ik][0],y-gtable[0][ik][1],z).get());
        }
    }
    
}
void OctreeLeaf::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {
    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) and
         ((x&(CHSIZE-1)) != 0       ) and ((y&(CHSIZE-1)) != 0       ) and ((z&(CHSIZE-1)) != 0       ) ) {
        geomifyselection(x,y,z,geometry,world);
    }
}
Edgedat& OctreeLeaf::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return zcondat;}
void OctreeLeaf::filesave(std::ostream& file) {
    char towrite = 'l';
    file.write(&towrite,1);
    file.write((char*)&fillvalue,sizeof(uint8_t));
    file.write((char*)&xcondat.t,sizeof(uint8_t));
    file.write((char*)&xcondat.x,sizeof(uint8_t));
    file.write((char*)&xcondat.y,sizeof(uint8_t));
    file.write((char*)&xcondat.z,sizeof(uint8_t));
    file.write((char*)&ycondat.t,sizeof(uint8_t));
    file.write((char*)&ycondat.x,sizeof(uint8_t));
    file.write((char*)&ycondat.y,sizeof(uint8_t));
    file.write((char*)&ycondat.z,sizeof(uint8_t));
    file.write((char*)&zcondat.t,sizeof(uint8_t));
    file.write((char*)&zcondat.x,sizeof(uint8_t));
    file.write((char*)&zcondat.y,sizeof(uint8_t));
    file.write((char*)&zcondat.z,sizeof(uint8_t));
}

OctreeBud::OctreeBud(BlockId fill) : fillvalue(fill) {}
BlockId OctreeBud::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
void OctreeBud::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {
    if (newid == fillvalue) {
    } else if (recur==0) {
        fillvalue = newid;
    }
    else {
        delete self;
        self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),recur-1);
        self->setser(x,y,z,newid,recur,self);
    }
}
//bool OctreeBud::uniqueat(long x,long y,long z,int recur) {throw;}
void OctreeBud::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==0) {
        delete self;
        self = toinsert;
    } else {
        delete self;
        self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),recur+CHPOWER-1);
        self->insertinto(x,y,z,recur,toinsert,self);
    }
}
BlockId OctreeBud::getsimpid() {return fillvalue;}
void OctreeBud::filesave(std::ostream& file) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
bool OctreeBud::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return false;}

OctreeBranch::OctreeBranch(OctreeSegment* a,OctreeSegment* b,
                           OctreeSegment* c,OctreeSegment* d,
                           OctreeSegment* e,OctreeSegment* f,
                           OctreeSegment* g,OctreeSegment* h, int dep) : depth(dep) {
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = b;
    subdivisions[0][1][0] = c;
    subdivisions[1][1][0] = d;
    subdivisions[0][0][1] = e;
    subdivisions[1][0][1] = f;
    subdivisions[0][1][1] = g;
    subdivisions[1][1][1] = h;
}

void OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {
    BlockLoc mask = 1<<depth;
    subdivisions[0][0][0]->geomify(x     ,y     ,z     ,geometry,world);
    subdivisions[1][0][0]->geomify(x|mask,y     ,z     ,geometry,world);
    subdivisions[0][1][0]->geomify(x     ,y|mask,z     ,geometry,world);
    subdivisions[1][1][0]->geomify(x|mask,y|mask,z     ,geometry,world);
    subdivisions[0][0][1]->geomify(x     ,y     ,z|mask,geometry,world);
    subdivisions[1][0][1]->geomify(x|mask,y     ,z|mask,geometry,world);
    subdivisions[0][1][1]->geomify(x     ,y|mask,z|mask,geometry,world);
    subdivisions[1][1][1]->geomify(x|mask,y|mask,z|mask,geometry,world);
}
void OctreeBranch::geomifyselection(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world) {
    subdivisions XYZINDEX->geomifyselection(x,y,z,geometry,world);
}
void OctreeBranch::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    subdivisions XYZINDEX->hermitifyselection(x,y,z,world);
}
void OctreeBranch::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    BlockLoc mask = 1<<depth;
    subdivisions[0][0][0]->hermitify(x     ,y     ,z     ,world);
    subdivisions[1][0][0]->hermitify(x|mask,y     ,z     ,world);
    subdivisions[0][1][0]->hermitify(x     ,y|mask,z     ,world);
    subdivisions[1][1][0]->hermitify(x|mask,y|mask,z     ,world);
    subdivisions[0][0][1]->hermitify(x     ,y     ,z|mask,world);
    subdivisions[1][0][1]->hermitify(x|mask,y     ,z|mask,world);
    subdivisions[0][1][1]->hermitify(x     ,y|mask,z|mask,world);
    subdivisions[1][1][1]->hermitify(x|mask,y|mask,z|mask,world);
}
OctreeSegment* OctreeBranch::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (depth<CHPOWER) {
        return this;
    } else {
        return subdivisions XYZINDEX->getvoxunit(x,y,z);
    }
}

BlockId OctreeBranch::getser(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getser(x,y,z);
}
void OctreeBranch::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {
    subdivisions XYZINDEX->setser(x,y,z,newid,recur-1,subdivisions XYZINDEX);
}
//void OctreeBranch::collapse(long x,long y,long z,OctreeSegment*& self) {
//    subdivisions[x&1][y&1][z&1]->collapse(x>>1,y>>1,z>>1,subdivisions[x&1][y&1][z&1]);
//}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeBranch::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->xcon(x,y,z);
}
Edgedat& OctreeBranch::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->ycon(x,y,z);
}
Edgedat& OctreeBranch::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->zcon(x,y,z);
}
Feature& OctreeBranch::feat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->feat(x,y,z);
}
void OctreeBranch::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==0) {
        delete self;//possible memory leak; todo:make branches delete their children in deconstructors
        self = toinsert;
    } else {
        subdivisions XYZINDEX->insertinto(x,y,z,recur-1,toinsert,subdivisions XYZINDEX);
    }
}
void OctreeBranch::filesave(std::ostream& file) {
    char towrite = 'c';
    file.write(&towrite,1);
    subdivisions[0][0][0]->filesave(file);
    subdivisions[1][0][0]->filesave(file);
    subdivisions[0][1][0]->filesave(file);
    subdivisions[1][1][0]->filesave(file);
    subdivisions[0][0][1]->filesave(file);
    subdivisions[1][0][1]->filesave(file);
    subdivisions[0][1][1]->filesave(file);
    subdivisions[1][1][1]->filesave(file);
}
BlockId OctreeBranch::getsimpid() {return 0;}
bool OctreeBranch::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->featuresque(x,y,z);
}

void Octree::expand(BlockLoc x,BlockLoc y,BlockLoc z) {expand(underpressure(x,y,z));}
void Octree::expandchunk(BlockLoc x,BlockLoc y,BlockLoc z) {
    expand(std::max(underpressure(x<<CHPOWER,y<<CHPOWER,z<<CHPOWER),
                    underpressure(((x+1)<<CHPOWER)-1,((y+1)<<CHPOWER)-1,((z+1)<<CHPOWER)-1 )));
    
}
void Octree::expand(int desireddepth) {
//    int desireddepth = ;//+bias;
    while (desireddepth>depth) {
        depth++;
        if (depth&1) {
            data = new OctreeBranch(new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),data,depth-1);
        } else {
            data = new OctreeBranch(data,new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),depth-1);
        }
    }
}
int Octree::underpressure(BlockLoc x,BlockLoc y,BlockLoc z) {
    //    std::cout<<((ALTERNATOR+z)^ALTERNATOR)<<"\n";
    x -= ALTERNATOR&(CHSIZE-1);
    y -= ALTERNATOR&(CHSIZE-1);
    z -= ALTERNATOR&(CHSIZE-1);
    x = (x+ALTERNATOR)^ALTERNATOR;
    y = (y+ALTERNATOR)^ALTERNATOR;
    z = (z+ALTERNATOR)^ALTERNATOR;
//    long mo = std::max(std::max(x,y),z);
    int r;
    for (r=0;(1<<r)<=x or (1<<r)<=y or (1<<r)<=z;r++) {}
    return r;
}

OctreeSegment* makeOctree(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 'l') {
        uint8_t id;
        uint8_t ids[12];
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &ids,sizeof(uint8_t)*12);
        return new OctreeLeaf(id,ids);
    } else if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'f') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeFeature(id);
    } else if (tester == 'c') {
        recur--;
        return new OctreeBranch(makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),
                                makeOctree(file,recur),recur);
    } else {
        throw;
    }
}


OctreeSegment* makeOctree(BlockId (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
    if (recur == 0) {
        uint8_t equi = data[x][y][z];
        
        if (equi==data[x+1][y][z] and equi==data[x][y+1][z] and equi==data[x][y][z+1]) {
            if (equi==data[x+1][y+1][z] and equi==data[x+1][y][z+1] and equi==data[x][y+1][z+1] and equi==data[x+1][y+1][z+1]) {
                return new OctreeBud(data[x][y][z]);
            } else {
                return new OctreeFeature(data[x][y][z]);
            }
        } else {
            return new OctreeLeaf(data[x][y][z]);
        }
    }
    BlockLoc add = (1<<recur)+1;
    BlockId equi = data[x][y][z];
    for (BlockLoc xi=x;xi<x+add;xi++) {
        for (BlockLoc yi=y;yi<y+add;yi++) {
            for (BlockLoc zi=z;zi<z+add;zi++) {
                if (data[xi][yi][zi] != equi) {
                    int k = (1<<(recur-1));
                    OctreeSegment* a = makeOctree(data,x,y,z,recur-1);
                    OctreeSegment* b = makeOctree(data,x+k,y,z,recur-1);
                    OctreeSegment* c = makeOctree(data,x,y+k,z,recur-1);
                    OctreeSegment* d = makeOctree(data,x+k,y+k,z,recur-1);
                    OctreeSegment* e = makeOctree(data,x,y,z+k,recur-1);
                    OctreeSegment* f = makeOctree(data,x+k,y,z+k,recur-1);
                    OctreeSegment* g = makeOctree(data,x,y+k,z+k,recur-1);
                    OctreeSegment* h = makeOctree(data,x+k,y+k,z+k,recur-1);
                    return new OctreeBranch(a,b,c,d,e,f,g,h,recur-1);
                }
            }
        }
    }
    return new OctreeBud(equi);
}

void Octree::loadportion(BlockLoc x,BlockLoc y,BlockLoc z,BlockId (*dat)[CHSIZE+1][CHSIZE+1]) {
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),depth-CHPOWER,makeOctree(dat,0,0,0,CHPOWER),data);
}


Octree::Octree(glm::mat4& trans) : realworld(trans) {}

void Octree::hermitify(BlockLoc x, BlockLoc y, BlockLoc z) {
    data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))->hermitify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),data);
    
}
void Octree::filepushportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    //    file =
    std::ofstream file = std::ofstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::out|std::ios::binary|std::ios::trunc);
    data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))->filesave(file);
    file.close();
}
void Octree::filepullportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    //    file =
    std::ifstream file = std::ifstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::in|std::ios::binary);
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),depth-CHPOWER,makeOctree(file,CHPOWER),data);
    file.close();
}
bool Octree::dataexists(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    return boost::filesystem::exists(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)));
}
glm::vec3 omgqef(glm::vec3 normals[], glm::vec3 positions[], int length )
{
    glm::vec3 ret;
    for (int k=0;k<length;k++) {
        ret += positions[k];
    }
    return ret/(float)length;
}

void Octree::render() {
    realworld.render();
}
bool Octree::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return realworld.existsat(x,y,z);
}
void Octree::manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    GeometryOctreeLeaf* newguy = new GeometryOctreeLeaf(x,y,z,0);
    
    data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))->geomify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),newguy,data);
    
    realworld.expand(x,y,z);
    realworld.insertinto(x,y,z,newguy);
//    realworld.data->insertinto(realworld.flipbits(x),realworld.flipbits(y),realworld.flipbits(z),realworld.depth,newguy,realworld.data);
    newguy->matrixmap(&realworld.matrix);
    newguy->securecore();
    newguy->open = false;
//    for(auto iterator = newguy->geometry.begin(); iterator != newguy->geometry.end(); iterator++) {
//        iterator->second.matrix = &realworld.matrix;
//        std::cout<<"created geometry with material "<<(int)iterator->first<<"\n";
//    }
//    std::cout<<"finished generation.\n";

}
//
void Octree::voxXskirt(BlockLoc x,BlockLoc y,BlockLoc z) {
    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
            data->hermitifyselection(ASCHUNKLOC(x+1)-1,yi,zi,data);
        }
    }
    GeometryOctreeLeaf* geo = realworld.getgeomat(x,y,z);
    geo->open = true;
    geo->preparesnippets();
    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
            data->geomifyselection(ASCHUNKLOC(x+1)-1,yi,zi,geo,data);
            data->geomifyselection(ASCHUNKLOC(x+1),yi,zi,geo,data);
        }
    }
    geo->matrixmap(&realworld.matrix);
    geo->open = false;
}
void Octree::voxYskirt(BlockLoc x,BlockLoc y,BlockLoc z) {
    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
            data->hermitifyselection(xi,ASCHUNKLOC(y+1)-1,zi,data);
        }
    }
    GeometryOctreeLeaf* geo = realworld.getgeomat(x,y,z);
    geo->open = true;
    geo->preparesnippets();
    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
            data->geomifyselection(xi,ASCHUNKLOC(y+1)-1,zi,geo,data);
            data->geomifyselection(xi,ASCHUNKLOC(y+1),zi,geo,data);
        }
    }
    geo->matrixmap(&realworld.matrix);
    geo->open = false;
}

void Octree::voxsnippets(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (existsat(x-1, y, z)) {
        voxXskirt(x-1, y, z);
    }
    if (existsat(x+1, y, z)) {
        voxXskirt(x, y, z);
    }
    if (existsat(x, y-1, z)) {
        voxYskirt(x, y-1, z);
    }
    if (existsat(x, y+1, z)) {
        voxYskirt(x, y, z);
    }
//    if (existsat(x, y, z-1)) {
//        voxZskirt(x, y, z-1);
//    }
//    if (existsat(x, y, z+1)) {
//        voxZskirt(x, y, z);
//    }
//    for (int yo1=-1;yo1<=0;yo1++) {
//        for (int zo1=-1;zo1<=0;zo1++) {
//            bool count = true;
//            for (int yo2=0;yo2<=1;yo2++) {
//                for (int zo2=0;zo2<=1;zo2++) {
//                    if (!existsat(x,y+yo1+yo2,z+zo1+zo2)) {
//                        count = false;
//                    }
//                }
//            }
//            if (count) {
//                voxXrow(x,y+yo1,z+zo1);
//            }
//        }
//    }
//    for (int xo1=-1;xo1<=0;xo1++) {
//        for (int zo1=-1;zo1<=0;zo1++) {
//            bool count = true;
//            for (int xo2=0;xo2<=1;xo2++) {
//                for (int zo2=0;zo2<=1;zo2++) {
//                    if (!existsat(x+xo1+xo2,y,z+zo1+zo2)) {
//                        count = false;
//                    }
//                }
//            }
//            if (count) {
//                voxYrow(x+xo1,y,z+zo1);
//            }
//        }
//    }
//    for (int xo1=-1;xo1<=0;xo1++) {
//        for (int yo1=-1;yo1<=0;yo1++) {
//            bool count = true;
//            for (int xo2=0;xo2<=1;xo2++) {
//                for (int yo2=0;yo2<=1;yo2++) {
//                    if (!existsat(x+xo1+xo2,y+yo1+yo2,z)) {
//                        count = false;
//                    }
//                }
//            }
//            if (count) {
//                voxZrow(x+xo1,y+yo1,z);
//            }
//        }
//    }
//    for (int xo1=-1;xo1<=0;xo1++) {
//        for (int yo1=-1;yo1<=0;yo1++) {
//            for (int zo1=-1;zo1<=0;zo1++) {
//                bool count = true;
//                for (int xo2=0;xo2<=1;xo2++) {
//                    for (int yo2=0;yo2<=1;yo2++) {
//                        for (int zo2=0;zo2<=1;zo2++) {
//                            if (!existsAt(x+xo1+xo2,y+yo1+yo2,z+zo1+zo2)) {
//                                count = false;
//                            }
//                        }
//                    }
//                }
//                if (count) {
//                    voxXYZcorner(x+xo1,y+yo1,z+zo1);
//                }
//            }
//        }
//    }

//    for (int ix=0;ix<2;ix++) {
//        for (int iy=0;iy<2;iy++) {
//            for (int iz=0;iz<2;iz++) {
//                if (!existsat(x+ix,y+iy,z+iz)) {
//                    return;
//                }
//            }
//        }
//    }
////    for (int xi=ASCHUNKLOC(x);xi<ASCHUNKLOC(x+1)-1;xi++) {
////        for (int yi=ASCHUNKLOC(x);yi<ASCHUNKLOC(y+1)-1;yi++) {
////            data->hermitifyselection(xi, yi,ASCHUNKLOC(z+1)-1,data);
////        }
////    }
////    for (int yi=ASCHUNKLOC(y)-1;yi<)
//    for (int yi=ASCHUNKLOC(y);yi<ASCHUNKLOC(y+1)-1;yi++) {
//        for (int zi=ASCHUNKLOC(z);zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->hermitifyselection(ASCHUNKLOC(x+1)-1,yi,zi,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x);xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int zi=ASCHUNKLOC(z);zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->hermitifyselection(xi,ASCHUNKLOC(y+1)-1,zi,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x);xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int yi=ASCHUNKLOC(y);yi<ASCHUNKLOC(y+1)-1;yi++) {
//            data->hermitifyselection(xi,yi,ASCHUNKLOC(z+1)-1,data);
//        }
//    }
//    
////    for (int xi=ASCHUNKLOC(x);xi<ASCHUNKLOC(x+1)-1;xi++) {
////        data->hermitifyselection(xi,ASCHUNKLOC(y+1)-1,ASCHUNKLOC(z+1)-1,data);
////    }
////    for (int yi=ASCHUNKLOC(y);yi<ASCHUNKLOC(y+1)-1;yi++) {
////        data->hermitifyselection(ASCHUNKLOC(x+1)-1,yi,ASCHUNKLOC(z+1)-1,data);
////    }
////    for (int zi=ASCHUNKLOC(z);zi<ASCHUNKLOC(z+1)-1;zi++) {
////        data->hermitifyselection(ASCHUNKLOC(x+1)-1,ASCHUNKLOC(y+1)-1,zi,data);
////    }
//    GeometryOctreeLeaf* geo = realworld.getgeomat(x,y,z);
//    //    geo->
//    geo->open = true;
//    geo->preparesnippets();
////    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
////        for (int yi=ASCHUNKLOC(x)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
////            data->geomifyselection(xi, yi,ASCHUNKLOC(z+1)-1,geo,data);
////            data->geomifyselection(xi, yi,ASCHUNKLOC(z+1),geo,data);
////        }
////    }
//    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)+1;yi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)+1;zi++) {
//            data->geomifyselection(ASCHUNKLOC(x+1)-1,yi,zi,geo,data);
//            data->geomifyselection(ASCHUNKLOC(x+1),yi,zi,geo,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->geomifyselection(xi,ASCHUNKLOC(y+1)-1,zi,geo,data);
//            data->geomifyselection(xi,ASCHUNKLOC(y+1),zi,geo,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
//            data->geomifyselection(xi,yi,ASCHUNKLOC(z+1)-1,geo,data);
//            data->geomifyselection(xi,yi,ASCHUNKLOC(z+1),geo,data);
//        }
//    }
////    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
////        data->geomifyselection(xi,ASCHUNKLOC(y+1)-1,ASCHUNKLOC(z+1)-1,geo,data);
////        data->geomifyselection(xi,ASCHUNKLOC(y+1),ASCHUNKLOC(z+1)-1,geo,data);
////        data->geomifyselection(xi,ASCHUNKLOC(y+1)-1,ASCHUNKLOC(z+1),geo,data);
////        data->geomifyselection(xi,ASCHUNKLOC(y+1),ASCHUNKLOC(z+1),geo,data);
////    }
////    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
////        data->geomifyselection(ASCHUNKLOC(x+1)-1,yi,ASCHUNKLOC(z+1)-1,geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1),yi,ASCHUNKLOC(z+1)-1,geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1)-1,yi,ASCHUNKLOC(z+1),geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1),yi,ASCHUNKLOC(z+1),geo,data);
////    }
////    for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
////        data->geomifyselection(ASCHUNKLOC(x+1)-1,ASCHUNKLOC(y+1)-1,zi,geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1),ASCHUNKLOC(y+1)-1,zi,geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1)-1,ASCHUNKLOC(y+1),zi,geo,data);
////        data->geomifyselection(ASCHUNKLOC(x+1),ASCHUNKLOC(y+1),zi,geo,data);
////    }
//    geo->matrixmap(&realworld.matrix);
//    geo->open = false;
}

