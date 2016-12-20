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



//uint8_t gtable[2][6][2] = {
//    {{0,0},{1,1},{1,0},{1,1},{0,0},{0,1}},
//    {{1,1},{0,0},{1,0},{0,0},{1,1},{0,1}}
//};
uint8_t gtable[2][6] = {
    {0,3,1,3,0,2},
    {3,0,1,0,3,2}
};

Edgedat::Edgedat() {
    
}
Edgedat::Edgedat(uint8_t tc,int8_t xc,int8_t yc,int8_t zc) : x(xc),y(yc),z(zc),t(tc) {}
Edgedat::Edgedat(float xi,float yi,float zi,float ti) {
    x = (int8_t)128*xi;
    y = (int8_t)128*yi;
    z = (int8_t)128*zi;
    t = (uint8_t)255*ti;
}
MatrixCarriage Edgedat::calcxqef() {
    x=255;
    y=0;
    z=0;
    return calcqef(1*(t/255.0)*(x/128.0),(t/255.0),0,0);
}
MatrixCarriage Edgedat::calcyqef() {
    x=0;
    y=255;
    z=0;
    return calcqef(1*(t/255.0)*(y/128.0),0,(t/255.0),0);
}
MatrixCarriage Edgedat::calczqef() {
    x=0;
    y=0;
    z=255;
    return calcqef(1*(t/255.0)*(z/128.0),0,0,(t/255.0));
}
MatrixCarriage Edgedat::calcqef(QefPerc df,QefPerc mx,QefPerc my,QefPerc mz) {
    QefPerc xf = x/128.0;
    QefPerc yf = y/128.0;
    QefPerc zf = z/128.0;
    return MatrixCarriage(
                          //df*df,
                          df*xf,
                          df*yf,
                          df*zf,
                          xf*xf,
                          yf*yf,
                          zf*zf,
                          xf*yf,
                          xf*zf,
                          yf*zf,mx,my,mz,1
                          );
}
glm::vec3 Edgedat::getnorm() {
    return glm::vec3(x/128.0,y/128.0,z/128.0);
}
float Edgedat::getoffset() {
    return t/255.0;
}
glm::vec3 Feature::get(int lod) {
//    return TESTTESTTEST;
    return glm::vec3(float(x)/coef(lod),float(y)/coef(lod),float(z)/coef(lod));
}
//void Feature::operator=(const glm::vec3& other) {
//    x = other.x*255;
//    y = other.y*255;
//    z = other.z*255;
//}
float Feature::coef(int lod) {
    return (1<<(8-lod));
}
float Feature::invcoef(int lod) {
    return (1<<(8-lod));
}
Feature::Feature(const glm::vec3& other,int lod) {
    //    TESTTESTTEST = other;
    x = other.x*invcoef(lod);
    y = other.y*invcoef(lod);
    z = other.z*invcoef(lod);
    
}
Feature::Feature(uint8_t xi,uint8_t yi,uint8_t zi) : x(xi),y(yi),z(zi) {}

Feature::Feature(): x(128),y(128),z(128) {}

MatrixCarriage operator+(const MatrixCarriage &m1, const MatrixCarriage &m2)
{
    return MatrixCarriage(
                          //m1.bb+m2.bb,
                          m1.Abx+m2.Abx,
                          m1.Aby+m2.Aby,
                          m1.Abz+m2.Abz,
                          m1.AAxx+m2.AAxx,
                          m1.AAyy+m2.AAyy,
                          m1.AAzz+m2.AAzz,
                          m1.AAxy+m2.AAxy,
                          m1.AAxz+m2.AAxz,
                          m1.AAyz+m2.AAyz,
                          m1.Mx+m2.Mx,
                          m1.My+m2.My,
                          m1.Mz+m2.Mz,
                          m1.Mt+m2.Mt
    );
}

MatrixCarriage::MatrixCarriage() :
/*bb(0),*/ Abx(0), Aby(0), Abz(0), AAxx(0), AAyy(0), AAzz(0), AAxy(0), AAxz(0), AAyz(0),Mx(0),My(0),Mz(0),Mt(0) {};
MatrixCarriage::MatrixCarriage(QefPerc b,QefPerc c,QefPerc d,QefPerc e,QefPerc f,QefPerc g,QefPerc h,QefPerc i,QefPerc j,QefPerc x,QefPerc y,QefPerc z,int co) :
/*bb(a),*/ Abx(b), Aby(c), Abz(d), AAxx(e), AAyy(f), AAzz(g), AAxy(h), AAxz(i), AAyz(j) ,Mx(x),My(y),Mz(z),Mt(co){};

MatrixCarriage MatrixCarriage::translate(QefPerc x,QefPerc y,QefPerc z) {
    return MatrixCarriage(
                          //bb-x*Abx-y*Aby-z*Abz
                          //+x*x*AAxx+y*y*AAyy+z*z*AAzz
                          //+x*y*AAxy+x*z*AAxz+y*z*AAyz,
                          Abx+x*AAxx+y*AAxy+z*AAxz,
                          Aby+x*AAxy+y*AAyy+z*AAyz,
                          Abz+x*AAxz+y*AAyz+z*AAzz,
                          
                          AAxx,
                          AAyy,
                          AAzz,
                          AAxy,
                          AAxz,
                          AAyz,
                          Mx+x,
                          My+y,
                          Mz+z,
                          Mt
    );
}
Feature MatrixCarriage::evaluate(int lod) {
    glm::dmat3 ATA = glm::dmat3(0.0);
    glm::dvec3 ATb = glm::dvec3(Abx,Aby,Abz);
    
    glm::dvec3 x = glm::dvec3(0.0);
    ATA[0][0] += AAxx;
    ATA[0][1] += AAxy;
    ATA[0][2] += AAxz;
    ATA[1][1] += AAyy;
    ATA[1][2] += AAyz;
    ATA[2][2] += AAzz;
    
//    glm::dvec3 masspoint = glm::dvec3(.5*(1<<lod),.5*(1<<lod),.5*(1<<lod));
    glm::dvec3 masspoint = glm::dvec3(Mx,My,Mz)*(1.0/Mt);
    ATb -= svd_vmul_sym(ATA, masspoint);
    svd_solve_ATA_ATb(ATA, ATb, x);
    
    //qef_calc_error(ATA, x, ATb);
//    std::cout<<Mt;
    x += masspoint;
    
//    if (lod>2) {
//        std::cout<<lod<<" "<<x.x<<" "<<"\n";
//    }
//    x = glm::vec3(float(rand())/RAND_MAX,float(rand())/RAND_MAX,float(rand())/RAND_MAX);
//    std::cout<<;
    return Feature(x,lod);
}

