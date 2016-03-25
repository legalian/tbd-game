//
//  octree.cpp
//  Total Control
//
//  Created by Parker Lawrence on 2/29/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"
Edgedat::Edgedat() {
    
}
Edgedat::Edgedat(uint8_t xi,uint8_t yi,uint8_t zi,uint8_t ti) {
    x = xi;
    y = yi;
    z = zi;
    t = ti;
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

uint8_t OctreeSegment::getser(long x,long y,long z) {throw;}
void OctreeSegment::setser(long x,long y,long z,uint8_t newid,int recur,OctreeSegment*& self) {throw;}
void OctreeSegment::collapse(long x,long y,long z,OctreeSegment*& self) {}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeSegment::xcon(long x,long y,long z) {throw;}
Edgedat& OctreeSegment::ycon(long x,long y,long z) {throw;}
Edgedat& OctreeSegment::zcon(long x,long y,long z) {throw;}
Feature& OctreeSegment::feat(long x,long y,long z) {throw;}
void OctreeSegment::insertinto(long x,long y,long z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
void OctreeSegment::savetofile(std::ostream& file) {throw;}
uint8_t OctreeSegment::getsimpid() {throw;}
bool OctreeSegment::featuresque(long x,long y,long z) {throw;}

OctreeFeature::OctreeFeature(uint8_t fill) : fillvalue(fill) {}
uint8_t OctreeFeature::getser(long x,long y,long z) {return fillvalue;}
void OctreeFeature::setser(long x,long y,long z,uint8_t newid,int recur,OctreeSegment*& self) {fillvalue = newid;}
Feature& OctreeFeature::feat(long x, long y, long z)  {return point;}
uint8_t OctreeFeature::getsimpid() {return 0;}
void OctreeFeature::savetofile(std::ostream& file) {throw;}
bool OctreeFeature::featuresque(long x,long y,long z) {return true;}

OctreeLeaf::OctreeLeaf(uint8_t fill) : OctreeFeature(fill) {}
//bool OctreeLeaf::uniqueat(long x,long y,long z,int recur) {return true;}
Edgedat& OctreeLeaf::xcon(long x,long y,long z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(long x,long y,long z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(long x,long y,long z) {return zcondat;}
void OctreeLeaf::savetofile(std::ostream& file) {throw;}

OctreeBud::OctreeBud(uint8_t fill) : fillvalue(fill) {}
uint8_t OctreeBud::getser(long x,long y,long z) {return fillvalue;}
void OctreeBud::setser(long x,long y,long z,uint8_t newid,int recur,OctreeSegment*& self) {
    if (newid == fillvalue) {
    } else if (recur==0) {
        fillvalue = newid;
    }
    else {
        delete self;
        self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue));
        self->setser(x,y,z,newid,recur,self);
    }
}
//bool OctreeBud::uniqueat(long x,long y,long z,int recur) {throw;}
void OctreeBud::insertinto(long x,long y,long z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==0) {
        delete self;
        self = toinsert;
    } else {
        delete self;
        self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                new OctreeBud(fillvalue),new OctreeBud(fillvalue));
        self->insertinto(x,y,z,recur,toinsert,self);
    }
}
uint8_t OctreeBud::getsimpid() {return fillvalue;}
void OctreeBud::savetofile(std::ostream& file) {throw;}
bool OctreeBud::featuresque(long x,long y,long z) {return false;}

OctreeBranch::OctreeBranch(OctreeSegment* a,OctreeSegment* b,
                           OctreeSegment* c,OctreeSegment* d,
                           OctreeSegment* e,OctreeSegment* f,
                           OctreeSegment* g,OctreeSegment* h) {
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = b;
    subdivisions[0][1][0] = c;
    subdivisions[1][1][0] = d;
    subdivisions[0][0][1] = e;
    subdivisions[1][0][1] = f;
    subdivisions[0][1][1] = g;
    subdivisions[1][1][1] = h;
}


