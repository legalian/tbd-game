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

#define ASBLOCKLOC +(ALTERNATOR-(ALTERNATOR&(CHSIZE-1)))

#if CHPOWER%2 == 0
    #define ASCHUNKLOC +ALTERNATOR<<CHPOWER
#else
    #define ASCHUNKLOC +ALTALTERNATOR<<CHPOWER
#endif



Edgedat::Edgedat() {
    
}
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

BlockId OctreeSegment::getser(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
void OctreeSegment::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {throw;}
void OctreeSegment::collapse(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*& self) {}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeSegment::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Feature& OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
void OctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
void OctreeSegment::savetofile(std::ostream& file) {throw;}
uint8_t OctreeSegment::getsimpid() {throw;}
bool OctreeSegment::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}

OctreeFeature::OctreeFeature(uint8_t fill) : fillvalue(fill) {}
uint8_t OctreeFeature::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
void OctreeFeature::setser(BlockLoc x,BlockLoc y,BlockLoc z,uint8_t newid,int recur,OctreeSegment*& self) {fillvalue = newid;}
Feature& OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z)  {return point;}
uint8_t OctreeFeature::getsimpid() {return 0;}
void OctreeFeature::savetofile(std::ostream& file) {throw;}
bool OctreeFeature::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return true;}

OctreeLeaf::OctreeLeaf(uint8_t fill) : OctreeFeature(fill) {}
//bool OctreeLeaf::uniqueat(long x,long y,long z,int recur) {return true;}
Edgedat& OctreeLeaf::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return zcondat;}
void OctreeLeaf::savetofile(std::ostream& file) {throw;}