Edgedat silence = Edgedat((float)0,0,0,.5);

void OctreeSegment::vertifySoftLoads() {}
MatrixCarriage OctreeSegment::getqef() {return MatrixCarriage();}
//std::pair<Location,bool> OctreeSegment::getneedsaload(int,int,int,int,int,int) {return std::pair<Location,bool>(Location(0,0,0),false);}
void OctreeSegment::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {}
bool OctreeSegment::isvbaked(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
void OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {}
void OctreeSegment::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
void OctreeSegment::render(const glm::mat4& matr) {}
OctreePortionAwareBranch* OctreeSegment::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {return NULL;}
BlockId OctreeSegment::getser(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
BlockId OctreeSegment::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
Edgedat& OctreeSegment::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
Edgedat& OctreeSegment::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
glm::vec3 OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {throw;}
glm::vec3 OctreeSegment::featwrt(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {return glm::vec3(0,0,0);}
void OctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
void OctreeSegment::filesave(std::ostream& file) {throw;}
void OctreeSegment::worldfilesave(std::ostream& file,int) {throw;}

OctreeSegment* OctreeSegment::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {self = new OctreeBud(1);return this;}

MatrixCarriage OctreeFeature::getqef() {return qef;}
OctreeFeature::OctreeFeature(BlockId fill,uint8_t conn) : fillvalue(fill) , conflag(conn) {}
BlockId OctreeFeature::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
BlockId OctreeFeature::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
glm::vec3 OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);}
glm::vec3 OctreeFeature::featwrt(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);}

void OctreeFeature::filesave(std::ostream& file) {
    char towrite = 'f';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
    file.write((char*)&conflag, sizeof(uint8_t));
}
//bool OctreeFeature::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return true;}

//void OctreeFeature::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
//}
void OctreeFeature::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
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
                qef = qef+(world->zcon(x+xl,y+yl,z).calczqef().translate(xl,yl,0));
            }
        }
    }
    for (int xl=0;xl<2;xl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[xl][0][zl]!=buf[xl][1][zl]) {
                qef = qef+(world->ycon(x+xl,y,z+zl).calcyqef().translate(xl,0,zl));
            }
        }
    }
    for (int yl=0;yl<2;yl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[0][yl][zl]!=buf[1][yl][zl]) {
                qef = qef+(world->xcon(x,y+yl,z+zl).calcxqef().translate(0,yl,zl));
            }
        }
    }
}

void OctreeFeature::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {
    point = qef.evaluate(0);
}

OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn) : OctreeFeature(fill,conn) {}
OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn,int8_t n[12]) : OctreeFeature(fill,conn),
    xcondat(n[0],n[1],n[2],n[3]),
    ycondat(n[4],n[5],n[6],n[7]),
    zcondat(n[8],n[9],n[10],n[11])  {}


#ifdef WIREFRAMEDEBUG
void OctreeFeature::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {
    //drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),1,geometry,1.0,1.0,1.0);
//    if (giveXflag(x,y,z,0,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0))+1,y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),0,1,1,geometry,1,0,0);
//    }
//    if (giveYflag(x,y,z,0,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0))+1,z-(ASBLOCKLOC(0)),1,0,1,geometry,0,1,0);
//    }
//    if (giveZflag(x,y,z,0,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0))+1,1,1,0,geometry,0,0,1);
//    }
}
#endif
void OctreeLeaf::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {
//    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) and
//         ((x&(CHSIZE-1)) != 0       ) and ((y&(CHSIZE-1)) != 0       ) and ((z&(CHSIZE-1)) != 0       ) ) {
//        geomifyselection(x,y,z,geometry,world,lod);
//        drawgeometry(x,y,z,geometry,world,lod,fillvalue);
    
#ifdef WIREFRAMEDEBUG

    Xstitch(x,y,z,geometry,world,fillvalue,world->getser(x+(1<<lod),y,z));
    Ystitch(x,y,z,geometry,world,fillvalue,world->getser(x,y+(1<<lod),z));
    Zstitch(x,y,z,geometry,world,fillvalue,world->getser(x,y,z+(1<<lod)));
#else
    Xstitch(x,y,z,geometry,world,fillvalue,world->getser(x+(1<<lod),y,z));
    Ystitch(x,y,z,geometry,world,fillvalue,world->getser(x,y+(1<<lod),z));
    Zstitch(x,y,z,geometry,world,fillvalue,world->getser(x,y,z+(1<<lod)));
#endif
    
//    }
}
Edgedat& OctreeLeaf::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return zcondat;}
void OctreeLeaf::filesave(std::ostream& file) {
    char towrite = 'l';
    file.write(&towrite,1);
    file.write((char*)&fillvalue,sizeof(uint8_t));
    file.write((char*)&conflag, sizeof(uint8_t));
    file.write((char*)&xcondat.t,sizeof(uint8_t));
    file.write((char*)&xcondat.x,sizeof(int8_t));
    file.write((char*)&xcondat.y,sizeof(int8_t));
    file.write((char*)&xcondat.z,sizeof(int8_t));
    file.write((char*)&ycondat.t,sizeof(uint8_t));
    file.write((char*)&ycondat.x,sizeof(int8_t));
    file.write((char*)&ycondat.y,sizeof(int8_t));
    file.write((char*)&ycondat.z,sizeof(int8_t));
    file.write((char*)&zcondat.t,sizeof(uint8_t));
    file.write((char*)&zcondat.x,sizeof(int8_t));
    file.write((char*)&zcondat.y,sizeof(int8_t));
    file.write((char*)&zcondat.z,sizeof(int8_t));
}