uint8_t OctreeBranch::getser(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->getser(x>>1,y>>1,z>>1);}
void OctreeBranch::setser(long x,long y,long z,uint8_t newid,int recur,OctreeSegment*& self) {
    subdivisions[x&1][y&1][z&1]->setser(x>>1,y>>1,z>>1,newid,recur-1,subdivisions[x&1][y&1][z&1]);
}
//void OctreeBranch::collapse(long x,long y,long z,OctreeSegment*& self) {
//    subdivisions[x&1][y&1][z&1]->collapse(x>>1,y>>1,z>>1,subdivisions[x&1][y&1][z&1]);
//}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeBranch::xcon(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->xcon(x>>1,y>>1,z>>1);}
Edgedat& OctreeBranch::ycon(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->ycon(x>>1,y>>1,z>>1);}
Edgedat& OctreeBranch::zcon(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->zcon(x>>1,y>>1,z>>1);}
Feature& OctreeBranch::feat(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->feat(x>>1,y>>1,z>>1);}
void OctreeBranch::insertinto(long x,long y,long z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==0) {
        delete self;
        self = toinsert;
    } else {
        subdivisions[x&1][y&1][z&1]->insertinto(x>>1,y>>1,z>>1,recur-1,toinsert,subdivisions[x&1][y&1][z&1]);
    }
}
void OctreeBranch::savetofile(std::ostream& file) {throw;}
uint8_t OctreeBranch::getsimpid() {return 0;}
bool OctreeBranch::featuresque(long x,long y,long z) {return subdivisions[x&1][y&1][z&1]->featuresque(x>>1,y>>1,z>>1);}

void Octree::expand(long x,long y,long z) {expand(underpressure(x,y,z));}
void Octree::expand(long x,long y,long z,int bias) {
    expand(std::max(underpressure(x,y,z),underpressure(x+(1<<bias)-1,y+(1<<bias)-1,z+(1<<bias)-1)));
}
void Octree::expand(int desireddepth) {
//    int desireddepth = ;//+bias;
    while (desireddepth>depth) {
        depth++;
        if (depth&1) {
            data = new OctreeBranch(new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),data);
        } else {
            data = new OctreeBranch(data,new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0),
                                    new OctreeBud(0),new OctreeBud(0));
        }
    }
}
int Octree::underpressure(long x,long y,long z) {
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
long Octree::flipbits(long f) {
    
    f -= ALTERNATOR&(CHSIZE-1);
    long k = 0;
    
    long dist = f+ALTERNATOR;
    for (int w=0;w<depth;w++) {
        //        k|= (dist&(1<<w))>>(desireddepth-1-(w<<1));
        if(dist & (1 << w))
            k |= 1 << ((depth - 1) - w);
    }
    return k;
}

OctreeSegment* OctreeSegment::makeOctree(uint8_t (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
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
    return new OctreeBranch(a,b,c,d,e,f,g,h);
}
void Octree::loadportion(long x,long y,long z,uint8_t (*dat)[CHSIZE+1][CHSIZE+1]) {
    expand(x,y,z,CHPOWER);
    data->insertinto(flipbits(x),flipbits(y),flipbits(z),depth-CHPOWER,OctreeSegment::makeOctree(dat,0,0,0,CHPOWER),data);
}
void Octree::loadfromfile(long x,long y,long z,int recur,std::string file) {
    
}
void Octree::setAt(long x,long y,long z,uint8_t newid) {
    expand(x,y,z);
    data->setser(flipbits(x),flipbits(y),flipbits(z),newid,depth,data);
}
uint8_t Octree::getAt(long x,long y,long z) {
    if (underpressure(x,y,z)>depth) {throw;}
    return data->getser(flipbits(x),flipbits(y),flipbits(z));
}
Edgedat& Octree::conx(long x,long y,long z) {
    if (underpressure(x,y,z)>depth) {throw;}
    return data->xcon(flipbits(x),flipbits(y),flipbits(z));
}
Edgedat& Octree::cony(long x,long y,long z) {
    if (underpressure(x,y,z)>depth) {throw;}
    return data->ycon(flipbits(x),flipbits(y),flipbits(z));
}
Edgedat& Octree::conz(long x,long y,long z) {
    if (underpressure(x,y,z)>depth) {throw;}
    return data->zcon(flipbits(x),flipbits(y),flipbits(z));
}
Feature& Octree::feat(long x,long y,long z) {
    if (underpressure(x,y,z)>depth) {throw;}
    return data->feat(flipbits(x),flipbits(y),flipbits(z));
}
void Octree::hermitify(long x, long y, long z) {
    for (long xt=x<<CHPOWER;xt<(x+1)<<CHPOWER;xt++) {
        for (long yt=y<<CHPOWER;yt<(y+1)<<CHPOWER;yt++) {
            for (long zt=z<<CHPOWER;zt<(z+1)<<CHPOWER;zt++) {
                if (data->featuresque(flipbits(xt), flipbits(yt), flipbits(zt))) {
                    int size = 0;
                    
                    
                    
//                    feat(x,y,z) =
                    
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