OctreeBud::OctreeBud(uint8_t fill) : fillvalue(fill) {}
uint8_t OctreeBud::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
void OctreeBud::setser(BlockLoc x,BlockLoc y,BlockLoc z,uint8_t newid,int recur,OctreeSegment*& self) {
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
uint8_t OctreeBud::getsimpid() {return fillvalue;}
void OctreeBud::savetofile(std::ostream& file) {throw;}
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


uint8_t OctreeBranch::getser(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getser(x,y,z);
}
void OctreeBranch::setser(BlockLoc x,BlockLoc y,BlockLoc z,uint8_t newid,int recur,OctreeSegment*& self) {
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
void OctreeBranch::savetofile(std::ostream& file) {throw;}
uint8_t OctreeBranch::getsimpid() {return 0;}
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


OctreeSegment* makeOctree(uint8_t (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
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
    int k = (1<<(recur-1));
    OctreeSegment* a = makeOctree(data,x,y,z,recur-1);
    OctreeSegment* b = makeOctree(data,x+k,y,z,recur-1);
    OctreeSegment* c = makeOctree(data,x,y+k,z,recur-1);
    OctreeSegment* d = makeOctree(data,x+k,y+k,z,recur-1);
    OctreeSegment* e = makeOctree(data,x,y,z+k,recur-1);
    OctreeSegment* f = makeOctree(data,x+k,y,z+k,recur-1);
    OctreeSegment* g = makeOctree(data,x,y+k,z+k,recur-1);
    OctreeSegment* h = makeOctree(data,x+k,y+k,z+k,recur-1);
    uint8_t equi = a->getsimpid();
    if (equi!=0 and equi==b->getsimpid() and equi==c->getsimpid() and equi==d->getsimpid() and
                    equi==e->getsimpid() and equi==f->getsimpid() and equi==g->getsimpid() and equi==h->getsimpid()) {
        delete a;
        delete b;
        delete c;
        delete d;
        delete e;
        delete f;
        delete g;
        delete h;
        return new OctreeBud(equi);
    }
    return new OctreeBranch(a,b,c,d,e,f,g,h,recur-1);
}
void Octree::loadportion(BlockLoc x,BlockLoc y,BlockLoc z,uint8_t (*dat)[CHSIZE+1][CHSIZE+1]) {
    expandchunk(x,y,z);
    data->insertinto(x ASCHUNKLOC,y ASCHUNKLOC,z ASCHUNKLOC,depth-CHPOWER,makeOctree(dat,0,0,0,CHPOWER),data);
}
void Octree::loadfromfile(BlockLoc x,BlockLoc y,BlockLoc z,int recur,std::string file) {
    
}
void Octree::setAt(BlockLoc x,BlockLoc y,BlockLoc z,uint8_t newid) {
    expand(x,y,z);
    data->setser(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC,newid,depth,data);
}
uint8_t Octree::getAt(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->getser(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC);
}
Edgedat& Octree::conx(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->xcon(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC);
}
Edgedat& Octree::cony(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->ycon(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC);
}
Edgedat& Octree::conz(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->zcon(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC);
}
Feature& Octree::feat(BlockLoc x,BlockLoc y,BlockLoc z) {
    return data->feat(x ASBLOCKLOC,y ASBLOCKLOC,z ASBLOCKLOC);
}
void Octree::hermitify(BlockLoc x, BlockLoc y, BlockLoc z) {
    for (BlockLoc xt=x<<CHPOWER;xt<(x+1)<<CHPOWER;xt++) {
        for (BlockLoc yt=y<<CHPOWER;yt<(y+1)<<CHPOWER;yt++) {
            for (BlockLoc zt=z<<CHPOWER;zt<(z+1)<<CHPOWER;zt++) {
                if (data->featuresque(xt ASBLOCKLOC,yt ASBLOCKLOC,zt ASBLOCKLOC)) {
                    int size = 0;
                    glm::vec3 points[12];
                    glm::vec3 normals[12];
                    for (int xl=0;xl<2;xl++) {
                        for (int yl=0;yl<2;yl++) {
                            if (getAt(xl+xt,yl+yt,zt)!=getAt(xl+xt,yl+yt,zt+1)) {
                                points[size] = glm::vec3(xl,yl,conz(xl+xt,yl+yt,zt).getoffset());
                                normals[size] = conz(xl+xt,yl+yt,zt).getnorm();
                                size++;
                            }
                        }
                    }
                    for (int xl=0;xl<2;xl++) {
                        for (int zl=0;zl<2;zl++) {
                            if (getAt(xl+xt,yt,zl+zt)!=getAt(xl+xt,yt+1,zl+zt)) {
                                points[size] = glm::vec3(xl,cony(xl+xt,yt,zl+zt).getoffset(),zl);
                                normals[size] = cony(xl+xt,yt,zl+zt).getnorm();
                                size++;
                            }
                        }
                    }
                    for (int yl=0;yl<2;yl++) {
                        for (int zl=0;zl<2;zl++) {
                            if (getAt(xt,yl+yt,zl+zt)!=getAt(xt+1,yl+yt,zl+zt)) {
                                points[size] = glm::vec3(conx(xt,yl+yt,zl+zt).getoffset(),yl,zl);
                                normals[size] = conx(xt,yl+yt,zl+zt).getnorm();
                                size++;
                            }
                        }
                    }
                    
                    
                    feat(xt,yt,zt) = omgqef(normals, points, size);
                    
                }
            }
        }
    }
}
void Octree::save(std::string file) {
    throw;
}
glm::vec3 Octree::omgqef(glm::vec3 normals[], glm::vec3 positions[], int length )
{
//    var A = DenseMatrix.OfRowArrays(normals.Select(e => new[] { e.X, e.Y, e.Z }).ToArray());
//    var b = DenseVector.OfArray(normals.Zip(positions.Select(p => p - meanPoint), Vector3.Dot).ToArray());
//    
//    var pseudo = PseudoInverse(A);
//    var leastsquares = pseudo.Multiply(b);
//    
//    return leastsquares + DenseVector.OfArray(new[] { meanPoint.X, meanPoint.Y, meanPoint.Z });
    glm::vec3 ret;
    for (int k=0;k<length;k++) {
        ret += positions[k];
    }
    return ret/(float)length;
}


//choose 3:
//
//- me + choose 2 from below:
//- = me + choose 1 from below:
//-
//-
//-
//-
//choose(3,0,len,ar,[])
//choose(n,o,max,ar,pre) {
//    if (n=0) return [];
//    if (max-o<n) return [];
//    ret = [];
//    for (i=o;o<max,ar) {
//        ret += choose(n-1,o+1,max,ar,pre+[ar[o]])
//    }
//    return ret;
//}