OctreeBud::OctreeBud(BlockId fill) : fillvalue(fill) {}

bool OctreeBud::isvbaked(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue!=0;}
BlockId OctreeBud::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
BlockId OctreeBud::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}


void OctreeBud::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (thisdep==recur) {
        delete self;
        self = toinsert;
    } else {
        delete self;
        if (thisdep==7) {
            self = new OctreePortionAwareBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),thisdep-1,true);
        } else {
            self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),thisdep-1);
        }
        self->insertinto(x,y,z,recur,thisdep,toinsert,self);
    }
}
//BlockId OctreeBud::getsimpid() {return fillvalue;}
void OctreeBud::filesave(std::ostream& file) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
void OctreeBud::worldfilesave(std::ostream& file,int scale) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
//bool OctreeBud::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return false;}
OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,OctreeSegment* b,
                                                   OctreeSegment* c,OctreeSegment* d,
                                                   OctreeSegment* e,OctreeSegment* f,
                                                   OctreeSegment* g,OctreeSegment* h, int dep,bool hard) :
OctreeBranch(a,b,c,d,e,f,g,h,dep),hardLoaded(hard),bakeddetails(hard?0:WORLDFILE_VBITMASK),curlod(hard?-1:MIN_WORLDFILE_GEOMSAVE) {}

OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,OctreeSegment* b,
                                                   OctreeSegment* c,OctreeSegment* d,
                                                   OctreeSegment* e,OctreeSegment* f,
                                                   OctreeSegment* g,OctreeSegment* h, int dep,uint8_t x,uint8_t y,uint8_t z) :
OctreeBranch(a,b,c,d,e,f,g,h,dep,x,y,z),hardLoaded(false),bakeddetails(WORLDFILE_VBITMASK),curlod(MIN_WORLDFILE_GEOMSAVE) {}
OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,int dep,uint8_t x,uint8_t y,uint8_t z,uint8_t con) :
OctreeBranch(a,dep,x,y,z,con),hardLoaded(false),bakeddetails(WORLDFILE_VBITMASK),curlod(MIN_WORLDFILE_GEOMSAVE) {}
void OctreePortionAwareBranch::render(const glm::mat4& matr){
    if (nextpasscleanup!=NULL) {
        delete nextpasscleanup;
        nextpasscleanup = NULL;
    }
    if (geometry!=NULL) {
        for(auto iterator = geometry->begin(); iterator != geometry->end(); iterator++) {
            registergeom(iterator->first,&iterator->second);
        }
    }
}

void OctreePortionAwareBranch::matrixmap(glm::mat4 *matr) {
    for(auto iterator = geometry->begin(); iterator != geometry->end(); iterator++) {
        iterator->second.matrix = matr;
    }
}

//std::pair<Location,bool> OctreePortionAwareBranch::getneedsaload(int xl,int yl,int zl,int px,int py,int pz) {
////    if (!isvbaked(0,0,0)) {
////        return this;
////    }
//    int distance = std::max(std::max(abs(xl-px),abs(zl-pz)),abs(yl-py));
//    int lod = (int)((distance)/2);
//    if (curlod!=lod&&hardLoaded) {
//        std::cout<<lod;
//        return std::pair<Location,bool>(Location(xl-ASCHUNKLOC(0),yl-ASCHUNKLOC(0),zl-ASCHUNKLOC(0)),true);
//    } else {
//        return std::pair<Location,bool>(Location(0,0,0),false);
//    }
//}
void OctreePortionAwareBranch::vertify(BlockLoc x, BlockLoc y, BlockLoc z, int target) {
    if (!(bakeddetails&(1<<target))) {
        bakeddetails |= (1<<target);
        BlockLoc mask = 1<<depth;
        subdivisions[0][0][0]->vertify(x     ,y     ,z     ,target);
        subdivisions[1][0][0]->vertify(x|mask,y     ,z     ,target);
        subdivisions[0][1][0]->vertify(x     ,y|mask,z     ,target);
        subdivisions[1][1][0]->vertify(x|mask,y|mask,z     ,target);
        subdivisions[0][0][1]->vertify(x     ,y     ,z|mask,target);
        subdivisions[1][0][1]->vertify(x|mask,y     ,z|mask,target);
        subdivisions[0][1][1]->vertify(x     ,y|mask,z|mask,target);
        subdivisions[1][1][1]->vertify(x|mask,y|mask,z|mask,target);
    }
}

bool OctreePortionAwareBranch::isvbaked(BlockLoc x,BlockLoc y,BlockLoc z) {
    
    if (curlod == -1) {return false;}
    return bakeddetails&(1<<curlod);
}
#define SNAP(x,y) (x&(BLOCKLOCFULLMASK^((1<<y)-1)))
glm::vec3 OctreePortionAwareBranch::featwrt(BlockLoc x, BlockLoc y, BlockLoc z, int lod) {
//    std::cout<<curlod<<"\n";
//    if (curlod==0) {throw;}a
//    if (lod==0 and curlod==0) {
    x = SNAP(x,curlod);
    y = SNAP(y,curlod);
    z = SNAP(z,curlod);
    return subdivisions XYZINDEX->featwrt(x,y,z,curlod);
//    }
//    else {
//        return glm::vec3(x-(ASBLOCKLOC(0)),-10000,z-(ASBLOCKLOC(0)));
//    }
}
BlockId OctreePortionAwareBranch::getserwrt(BlockLoc x, BlockLoc y, BlockLoc z) {
    x = SNAP(x,curlod);
    y = SNAP(y,curlod);
    z = SNAP(z,curlod);
    return subdivisions XYZINDEX->getserwrt(x,y,z);
}
#undef SNAP

OctreeSegment* OctreePortionAwareBranch::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {
    changed = true;
    if (depth<recur) {
//                DEBUGDEBUG = 1;
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}
OctreePortionAwareBranch* OctreePortionAwareBranch::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {
    
    return this;
}



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
    connections =
    (subdivisions[0][0][0]->giveconflag(0,0,0,depth)&(17*(8+0)) )|
    (subdivisions[1][0][0]->giveconflag(0,0,0,depth)&(17*(8+1)) )|
    (subdivisions[0][1][0]->giveconflag(0,0,0,depth)&(17*(8+2)) )|
    (subdivisions[1][1][0]->giveconflag(0,0,0,depth)&(17*(8+3)) )|
    (subdivisions[0][0][1]->giveconflag(0,0,0,depth)&(17*(8+4)) )|
    (subdivisions[1][0][1]->giveconflag(0,0,0,depth)&(17*(8+5)) )|
    (subdivisions[0][1][1]->giveconflag(0,0,0,depth)&(17*(8+6)) )|
    (subdivisions[1][1][1]->giveconflag(0,0,0,depth)&(17*(8+7)) );
}
OctreeBranch::OctreeBranch(OctreeSegment* a,OctreeSegment* b,
                           OctreeSegment* c,OctreeSegment* d,
                           OctreeSegment* e,OctreeSegment* f,
                           OctreeSegment* g,OctreeSegment* h, int dep,uint8_t x,uint8_t y,uint8_t z) : depth(dep),point(Feature(x,y,z)) {
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = b;
    subdivisions[0][1][0] = c;
    subdivisions[1][1][0] = d;
    subdivisions[0][0][1] = e;
    subdivisions[1][0][1] = f;
    subdivisions[0][1][1] = g;
    subdivisions[1][1][1] = h;
    connections =
    (subdivisions[0][0][0]->giveconflag(0,0,0,depth)&(17*(8+0)) )|
    (subdivisions[1][0][0]->giveconflag(0,0,0,depth)&(17*(8+1)) )|
    (subdivisions[0][1][0]->giveconflag(0,0,0,depth)&(17*(8+2)) )|
    (subdivisions[1][1][0]->giveconflag(0,0,0,depth)&(17*(8+3)) )|
    (subdivisions[0][0][1]->giveconflag(0,0,0,depth)&(17*(8+4)) )|
    (subdivisions[1][0][1]->giveconflag(0,0,0,depth)&(17*(8+5)) )|
    (subdivisions[0][1][1]->giveconflag(0,0,0,depth)&(17*(8+6)) )|
    (subdivisions[1][1][1]->giveconflag(0,0,0,depth)&(17*(8+7)) );
}
OctreeBranch::OctreeBranch(OctreeSegment* a,int dep,uint8_t x,uint8_t y,uint8_t z,uint8_t conn) : depth(dep),point(Feature(x,y,z)),connections(conn) {
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = a;
    subdivisions[0][1][0] = a;
    subdivisions[1][1][0] = a;
    subdivisions[0][0][1] = a;
    subdivisions[1][0][1] = a;
    subdivisions[0][1][1] = a;
    subdivisions[1][1][1] = a;
}
void OctreeBranch::render(const glm::mat4& matr) {
    subdivisions[0][0][0]->render(matr);
    subdivisions[1][0][0]->render(matr);
    subdivisions[0][1][0]->render(matr);
    subdivisions[1][1][0]->render(matr);
    subdivisions[0][0][1]->render(matr);
    subdivisions[1][0][1]->render(matr);
    subdivisions[0][1][1]->render(matr);
    subdivisions[1][1][1]->render(matr);
}
MatrixCarriage OctreeBranch::getqef() {
    return subdivisions[0][0][0]->getqef()+
           subdivisions[1][0][0]->getqef().translate(1<<(depth),0         ,0)+
           subdivisions[0][1][0]->getqef().translate(0         ,1<<(depth),0)+
           subdivisions[1][1][0]->getqef().translate(1<<(depth),1<<(depth),0)+
           subdivisions[0][0][1]->getqef().translate(0         ,0         ,1<<(depth))+
           subdivisions[1][0][1]->getqef().translate(1<<(depth),0         ,1<<(depth))+
           subdivisions[0][1][1]->getqef().translate(0         ,1<<(depth),1<<(depth))+
           subdivisions[1][1][1]->getqef().translate(1<<(depth),1<<(depth),1<<(depth));
}

void OctreeBranch::vertifySoftLoads() {
    if (depth+1>=MIN_WORLDFILE_GEOMSAVE&&depth+1<=MAX_WORLDFILE_GEOMSAVE) {
        point = getqef().evaluate(depth+1);
    }
    subdivisions[0][0][0]->vertifySoftLoads();
    subdivisions[1][0][0]->vertifySoftLoads();
    subdivisions[0][1][0]->vertifySoftLoads();
    subdivisions[1][1][0]->vertifySoftLoads();
    subdivisions[0][0][1]->vertifySoftLoads();
    subdivisions[1][0][1]->vertifySoftLoads();
    subdivisions[0][1][1]->vertifySoftLoads();
    subdivisions[1][1][1]->vertifySoftLoads();
}

void OctreeBranch::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {
    if (depth<target) {
        point = getqef().evaluate(target);
    } else {
        BlockLoc mask = 1<<depth;
        subdivisions[0][0][0]->vertify(x     ,y     ,z     ,target);
        subdivisions[1][0][0]->vertify(x|mask,y     ,z     ,target);
        subdivisions[0][1][0]->vertify(x     ,y|mask,z     ,target);
        subdivisions[1][1][0]->vertify(x|mask,y|mask,z     ,target);
        subdivisions[0][0][1]->vertify(x     ,y     ,z|mask,target);
        subdivisions[1][0][1]->vertify(x|mask,y     ,z|mask,target);
        subdivisions[0][1][1]->vertify(x     ,y|mask,z|mask,target);
        subdivisions[1][1][1]->vertify(x|mask,y|mask,z|mask,target);
    }
}

//std::pair<Location,bool> OctreeBranch::getneedsaload(int x,int y,int z,int px,int py,int pz) {
//    BlockLoc mask = 1<<depth;
//    std::pair<Location,bool> a = subdivisions[0][0][0]->getneedsaload(x     ,y     ,z     ,px,py,pz);if (a.second) {return a;}
//    std::pair<Location,bool> b = subdivisions[1][0][0]->getneedsaload(x|mask,y     ,z     ,px,py,pz);if (b.second) {return b;}
//    std::pair<Location,bool> c = subdivisions[0][1][0]->getneedsaload(x     ,y|mask,z     ,px,py,pz);if (c.second) {return c;}
//    std::pair<Location,bool> d = subdivisions[1][1][0]->getneedsaload(x|mask,y|mask,z     ,px,py,pz);if (d.second) {return d;}
//    std::pair<Location,bool> e = subdivisions[0][0][1]->getneedsaload(x     ,y     ,z|mask,px,py,pz);if (e.second) {return e;}
//    std::pair<Location,bool> f = subdivisions[1][0][1]->getneedsaload(x|mask,y     ,z|mask,px,py,pz);if (f.second) {return f;}
//    std::pair<Location,bool> g = subdivisions[0][1][1]->getneedsaload(x     ,y|mask,z|mask,px,py,pz);if (g.second) {return g;}
//    std::pair<Location,bool> h = subdivisions[1][1][1]->getneedsaload(x|mask,y|mask,z|mask,px,py,pz);if (h.second) {return h;}
//    return std::pair<Location,bool>(Location(0,0,0),false);
//}
bool OctreeBranch::isvbaked(BlockLoc x,BlockLoc y,BlockLoc z) {return subdivisions XYZINDEX->isvbaked(x,y,z);}



#ifdef WIREFRAMEDEBUG
void drawdebugcube(float x,float y,float z,float a,float b,float c,std::map<uint8_t,GeomTerrain>* geometry,float u,float v,float w) {
    glm::vec3 base = glm::vec3(x,y,z);
    (*geometry)[0].addWireVert(base+glm::vec3(0, 0, 0));(*geometry)[0].addWireVert(base+glm::vec3(a, 0, 0));
    (*geometry)[0].addWireVert(base+glm::vec3(0, b, 0));(*geometry)[0].addWireVert(base+glm::vec3(a, b, 0));
    (*geometry)[0].addWireVert(base+glm::vec3(0, 0, c));(*geometry)[0].addWireVert(base+glm::vec3(a, 0, c));
    (*geometry)[0].addWireVert(base+glm::vec3(0, b, c));(*geometry)[0].addWireVert(base+glm::vec3(a, b, c));
    (*geometry)[0].addWireVert(base+glm::vec3(0, 0, 0));(*geometry)[0].addWireVert(base+glm::vec3(0, b, 0));
    (*geometry)[0].addWireVert(base+glm::vec3(a, 0, 0));(*geometry)[0].addWireVert(base+glm::vec3(a, b, 0));
    (*geometry)[0].addWireVert(base+glm::vec3(0, 0, c));(*geometry)[0].addWireVert(base+glm::vec3(0, b, c));
    (*geometry)[0].addWireVert(base+glm::vec3(a, 0, c));(*geometry)[0].addWireVert(base+glm::vec3(a, b, c));
    (*geometry)[0].addWireVert(base+glm::vec3(0, 0, 0));(*geometry)[0].addWireVert(base+glm::vec3(0, 0, c));
    (*geometry)[0].addWireVert(base+glm::vec3(a, 0, 0));(*geometry)[0].addWireVert(base+glm::vec3(a, 0, c));
    (*geometry)[0].addWireVert(base+glm::vec3(0, b, 0));(*geometry)[0].addWireVert(base+glm::vec3(0, b, c));
    (*geometry)[0].addWireVert(base+glm::vec3(a, b, 0));(*geometry)[0].addWireVert(base+glm::vec3(a, b, c));
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    (*geometry)[0].addWireColor(u,v,w);(*geometry)[0].addWireColor(u,v,w);
    
}
#endif
BranchRegistry* SWO;
void OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {
    
#ifdef WIREFRAMEDEBUG
    extern BranchRegistry* SWO;
    
    if (DEBUGDEBUG!=0) {
        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),(1<<(depth+1)),(1<<(depth+1)),(1<<(depth+1)),geometry,
                      1,1,1);
    }

#endif
    if (depth<lod) {
//        int mask = CHSIZE-(1<<lod);
//        if (((x&mask) != mask) and ((y&mask) != mask) and ((z&mask) != mask) and
        //            ((x&mask) != 0   ) and ((y&mask) != 0   ) and ((z&mask) != 0   )) {
//#define CURLODGRAB(d) (expr=d)!=NULL?expr->curlod:-2
        //        OctreePortionAwareBranch* expr;
#define NULLSUB(a) a==NULL?56:a->curlod
        OctreePortionAwareBranch* a=world->getvoxunit(x-1,y,z);
        OctreePortionAwareBranch* b=world->getvoxunit(x,y-1,z);
        OctreePortionAwareBranch* c=world->getvoxunit(x,y,z-1);
        OctreePortionAwareBranch* d=world->getvoxunit(x-1,y-1,z);
        OctreePortionAwareBranch* e=world->getvoxunit(x-1,y,z-1);
        OctreePortionAwareBranch* f=world->getvoxunit(x,y-1,z-1);
        int lodmin = std::min(std::min(std::min(NULLSUB(a),NULLSUB(b)),NULLSUB(c)), std::min(std::min(NULLSUB(d),NULLSUB(e)),NULLSUB(f)));
//        int lodmin = std::min(std::min(std::min(world->getvoxunit(x-1,y,z)->curlod  ,world->getvoxunit(x,y-1,z)->curlod  ),world->getvoxunit(x,y,z-1)->curlod  ),
//                              std::min(std::min(world->getvoxunit(x-1,y-1,z)->curlod,world->getvoxunit(x,y-1,z-1)->curlod),world->getvoxunit(x-1,y,z-1)->curlod));
#undef NULLSUB
//        int lodmax = std::max(std::max(std::max(world->getlodat(x-1,y,z)  ,world->getlodat(x,y-1,z)  ),world->getlodat(x,y,z-1)  ),
//                              std::max(std::max(world->getlodat(x-1,y-1,z),world->getlodat(x,y-1,z-1)),world->getlodat(x-1,y,z-1)));
        if (lodmin>=lod) {
//            drawgeometry(x,y,z,geometry,world,lod,);
            uint8_t an  = getser(0,0,0);
//            uint8_t anx = world->getser(x+(1<<lod),y,z);
//            uint8_t any = world->getser(x,y+(1<<lod),z);
//            uint8_t anz = world->getser(x,y,z+(1<<lod));
            Xstitch(x,y,z,geometry,world,an,world->getser(x+(1<<lod),y,z));
            Ystitch(x,y,z,geometry,world,an,world->getser(x,y+(1<<lod),z));
            Zstitch(x,y,z,geometry,world,an,world->getser(x,y,z+(1<<lod)));
        } else {
            for (int t=0;t<(1<<lod);t+=(1<<lodmin)) {
                Xstitch(x+t,y,z,geometry,world,getser(t,0,0),world->getser(x+t+(1<<lodmin),y,z));
                Ystitch(x,y+t,z,geometry,world,getser(0,t,0),world->getser(x,y+t+(1<<lodmin),z));
                Zstitch(x,y,z+t,geometry,world,getser(0,0,t),world->getser(x,y,z+t+(1<<lodmin)));
                for (int i=(1<<lodmin);i<(1<<lod);i+=(1<<lodmin)) {
                    Xstitch(x+t,y+i,z,geometry,world,getser(t,i,0),world->getser(x+t+(1<<lodmin),y+i,z));
                    Ystitch(x,y+t,z+i,geometry,world,getser(0,t,i),world->getser(x,y+t+(1<<lodmin),z+i));
                    Zstitch(x+i,y,z+t,geometry,world,getser(i,0,t),world->getser(x+i,y,z+t+(1<<lodmin)));
                    
                    Xstitch(x+t,y,z+i,geometry,world,getser(t,0,i),world->getser(x+t+(1<<lodmin),y,z+i));
                    Ystitch(x+i,y+t,z,geometry,world,getser(i,t,0),world->getser(x+i,y+t+(1<<lodmin),z));
                    Zstitch(x,y+i,z+t,geometry,world,getser(0,i,t),world->getser(x,y+i,z+t+(1<<lodmin)));
                }
            }
        }
    } else {
        
        BlockLoc mask = 1<<depth;
        subdivisions[0][0][0]->geomify(x     ,y     ,z     ,geometry,world,lod);
        subdivisions[1][0][0]->geomify(x|mask,y     ,z     ,geometry,world,lod);
        subdivisions[0][1][0]->geomify(x     ,y|mask,z     ,geometry,world,lod);
        subdivisions[1][1][0]->geomify(x|mask,y|mask,z     ,geometry,world,lod);
        subdivisions[0][0][1]->geomify(x     ,y     ,z|mask,geometry,world,lod);
        subdivisions[1][0][1]->geomify(x|mask,y     ,z|mask,geometry,world,lod);
        subdivisions[0][1][1]->geomify(x     ,y|mask,z|mask,geometry,world,lod);
        subdivisions[1][1][1]->geomify(x|mask,y|mask,z|mask,geometry,world,lod);
    }
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
OctreePortionAwareBranch* OctreeBranch::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getvoxunit(x,y,z);
}

BlockId OctreeBranch::getser(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getser(x,y,z);
}
BlockId OctreeBranch::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getserwrt(x,y,z);
}

Edgedat& OctreeBranch::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->xcon(x,y,z);
}
Edgedat& OctreeBranch::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->ycon(x,y,z);
}
Edgedat& OctreeBranch::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->zcon(x,y,z);
}
glm::vec3 OctreeBranch::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {
//        return point;
    if (depth<lod) {
        return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);
    } else {
        return subdivisions XYZINDEX->feat(x,y,z,lod);
    }
}
glm::vec3 OctreeBranch::featwrt(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {
    //    return point;
    if (depth<lod) {
        return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);
    } else {
        return subdivisions XYZINDEX->featwrt(x,y,z,lod);
    }
}
void OctreeBranch::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==thisdep) {
        delete self;//possible memory leak; todo:make branches delete their children in deconstructors
        self = toinsert;
        
    } else {
        subdivisions XYZINDEX->insertinto(x,y,z,recur,depth,toinsert,subdivisions XYZINDEX);
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
void OctreeBranch::worldfilesave(std::ostream& file,int scale) {
    char towrite = 'c';
    file.write(&towrite,1);
    if (scale<=MAX_WORLDFILE_GEOMSAVE) {
        file.write((char*)&point.x, sizeof(uint8_t));
        file.write((char*)&point.y, sizeof(uint8_t));
        file.write((char*)&point.z, sizeof(uint8_t));
        if (scale==MIN_WORLDFILE_GEOMSAVE) {
            uint8_t getmeh = getser(0,0,0);
            file.write((char*)&connections, sizeof(uint8_t));
            file.write((char*)&getmeh, sizeof(uint8_t));
        }
    }
    if (scale>MIN_WORLDFILE_GEOMSAVE) {
        subdivisions[0][0][0]->worldfilesave(file,scale-1);
        subdivisions[1][0][0]->worldfilesave(file,scale-1);
        subdivisions[0][1][0]->worldfilesave(file,scale-1);
        subdivisions[1][1][0]->worldfilesave(file,scale-1);
        subdivisions[0][0][1]->worldfilesave(file,scale-1);
        subdivisions[1][0][1]->worldfilesave(file,scale-1);
        subdivisions[0][1][1]->worldfilesave(file,scale-1);
        subdivisions[1][1][1]->worldfilesave(file,scale-1);
    }
}


OctreeSegment* OctreeBranch::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {
    if (depth<recur) {
//        DEBUGDEBUG = 1;a
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}

void Octree::expand(BlockLoc x,BlockLoc y,BlockLoc z) {throw;expand(underpressure(x,y,z));}
void Octree::expandchunk(BlockLoc x,BlockLoc y,BlockLoc z) {
//        expand(std::max(underpressure(x<<CHPOWER,y<<CHPOWER,z<<CHPOWER),
//                        underpressure(((x+1)<<CHPOWER)-1,((y+1)<<CHPOWER)-1,((z+1)<<CHPOWER)-1 )));
    expand(std::max(underpressure(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z)),
                    underpressure(ASCHUNKLOC(x+1)-1,ASCHUNKLOC(y+1)-1,ASCHUNKLOC(z+1)-1)));
}
void Octree::expandarbit(BlockLoc x,BlockLoc y,BlockLoc z,int recur) {
    expand(std::max(underpressure(x,y,z),
                    underpressure(x+(1<<recur)-1,y+(1<<recur)-1,z+(1<<recur)-1)));
}
void Octree::expand(int desireddepth) {
//    int desireddepth = ;//+bias;
    while (desireddepth>=depth+1) {
        depth++;
        if (depth!=7) {
            if (depth&1) {
                data = new OctreeBranch(new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),data,depth-1);
            } else {
                data = new OctreeBranch(data,new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),depth-1);
            }
        } else {
            if (depth&1) {
                data = new OctreePortionAwareBranch(new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),data,depth-1,false);
            } else {
                data = new OctreePortionAwareBranch(data,new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),depth-1,true);
            }
        }
    }
}
int Octree::underpressure(BlockLoc x,BlockLoc y,BlockLoc z) {
    x ^= ASBLOCKLOC(0);
    y ^= ASBLOCKLOC(0);
    z ^= ASBLOCKLOC(0);
    int r;
    for (r=0;(1<<r)<=x or (1<<r)<=y or (1<<r)<=z;r++) {}
    return r;
}

OctreeSegment* makeOctree(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 'l') {
        uint8_t id;
        uint8_t conflag;
        int8_t ids[12];
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &conflag,sizeof(uint8_t));
        file.read((char*) &ids,sizeof(int8_t)*12);
        return new OctreeLeaf(id,conflag,ids);
    } else if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'f') {
        uint8_t id;
        uint8_t conflag;
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &conflag,sizeof(uint8_t));
        return new OctreeFeature(id,conflag);
    } else if (tester == 'c') {
        recur--;
        if (recur == 6) {
            return new OctreePortionAwareBranch(makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),recur,true);
        } else {
            return new OctreeBranch(makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),recur);
        }
    } else {
        throw;
    }
}

OctreeSegment* loadWorldFile(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'c') {
        recur--;
        if (recur<=MAX_WORLDFILE_GEOMSAVE-1) {
            uint8_t x;
            uint8_t y;
            uint8_t z;
            file.read((char*) &x,sizeof(uint8_t));
            file.read((char*) &y,sizeof(uint8_t));
            file.read((char*) &z,sizeof(uint8_t));
            if (recur > MIN_WORLDFILE_GEOMSAVE-1) {
                if (recur == CHPOWER-1) {
                    return new OctreePortionAwareBranch(loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),recur,x,y,x);
                } else {
                    return new OctreeBranch(loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),recur,x,y,x);
                }
            } else {
                uint8_t conn;
                uint8_t id;
                file.read((char*) &id,sizeof(uint8_t));
                file.read((char*) &conn,sizeof(uint8_t));
                if (recur == CHPOWER-1) {
                    return new OctreePortionAwareBranch(new OctreeBud(id),recur,x,y,x,conn);
                } else {
                    return new OctreeBranch(new OctreeBud(id),recur,x,y,x,conn);
                }
            }
        } else {
            if (recur == CHPOWER-1) {
                return new OctreePortionAwareBranch(loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),recur,false);
            } else {
                return new OctreeBranch(loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),recur);
            }
        }
    } else {
        std::cout<<tester<<"\n";
        throw;
    }
}


OctreeSegment* makeOctree(BlockId (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
    extern uint8_t materialattribs[];
    if (recur == 0) {
        uint8_t equi = data[x][y][z];
        uint8_t meshcomp = (materialattribs[data[x+1][y+1][z+1]]&1);
        uint8_t volumecomp = (materialattribs[equi]&1);
        
        uint8_t connectionflag =
        ((meshcomp^(materialattribs[data[x][y][z]]&1))*(8) ) |
        ((meshcomp^(materialattribs[data[x+1][y][z]]&1))*(1+8+16) ) |
        ((meshcomp^(materialattribs[data[x][y+1][z]]&1))*(2+8+32) ) |
        ((meshcomp^(materialattribs[data[x][y][z+1]]&1))*(4+8+64) ) |
        ((meshcomp^(materialattribs[data[x][y+1][z+1]]&1))*(2+4+8+32+64) ) |
        ((meshcomp^(materialattribs[data[x+1][y][z+1]]&1))*(1+4+8+16+64) ) |
        ((meshcomp^(materialattribs[data[x+1][y+1][z]]&1))*(1+2+8+16+32) );
        if ((connectionflag&8)|volumecomp) {
            connectionflag |=
            ((materialattribs[data[x+1][y][z]]&1)<<4) |
            ((materialattribs[data[x][y+1][z]]&1)<<5) |
            ((materialattribs[data[x][y][z+1]]&1)<<6) |
            128;
        }
        
        if (equi==data[x+1][y][z] and equi==data[x][y+1][z] and equi==data[x][y][z+1]) {
            if (equi==data[x+1][y+1][z] and equi==data[x+1][y][z+1] and equi==data[x][y+1][z+1] and equi==data[x+1][y+1][z+1]) {
                return new OctreeBud(data[x][y][z]);
            } else {
                return new OctreeFeature(data[x][y][z],connectionflag);
            }
        } else {
            return new OctreeLeaf(data[x][y][z],connectionflag);
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
                    if (recur == 7) {return new OctreePortionAwareBranch(a,b,c,d,e,f,g,h,recur-1,true);}
                    else {return new OctreeBranch(a,b,c,d,e,f,g,h,recur-1);}
                }
            }
        }
    }
    return new OctreeBud(equi);
}


void Octree::loadportion(BlockLoc x,BlockLoc y,BlockLoc z,BlockId (*dat)[CHSIZE+1][CHSIZE+1]) {
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,depth,makeOctree(dat,0,0,0,CHPOWER),data);
}
Octree::Octree(std::string name,glm::mat4& trans,Environment& backref,int pop) : matrixRef(trans), currenttests(backref), popular(pop), depth(CHPOWER) {
    if (boost::filesystem::exists(SAVEDIR "/"+name+"/massfile")) {
        std::ifstream file = std::ifstream(SAVEDIR "/"+name+"/massfile",std::ios::in|std::ios::binary);
        file.read((char*)&depth,sizeof(int));
        data = loadWorldFile(file,depth);
        file.close();
    } else {
        data = new OctreeBud(pop);
    }
}
void Octree::filepushportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    std::ofstream file = std::ofstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::out|std::ios::binary|std::ios::trunc);
    OctreePortionAwareBranch* look = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
    if (look!=NULL) {
        look->filesave(file);
    } else {
        OctreeBud(data->getser(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))).filesave(file);
    }
    file.close();
}
void Octree::filepullportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    //    file =
    std::ifstream file = std::ifstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::in|std::ios::binary);
    std::cout<<filebase<<" was opened. \n";
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,depth,makeOctree(file,CHPOWER),data);
    file.close();
}
bool Octree::dataexists(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    return boost::filesystem::exists(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)));
}
void Octree::render() {
    data->render(matrixRef);
}
bool Octree::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (underpressure(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))>depth) {
        return popular!=0;
    }
    OctreePortionAwareBranch* vox=data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
    if (vox==NULL) {
        return data->getser(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))!=0;
    } else {
        return vox->hardLoaded;
    }
}
void Octree::h_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (existsat(x  ,y  ,z  ) and existsat(x+1,y  ,z  ) and
        existsat(x  ,y+1,z  ) and existsat(x+1,y+1,z  ) and
        existsat(x  ,y  ,z+1) and existsat(x+1,y  ,z+1) and
        existsat(x  ,y+1,z+1) and existsat(x+1,y+1,z+1)) {
        
        OctreePortionAwareBranch* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        if (portion != NULL) {
            portion->hermitify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),data);
        }
    }
}
void Octree::v_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (existsat(x  ,y  ,z  ) and existsat(x+1,y  ,z  ) and
        existsat(x  ,y+1,z  ) and existsat(x+1,y+1,z  ) and
        existsat(x  ,y  ,z+1) and existsat(x+1,y  ,z+1) and
        existsat(x  ,y+1,z+1) and existsat(x+1,y+1,z+1)) {
        
        OctreePortionAwareBranch* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        if (portion != NULL) {
            portion->vertify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),portion->curlod);
        }
    }
}
void Octree::g_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    
    if (data->isvbaked(ASCHUNKLOC(x-1),ASCHUNKLOC(y-1),ASCHUNKLOC(z-1)) and data->isvbaked(ASCHUNKLOC(x  ),ASCHUNKLOC(y-1),ASCHUNKLOC(z-1)) and
        data->isvbaked(ASCHUNKLOC(x-1),ASCHUNKLOC(y  ),ASCHUNKLOC(z-1)) and data->isvbaked(ASCHUNKLOC(x  ),ASCHUNKLOC(y  ),ASCHUNKLOC(z-1)) and
        data->isvbaked(ASCHUNKLOC(x-1),ASCHUNKLOC(y-1),ASCHUNKLOC(z  )) and data->isvbaked(ASCHUNKLOC(x  ),ASCHUNKLOC(y-1),ASCHUNKLOC(z  )) and
        data->isvbaked(ASCHUNKLOC(x-1),ASCHUNKLOC(y  ),ASCHUNKLOC(z  )) and data->isvbaked(ASCHUNKLOC(x  ),ASCHUNKLOC(y  ),ASCHUNKLOC(z  )) ){
        
        
        OctreePortionAwareBranch* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        
        if (portion!=NULL) {
            
//            std::cout<<"fixed "<<std::max(std::max(abs(x),abs(y)),abs(z))<<", location :"<<Location(x,y,z).tostring()<<"\n";
            const int lodlookup[7][3] = {{0,0,0},{-1,0,0},{0,-1,0},{-1,-1,0},{0,0,-1},{-1,0,-1},{0,-1,-1}};
            bool needchange = false;
            for (int i=0;i<7;i++) {
                OctreePortionAwareBranch* ext = data->getvoxunit(ASCHUNKLOC(x+lodlookup[i][0]),ASCHUNKLOC(y+lodlookup[i][1]),ASCHUNKLOC(z+lodlookup[i][2]));
                int lod = ext!=NULL?ext->curlod:-2;
                if (portion->lodserial[i]!=lod) {
                    portion->lodserial[i]=lod;
                    needchange=true;
                }
            }
            if (needchange) {
                int lod = portion->curlod;
        
                extern BranchRegistry* SWO;
                SWO = &currenttests;
                std::map<uint8_t,GeomTerrain>* newgeometry = new std::map<uint8_t,GeomTerrain>();
                portion->geomify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),newgeometry,data,lod);
                
                portion->nextpasscleanup = portion->geometry;
                portion->geometry = newgeometry;
        
                portion->matrixmap(&matrixRef);
                
            }
        }
    }
}

void Xstitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anx) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[anx]) {
        glm::vec3 buf[] = {
            world->featwrt(x,y  ,z  ,0),
            world->featwrt(x,y-1,z  ,0),
            world->featwrt(x,y  ,z-1,0),
            world->featwrt(x,y-1,z-1,0)
        };
        if (materialprops[fillvalue]>materialprops[anx]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {//if (materialprops[fillvalue]<materialprops[anx]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[anx].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}
void Ystitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId any) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[any]) {
        glm::vec3 buf[] = {
            world->featwrt(x  ,y,z  ,0),
            world->featwrt(x-1,y,z  ,0),
            world->featwrt(x  ,y,z-1,0),
            world->featwrt(x-1,y,z-1,0)
        };
        if (materialprops[fillvalue]>materialprops[any]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[0][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[any]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[any].addVert(buf[gtable[1][ik]]);
            }
        }
    }
}
void Zstitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anz) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[anz]) {
        glm::vec3 buf[] = {
            world->featwrt(x  ,y  ,z,0),
            world->featwrt(x-1,y  ,z,0),
            world->featwrt(x  ,y-1,z,0),
            world->featwrt(x-1,y-1,z,0)
        };
        if (materialprops[fillvalue]>materialprops[anz]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[anz]) {
            for (int ik=0;ik<6;ik++) {
                (*geometry)[anz].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}



