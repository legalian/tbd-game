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

Feature::Feature(): x(128),y(128),z(128){
}

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
    
    x += masspoint;
    if (x.x<0 or x.x>(1<<lod) or x.y<0 or x.y>(1<<lod) or x.z<0 or x.z>(1<<lod)) {
        x=masspoint;
    }
    
//    if (lod>2) {
//        std::cout<<lod<<" "<<x.x<<" "<<"\n";
//    }
//    x = glm::vec3(float(rand())/RAND_MAX,float(rand())/RAND_MAX,float(rand())/RAND_MAX);
//    std::cout<<;
    return Feature(x,lod);
}



MatrixCarriage OctreeSegment::getqef() {return MatrixCarriage();}
//void OctreeSegment::vertifyselection(BlockLoc x,BlockLoc y,BlockLoc z,int target) {}
//void OctreeSegment::geomifyselectionX(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {}
//void OctreeSegment::geomifyselectionY(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {}
//void OctreeSegment::geomifyselectionZ(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {}
//void OctreeSegment::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
void OctreeSegment::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {}
void OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int lod) {}
void OctreeSegment::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
OctreeSegment* OctreeSegment::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {return this;}
//void OctreeSegment::hermitifyportion(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
BlockId OctreeSegment::getser(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
BlockId OctreeSegment::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
//void OctreeSegment::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {throw;}
//void OctreeSegment::collapse(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment*& self) {}
//bool OctreeSegment::uniqueat(long x,long y,long z,int recur) {throw;}
Edgedat& OctreeSegment::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
glm::vec3 OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {throw 79;}
glm::vec3 OctreeSegment::featwrt(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {return glm::vec3(0,0,0);}
void OctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
void OctreeSegment::filesave(std::ostream& file) {throw;}
//BlockId OctreeSegment::getsimpid() {throw;}
//bool OctreeSegment::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
int OctreeSegment::getlodat(BlockLoc x, BlockLoc y, BlockLoc z) {return -2;}
void OctreeSegment::setlodat(BlockLoc x, BlockLoc y, BlockLoc z,int newlod) {}

MatrixCarriage OctreeFeature::getqef() {return qef;}
OctreeFeature::OctreeFeature(BlockId fill) : fillvalue(fill) {}
BlockId OctreeFeature::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
BlockId OctreeFeature::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
//void OctreeFeature::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {fillvalue = newid;}
glm::vec3 OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);}
glm::vec3 OctreeFeature::featwrt(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)))+point.get(lod);}
//BlockId OctreeFeature::getsimpid() {return 0;}
void OctreeFeature::filesave(std::ostream& file) {
    char towrite = 'f';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
//bool OctreeFeature::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return true;}

//void OctreeFeature::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
//}
void OctreeFeature::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
//    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) ) {
        //        hermitifyselection(x, y, z, world);
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
        
//    }
}
//void OctreeFeature::vertifyselection(BlockLoc x, BlockLoc y, BlockLoc z, int target) {
//    point = qef.evaluate(0);
//}
void OctreeFeature::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {
//    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) ) {
//        vertifyselection(x, y, z, target);
        point = qef.evaluate(0);
//    }
}

OctreeLeaf::OctreeLeaf(BlockId fill) : OctreeFeature(fill) {}
OctreeLeaf::OctreeLeaf(BlockId fill,int8_t n[12]) : OctreeFeature(fill),
    xcondat(n[0],n[1],n[2],n[3]),
    ycondat(n[4],n[5],n[6],n[7]),
    zcondat(n[8],n[9],n[10],n[11])  {}
//bool OctreeLeaf::uniqueat(long x,long y,long z,int recur) {return true;}
//void OctreeLeaf::geomifyselectionX(BlockLoc x, BlockLoc y, BlockLoc z,GeometryOctreeLeaf* geometry, OctreeSegment *world,int* lod,BlockId otherfill) {
//    Xstitch(x,y,z,geometry,world,lod,fillvalue,otherfill);
//}
//void OctreeLeaf::geomifyselectionY(BlockLoc x, BlockLoc y, BlockLoc z,GeometryOctreeLeaf* geometry, OctreeSegment *world,int* lod,BlockId otherfill) {
//    Ystitch(x,y,z,geometry,world,lod,fillvalue,otherfill);
//}
//void OctreeLeaf::geomifyselectionZ(BlockLoc x, BlockLoc y, BlockLoc z,GeometryOctreeLeaf* geometry, OctreeSegment *world,int* lod,BlockId otherfill) {
//    Zstitch(x,y,z,geometry,world,lod,fillvalue,otherfill);
//}
#ifdef WIREFRAMEDEBUG
void OctreeFeature::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int lod) {
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
void OctreeLeaf::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int lod) {
//    if ( ((x&(CHSIZE-1)) != CHSIZE-1) and ((y&(CHSIZE-1)) != CHSIZE-1) and ((z&(CHSIZE-1)) != CHSIZE-1) and
//         ((x&(CHSIZE-1)) != 0       ) and ((y&(CHSIZE-1)) != 0       ) and ((z&(CHSIZE-1)) != 0       ) ) {
//        geomifyselection(x,y,z,geometry,world,lod);
//        drawgeometry(x,y,z,geometry,world,lod,fillvalue);
    
#ifdef WIREFRAMEDEBUG
//    glm::vec3 base = glm::vec3(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 0));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 0));geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 0));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 1));geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 1));geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 0));
//    geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 0));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 1));geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 1));geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(1, 0, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, 1, 1));
//    geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 0));geometry->geometry[0].addWireVert(base+glm::vec3(1, 1, 1));
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    geometry->geometry[0].addWireColor(1.0,1.0,1.0);geometry->geometry[0].addWireColor(1.0,1.0,1.0);
//    BlockId anx = world->getser(x+(1<<lod),y,z);
//    if (anx!=fillvalue) {
//        geometry->geometry[0].addWireVert(base+glm::vec3(xcondat.getoffset(), 0, 0));
//        geometry->geometry[0].addWireVert(base+glm::vec3(xcondat.getoffset(), 0, 0)+xcondat.getnorm());
//        geometry->geometry[0].addWireColor(1.0, 0, 0);
//        geometry->geometry[0].addWireColor(1.0, 0, 0);
//        Xstitch(x,y,z,geometry,world,fillvalue,anx);
//    }
//    BlockId any = world->getser(x,y+(1<<lod),z);
//    if (any!=fillvalue) {
//        geometry->geometry[0].addWireVert(base+glm::vec3( 0, ycondat.getoffset(),0));
//        geometry->geometry[0].addWireVert(base+glm::vec3( 0, ycondat.getoffset(),0)+ycondat.getnorm());
//        geometry->geometry[0].addWireColor( 0, 1.0,0);
//        geometry->geometry[0].addWireColor( 0, 1.0,0);
//        Ystitch(x,y,z,geometry,world,fillvalue,any);
//    }
//    BlockId anz = world->getser(x,y,z+(1<<lod));
//    if (anz!=fillvalue) {
//        geometry->geometry[0].addWireVert(base+glm::vec3( 0, 0,zcondat.getoffset()));
//        geometry->geometry[0].addWireVert(base+glm::vec3( 0, 0,zcondat.getoffset())+zcondat.getnorm());
//        geometry->geometry[0].addWireColor( 0, 0,1.0);
//        geometry->geometry[0].addWireColor( 0, 0,1.0);
//        Zstitch(x,y,z,geometry,world,fillvalue,anz);
//    }
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
BlockId OctreeBud::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
BlockId OctreeBud::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
//void OctreeBud::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {
//    if (newid == fillvalue) {
//    } else if (recur==0) {
//        fillvalue = newid;
//    }
//    else {
//        delete self;
//        self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
//                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
//                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
//                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),recur-1);
//        self->setser(x,y,z,newid,recur,self);
//    }
//}
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
//BlockId OctreeBud::getsimpid() {return fillvalue;}
void OctreeBud::filesave(std::ostream& file) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
//bool OctreeBud::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {return false;}
OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,OctreeSegment* b,
                                                   OctreeSegment* c,OctreeSegment* d,
                                                   OctreeSegment* e,OctreeSegment* f,
                                                   OctreeSegment* g,OctreeSegment* h, int dep) : OctreeBranch(a,b,c,d,e,f,g,h,dep) {}
int OctreePortionAwareBranch::getlodat(BlockLoc x, BlockLoc y, BlockLoc z) {
    return curlod;
}
void OctreePortionAwareBranch::setlodat(BlockLoc x, BlockLoc y, BlockLoc z,int newlod) {
    curlod = newlod;
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

//void OctreeBranch::vertifyselection(BlockLoc x, BlockLoc y, BlockLoc z, int target) {
//    if (depth<target) {
//        point = getqef().evaluate(target);
//    } else {
//        subdivisions XYZINDEX->vertifyselection(x, y, z, target);
//    }
//}
void OctreeBranch::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {
    if (depth<target) {
//        int mask = CHSIZE-(1<<target);
//        if (((x&mask) != mask) and ((y&mask) != mask) and ((z&mask) != mask) and
//            ((x&mask) != 0   ) and ((y&mask) != 0   ) and ((z&mask) != 0   )) {
            point = getqef().evaluate(target);
//        }
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
int OctreeBranch::getlodat(BlockLoc x, BlockLoc y, BlockLoc z) {
    return subdivisions XYZINDEX->getlodat(x, y, z);
}
void OctreeBranch::setlodat(BlockLoc x, BlockLoc y, BlockLoc z,int newlod) {
    subdivisions XYZINDEX->setlodat(x, y, z, newlod);
}

//void OctreeBranch::drawgeometry(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,Octree* world,bool needy) {
//    extern uint8_t gtable[2][6][2];
//    extern uint8_t materialprops[];
//    int lod = depth+1;
//    uint8_t anx = world->getser(x+(1<<lod),y,z);
//    uint8_t any = world->getser(x,y+(1<<lod),z);
//    uint8_t anz = world->getser(x,y,z+(1<<lod));
//    uint8_t fillvalue = getser(0,0,0);
//    BlockLoc xo = x - (ASBLOCKLOC(0));
//    BlockLoc yo = y - (ASBLOCKLOC(0));
//    BlockLoc zo = z - (ASBLOCKLOC(0));
//    if (materialprops[fillvalue]>materialprops[anx]) {
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[fillvalue].addVert(glm::vec3(xo,yo-(gtable[1][ik][0]<<lod),zo-(gtable[1][ik][1]<<lod))+
//                                                  world->feat(x,y-(gtable[1][ik][0]<<lod),z-(gtable[1][ik][1]<<lod),lod).get(lod));
//        }
//    } else if (materialprops[fillvalue]<materialprops[anx]) {
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[anx].addVert(glm::vec3(xo,yo-(gtable[0][ik][0]<<lod),zo-(gtable[0][ik][1]<<lod))+
//                                            world->feat(x,y-(gtable[0][ik][0]<<lod),z-(gtable[0][ik][1]<<lod),lod).get(lod));
//        }
//    }
//    if (materialprops[fillvalue]>materialprops[any]) {
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[fillvalue].addVert(glm::vec3(xo-(gtable[0][ik][0]<<lod),yo,zo-(gtable[0][ik][1]<<lod))+
//                                                  world->feat(x-(gtable[0][ik][0]<<lod),y,z-(gtable[0][ik][1]<<lod),lod).get(lod));
//        }
//    } else if (materialprops[fillvalue]<materialprops[any]) {
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[any].addVert(glm::vec3(xo-(gtable[1][ik][0]<<lod),yo,zo-(gtable[1][ik][1]<<lod))+
//                                            world->feat(x-(gtable[1][ik][0]<<lod),y,z-(gtable[1][ik][1]<<lod),lod).get(lod));
//        }
//    }
//    if (materialprops[fillvalue]>materialprops[anz]) {
//        //                    std::cout<<"made a face. z";
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[fillvalue].addVert(glm::vec3(xo-(gtable[1][ik][0]<<lod),yo-(gtable[1][ik][1]<<lod),zo)+
//                                                  world->feat(x-(gtable[1][ik][0]<<lod),y-(gtable[1][ik][1]<<lod),z,lod).get(lod));
//        }
//    } else if (materialprops[fillvalue]<materialprops[anz]) {
//        for (int ik=0;ik<6;ik++) {
//            geometry->geometry[anz].addVert(glm::vec3(xo-(gtable[0][ik][0]<<lod),yo-(gtable[0][ik][1]<<lod),zo)+
//                                            world->feat(x-(gtable[0][ik][0]<<lod),y-(gtable[0][ik][1]<<lod),z,lod).get(lod));
//        }
//    }
//
//    
//}
#ifdef WIREFRAMEDEBUG
void drawdebugcube(float x,float y,float z,float a,float b,float c,GeometryOctreeLeaf* geometry,float u,float v,float w) {
    glm::vec3 base = glm::vec3(x,y,z);
    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, 0));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, b, 0));geometry->geometry[0].addWireVert(base+glm::vec3(a, b, 0));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, c));geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, b, c));geometry->geometry[0].addWireVert(base+glm::vec3(a, b, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, b, 0));
    geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(a, b, 0));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, c));geometry->geometry[0].addWireVert(base+glm::vec3(0, b, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, c));geometry->geometry[0].addWireVert(base+glm::vec3(a, b, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, 0, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, 0));geometry->geometry[0].addWireVert(base+glm::vec3(a, 0, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(0, b, 0));geometry->geometry[0].addWireVert(base+glm::vec3(0, b, c));
    geometry->geometry[0].addWireVert(base+glm::vec3(a, b, 0));geometry->geometry[0].addWireVert(base+glm::vec3(a, b, c));
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    geometry->geometry[0].addWireColor(u,v,w);geometry->geometry[0].addWireColor(u,v,w);
    
}
#endif
void OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int lod) {
    
#ifdef WIREFRAMEDEBUG
    if (connections&16) {
        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),(1<<(depth+1)),(1<<(depth+1)),(1<<(depth+1)),geometry,
                      1,1,1);
        for (int n=0;n<8;n++) {
            if (DEBUGDEBUG&(1<<n)) {
                int sx = (n&1)<<depth;
                int sy = ((n&2)>>1)<<depth;
                int sz = ((n&4)>>2)<<depth;
                drawdebugcube(x-(ASBLOCKLOC(0))+.25+sx,y-(ASBLOCKLOC(0))+.25+sy,z-(ASBLOCKLOC(0))+.25+sz,(1<<(depth))-.5,(1<<(depth))-.5,(1<<(depth))-.5,geometry,
                              1,0,0);
                
            }
        }
    }
//    if (giveXflag(x,y,z,depth-1,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0))+(1<<(depth+1)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),0,(1<<(depth+1)),(1<<(depth+1)),geometry,1,0,0);
//    }
//    if (giveYflag(x,y,z,depth-1,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0))+(1<<(depth+1)),z-(ASBLOCKLOC(0)),(1<<(depth+1)),0,(1<<(depth+1)),geometry,0,1,0);
//    }
//    if (giveZflag(x,y,z,depth-1,world)) {
//        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0))+(1<<(depth+1)),(1<<(depth+1)),(1<<(depth+1)),0,geometry,0,0,1);
//    }
#endif
    if (depth<lod) {
//        int mask = CHSIZE-(1<<lod);
//        if (((x&mask) != mask) and ((y&mask) != mask) and ((z&mask) != mask) and
        //            ((x&mask) != 0   ) and ((y&mask) != 0   ) and ((z&mask) != 0   )) {
        int lodmin = std::min(std::min(std::min(world->getlodat(x-1,y,z)  ,world->getlodat(x,y-1,z)  ),world->getlodat(x,y,z-1)  ),
                              std::min(std::min(world->getlodat(x-1,y-1,z),world->getlodat(x,y-1,z-1)),world->getlodat(x-1,y,z-1)));
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
//void OctreeBranch::geomifyselectionX(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {
//    if (depth<lod[0]) {
//        Xstitch(x,y,z,geometry,world,lod,getser(0,0,0),otherfill);
//    } else {
//        subdivisions XYZINDEX->geomifyselectionX(x,y,z,geometry,world,lod,otherfill);
//    }
//}
//void OctreeBranch::geomifyselectionY(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {
//    if (depth<lod[0]) {
//        Ystitch(x,y,z,geometry,world,lod,getser(0,0,0),otherfill);
//    } else {
//        subdivisions XYZINDEX->geomifyselectionY(x,y,z,geometry,world,lod,otherfill);
//    }
//}
//void OctreeBranch::geomifyselectionZ(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId otherfill) {
//    if (depth<lod[0]) {
//        Zstitch(x,y,z,geometry,world,lod,getser(0,0,0),otherfill);
//    } else {
//        subdivisions XYZINDEX->geomifyselectionZ(x,y,z,geometry,world,lod,otherfill);
//    }
//}
//void OctreeBranch::hermitifyselection(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
//    subdivisions XYZINDEX->hermitifyselection(x,y,z,world);
//}
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
BlockId OctreeBranch::getserwrt(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getserwrt(x,y,z);
}
//void OctreeBranch::setser(BlockLoc x,BlockLoc y,BlockLoc z,BlockId newid,int recur,OctreeSegment*& self) {
//    subdivisions XYZINDEX->setser(x,y,z,newid,recur-1,subdivisions XYZINDEX);
//}
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
//BlockId OctreeBranch::getsimpid() {return 0;}
//bool OctreeBranch::featuresque(BlockLoc x,BlockLoc y,BlockLoc z) {
//    return subdivisions XYZINDEX->featuresque(x,y,z);
//}

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
        int8_t ids[12];
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &ids,sizeof(int8_t)*12);
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
OctreeSegment* makePortionOctree(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 'l') {
        uint8_t id;
        int8_t ids[12];
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &ids,sizeof(int8_t)*12);
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
        return new OctreePortionAwareBranch(makeOctree(file,recur),
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
OctreeSegment* makePortionOctree(BlockId (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
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
                    return new OctreePortionAwareBranch(a,b,c,d,e,f,g,h,recur-1);
                }
            }
        }
    }
    return new OctreeBud(equi);
}

void Octree::loadportion(BlockLoc x,BlockLoc y,BlockLoc z,BlockId (*dat)[CHSIZE+1][CHSIZE+1]) {
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),depth-CHPOWER,makePortionOctree(dat,0,0,0,CHPOWER),data);
}


Octree::Octree(glm::mat4& trans) : realworld(trans) {}

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
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),depth-CHPOWER,makePortionOctree(file,CHPOWER),data);
    file.close();
}
bool Octree::dataexists(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    
    return boost::filesystem::exists(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)));
}
void Octree::render() {
    realworld.render();
}
bool Octree::existsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (underpressure(x<<CHPOWER,y<<CHPOWER,z<<CHPOWER)>depth) {
        return false;
    }
    return data->getser(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))!=0;
}
void Octree::h_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    
    if (existsat(x  ,y  ,z  ) and existsat(x+1,y  ,z  ) and
        existsat(x  ,y+1,z  ) and existsat(x+1,y+1,z  ) and
        existsat(x  ,y  ,z+1) and existsat(x+1,y  ,z+1) and
        existsat(x  ,y+1,z+1) and existsat(x+1,y+1,z+1)) {
        OctreeSegment* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        
        portion->hermitify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),data);
        GeometryOctreeLeaf* newguy = new GeometryOctreeLeaf(x,y,z,0);
        realworld.expand(x,y,z);
        realworld.insertinto(x,y,z,newguy);
    }
}
void Octree::v_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    
    if (existsat(x  ,y  ,z  ) and existsat(x+1,y  ,z  ) and
        existsat(x  ,y+1,z  ) and existsat(x+1,y+1,z  ) and
        existsat(x  ,y  ,z+1) and existsat(x+1,y  ,z+1) and
        existsat(x  ,y+1,z+1) and existsat(x+1,y+1,z+1)) {
        int lod = data->getlodat(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        
        OctreeSegment* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
        GeometryOctreeLeaf* newguy = realworld.getgeomat(x,y,z);
        
        if (!(newguy->bakeddetails&(1<<lod))) {
            portion->vertify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),lod);
            newguy->bakeddetails |= (1<<lod);
        }
    }
}
void Octree::g_manifest(BlockLoc x,BlockLoc y,BlockLoc z) {
    
//    data->setlodat(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),lod);
    
//    if (newguy->currentlod!=-1) {
//        
//    }
    if (realworld.existsat(x-1,y-1,z-1) and realworld.existsat(x  ,y-1,z-1) and
        realworld.existsat(x-1,y  ,z-1) and realworld.existsat(x  ,y  ,z-1) and
        realworld.existsat(x-1,y-1,z  ) and realworld.existsat(x  ,y-1,z  ) and
        realworld.existsat(x-1,y  ,z  ) and realworld.existsat(x  ,y  ,z  ) ){
        GeometryOctreeLeaf* newguy = realworld.getgeomat(x,y,z);
        
        const int lodlookup[7][3] = {{0,0,0},{-1,0,0},{0,-1,0},{-1,-1,0},{0,0,-1},{-1,0,-1},{0,-1,-1}};
        bool needchange = false;
        for (int i=0;i<7;i++) {
            int lod = data->getlodat(ASCHUNKLOC(x+lodlookup[i][0]),ASCHUNKLOC(y+lodlookup[i][1]),ASCHUNKLOC(z+lodlookup[i][2]));
            if (newguy->lodserial[i] !=lod) {
                newguy->lodserial[i]=lod;
                needchange=true;
            }
        }
        if (needchange) {
            int lod = data->getlodat(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
            OctreeSegment* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
            
            newguy->open = true;
            newguy->erase();
    //        newguy->removeseal();
            portion->geomify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),newguy,data,lod);
        //    realworld.data->insertinto(realworld.flipbits(x),realworld.flipbits(y),realworld.flipbits(z),realworld.depth,newguy,realworld.data);
            newguy->matrixmap(&realworld.matrix);
//            newguy->securecore();
            newguy->open = false;
            
        }
        
//        if (data->getlodat(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))==-1) {
//            data->setlodat(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),lod);
//            OctreeSegment* portion = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
//            GeometryOctreeLeaf* newguy = realworld.getgeomat(x,y,z);
//            newguy->open = true;
//            newguy->erase();
//    //        newguy->removeseal();
//            portion->geomify(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),newguy,data,lod);
//        //    realworld.data->insertinto(realworld.flipbits(x),realworld.flipbits(y),realworld.flipbits(z),realworld.depth,newguy,realworld.data);
//            newguy->matrixmap(&realworld.matrix);
//            newguy->securecore();
//            newguy->open = false;
//        }
    }
//    for(auto iterator = newguy->geometry.begin(); iterator != newguy->geometry.end(); iterator++) {
//        iterator->second.matrix = &realworld.matrix;
//        std::cout<<"created geometry with material "<<(int)iterator->first<<"\n";
//    }
//    std::cout<<"finished generation.\n";

}
//
//void Octree::firstpreparevoxXskirt
//void Octree::voxXskirt(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geo) {
//    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->hermitifyselection(ASCHUNKLOC(x+1)-1,yi,zi,data);
//            data->vertifyselection(ASCHUNKLOC(x+1)-1,yi,zi)
//        }
//    }
//    for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->geomifyselection(ASCHUNKLOC(x+1)-1,yi,zi,geo,data,0);
//            data->geomifyselection(ASCHUNKLOC(x+1),yi,zi,geo,data,0);
//        }
//    }
//}
//void Octree::voxYskirt(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geo) {
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->hermitifyselection(xi,ASCHUNKLOC(y+1)-1,zi,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int zi=ASCHUNKLOC(z)+1;zi<ASCHUNKLOC(z+1)-1;zi++) {
//            data->geomifyselection(xi,ASCHUNKLOC(y+1)-1,zi,geo,data,0);
//            data->geomifyselection(xi,ASCHUNKLOC(y+1),zi,geo,data,0);
//        }
//    }
//}
//void Octree::voxZskirt(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geo) {
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
//            data->hermitifyselection(xi,yi,ASCHUNKLOC(z+1)-1,data);
//        }
//    }
//    for (int xi=ASCHUNKLOC(x)+1;xi<ASCHUNKLOC(x+1)-1;xi++) {
//        for (int yi=ASCHUNKLOC(y)+1;yi<ASCHUNKLOC(y+1)-1;yi++) {
//            data->geomifyselection(xi,yi,ASCHUNKLOC(z+1)-1,geo,data,0);
//            data->geomifyselection(xi,yi,ASCHUNKLOC(z+1),geo,data,0);
//        }
//    }
//}

//GeometryOctreeLeaf* Octree::opengeoforediting(BlockLoc x,BlockLoc y,BlockLoc z) {
//    GeometryOctreeLeaf* geo = realworld.getgeomat(x,y,z);
//    geo->open = true;
//    opengeos.push_back(geo);
//    return geo;
//}
//void Octree::cleanupedits() {
//    for (int i = 0;i<opengeos.size();i++) {
//        opengeos[i]->matrixmap(&realworld.matrix);
//        opengeos[i]->open = false;
//    }
//    opengeos.clear();
//}
//void Octree::voxsnippets(BlockLoc x,BlockLoc y,BlockLoc z) {
//
//#define SNAP(x,y) (x&(BLOCKLOCFULLMASK^((1<<y)-1)))
//#define geoget(x,y,z) GeometryOctreeLeaf* geo=realworld.getgeomat(x,y,z);if(!geo->open){touchedgeoms.push_back(geo);geo->open=true;}BlockLoc xw=x;BlockLoc yw=y;BlockLoc zw=z;int orilod = geo->currentlod;
//#define giterX for (int xi=ASCHUNKLOC(xw)+(1<<minyamum);xi<ASCHUNKLOC(xw+1)-(1<<orilod);xi+=(1<<minyamum)) {
//#define giterY for (int yi=ASCHUNKLOC(yw)+(1<<minyamum);yi<ASCHUNKLOC(yw+1)-(1<<orilod);yi+=(1<<minyamum)) {
//#define giterZ for (int zi=ASCHUNKLOC(zw)+(1<<minyamum);zi<ASCHUNKLOC(zw+1)-(1<<orilod);zi+=(1<<minyamum)) {
//#define iterX for (int xi=ASCHUNKLOC(xw);xi<ASCHUNKLOC(xw+1)-1;xi++) {
//#define iterY for (int yi=ASCHUNKLOC(yw);yi<ASCHUNKLOC(yw+1)-1;yi++) {
//#define iterZ for (int zi=ASCHUNKLOC(zw);zi<ASCHUNKLOC(zw+1)-1;zi++) {
//#define maxX ASCHUNKLOC(xw+1)-1
//#define maxY ASCHUNKLOC(yw+1)-1
//#define maxZ ASCHUNKLOC(zw+1)-1
//#define ifmask(f,lod) ( !((bool) (f&((1<<lod)-1))  ) )
////#define WA ((1<<otherlod)-(1<<orilod))
//    
//    //#define INTLODONE
////    int otherlod = realworld.getgeomat(x,y,z)->currentlod;
////    if (otherlod != 1) {return;}
//    std::vector<GeometryOctreeLeaf*> touchedgeoms;
//    for (int xo=0;xo<2;xo++) {
//        if (existsat(x-1+(xo*2), y, z)) {
//            geoget(x-1+xo,y,z)
//            int otherlod = realworld.getgeomat(x+xo,y,z)->currentlod;
//            int lod[] = {otherlod,orilod,otherlod,orilod,otherlod,orilod,otherlod,orilod};
//            int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//            iterY iterZ data->hermitifyselection(maxX,yi,zi,data);}}
//            iterY iterZ if (ifmask(yi,orilod) and ifmask(zi,otherlod)) {data->vertifyselection(maxX,yi,zi,orilod);}}}
//            int minyamum = std::min(otherlod,orilod);
//            int maxyamum = std::max(otherlod,orilod);
//            giterY giterZ
//                    int anchor = data->getser(SNAP(maxX,orilod),SNAP(yi,orilod),SNAP(zi,orilod));
//
//                    Xstitch(SNAP(maxX,orilod),yi,zi,geo,data, lodone,anchor,
//                            data->getser(maxX+1           ,SNAP(yi,otherlod)            ,SNAP(zi,otherlod)));
//                    Ystitch(SNAP(maxX,orilod),yi,zi,geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,orilod),SNAP(yi+(1<<minyamum),orilod),SNAP(zi,orilod)));
//                    Zstitch(SNAP(maxX,orilod),yi,zi,geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,orilod),SNAP(yi,orilod)              ,SNAP(zi+(1<<minyamum),orilod)));
//
//                    int anchor2 = data->getser(maxX+1,SNAP(yi,otherlod),SNAP(zi,otherlod));
//                    //
//                    Xstitch(maxX+1,yi,zi,geo,data, lod,anchor2,
//                            data->getser(maxX+1+(1<<otherlod),SNAP(yi,otherlod)              ,SNAP(zi,otherlod)));
//                    Ystitch(maxX+1,yi,zi,geo,data, lod,anchor2,
//                            data->getser(maxX+1              ,SNAP(yi+(1<<minyamum),otherlod),SNAP(zi,otherlod)));
//                    Zstitch(maxX+1,yi,zi,geo,data, lod,anchor2,
//                            data->getser(maxX+1              ,SNAP(yi,otherlod)              ,SNAP(zi+(1<<minyamum),otherlod)));
//            }}
//        }
//    }
//    for (int yo=0;yo<2;yo++) {
//        if (existsat(x, y-1+(yo*2), z)) {
//            geoget(x,y-1+yo,z)
//            int otherlod = realworld.getgeomat(x,y+yo,z)->currentlod;
//            int lod[] = {otherlod,otherlod,orilod,orilod,otherlod,otherlod,orilod,orilod};
//            int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//            iterX iterZ data->hermitifyselection(xi,maxY,zi,data);}}
//            iterX iterZ if (ifmask(xi,orilod) and ifmask(zi,otherlod)) {data->vertifyselection(xi,maxY,zi,orilod);}}}
//            int minyamum = std::min(otherlod,orilod);
//            int maxyamum = std::max(otherlod,orilod);
//            giterX giterZ
//                int anchor = data->getser(SNAP(xi,orilod),SNAP(maxY,orilod),SNAP(zi,orilod));
//
//                Ystitch(xi,SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                        data->getser(SNAP(xi,otherlod)            ,maxY+1           ,SNAP(zi,otherlod)));
//                Xstitch(xi,SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                        data->getser(SNAP(xi+(1<<minyamum),orilod),SNAP(maxY,orilod),SNAP(zi,orilod)));
//                Zstitch(xi,SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                        data->getser(SNAP(xi,orilod)              ,SNAP(maxY,orilod),SNAP(zi+(1<<minyamum),orilod)));
//
//                int anchor2 = data->getser(SNAP(xi,otherlod),maxY+1,SNAP(zi,otherlod));
//                //
//                Ystitch(xi,maxY+1,zi,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi,otherlod)              ,maxY+1+(1<<otherlod),SNAP(zi,otherlod)));
//                Xstitch(xi,maxY+1,zi,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi+(1<<minyamum),otherlod),maxY+1              ,SNAP(zi,otherlod)));
//                Zstitch(xi,maxY+1,zi,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi,otherlod)              ,maxY+1              ,SNAP(zi+(1<<minyamum),otherlod)));
//            }}
//        }
//    }
//    for (int zo=0;zo<2;zo++) {
//        if (existsat(x, y, z-1+(zo*2))) {
//            geoget(x,y,z-1+zo)
//            int otherlod = realworld.getgeomat(x,y,z+zo)->currentlod;
//            int lod[] = {otherlod,otherlod,otherlod,otherlod,orilod,orilod,orilod,orilod};
//            int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//            iterX iterY data->hermitifyselection(xi,yi,maxZ,data);}}
//            iterX iterY if (ifmask(xi,orilod) and ifmask(yi,otherlod)) {data->vertifyselection(xi,yi,maxZ,orilod);}}}
//            int minyamum = std::min(otherlod,orilod);
//            int maxyamum = std::max(otherlod,orilod);
//            giterX giterY
//                int anchor = data->getser(SNAP(xi,orilod),SNAP(yi,orilod),SNAP(maxZ,orilod));
//
//                Zstitch(xi,yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                        data->getser(SNAP(xi,otherlod)            ,SNAP(yi,otherlod)            ,maxZ+1           ));
//                Xstitch(xi,yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                        data->getser(SNAP(xi+(1<<minyamum),orilod),SNAP(yi,orilod)              ,SNAP(maxZ,orilod)));
//                Ystitch(xi,yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                        data->getser(SNAP(xi,orilod)              ,SNAP(yi+(1<<minyamum),orilod),SNAP(maxZ,orilod)));
//
//                int anchor2 = data->getser(SNAP(xi,otherlod),SNAP(yi,otherlod),maxZ+1);
//
//                Zstitch(xi,yi,maxZ+1,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi,otherlod)              ,SNAP(yi,otherlod)              ,maxZ+1+(1<<otherlod)));
//                Xstitch(xi,yi,maxZ+1,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi+(1<<minyamum),otherlod),SNAP(yi,otherlod)              ,maxZ+1              ));
//                Ystitch(xi,yi,maxZ+1,geo,data, lod,anchor2,
//                        data->getser(SNAP(xi,otherlod)              ,SNAP(yi+(1<<minyamum),otherlod),maxZ+1              ));
//            }}
//        }
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
//                geoget(x,y+yo1,z+zo1)
//                int otherlody  = realworld.getgeomat(x,y+yo1+1,z+zo1)->currentlod;
//                int otherlodz  = realworld.getgeomat(x,y+yo1,z+zo1+1)->currentlod;
//                int otherlodyz = realworld.getgeomat(x,y+yo1+1,z+zo1+1)->currentlod;
//                int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//                int lody[]  = {otherlody,otherlody,orilod,orilod,otherlody,otherlody,orilod,orilod};
//                int lodz[]  = {otherlodz,otherlodz,otherlodz,otherlodz,orilod,orilod,orilod,orilod};
//                int lodyz[] = {otherlodyz,otherlodyz,otherlodz,otherlodz,otherlody,otherlody,orilod,orilod};
//                iterX data->hermitifyselection(xi,maxY,maxZ,data);}
//                iterX if (ifmask(xi,orilod)) {data->vertifyselection(xi,maxY,maxZ,orilod);}}
//                int minyamum = std::min(std::min(otherlody,orilod),std::min(otherlodz,otherlodyz));
//                int maxyamum = std::max(std::max(otherlody,orilod),std::max(otherlodz,otherlodyz));
//                giterX
//                    int anchor = data->getser(SNAP(xi,orilod),SNAP(maxY,orilod),SNAP(maxZ,orilod));
//        
//                    Xstitch(xi,SNAP(maxY,orilod),SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(SNAP(xi+(1<<minyamum),orilod),SNAP(maxY,orilod)            ,SNAP(maxZ,orilod)));
//                    Ystitch(xi,SNAP(maxY,orilod),SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(SNAP(xi,otherlody)           ,maxY+1                       ,SNAP(maxZ,otherlody)));
//                    Zstitch(xi,SNAP(maxY,orilod),SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(SNAP(xi,otherlodz)           ,SNAP(maxY,otherlodz)         ,maxZ+1           ));
//        
//                    int anchor2 = data->getser(SNAP(xi,otherlody),maxY+1,SNAP(maxZ,otherlody));
//        
//                    Xstitch(xi,maxY+1,SNAP(maxZ,orilod),geo,data, lody,anchor2,
//                            data->getser(SNAP(xi+(1<<minyamum),otherlody),maxY+1                       ,SNAP(maxZ,otherlody)));
//                    Ystitch(xi,maxY+1,SNAP(maxZ,orilod),geo,data, lody,anchor2,
//                            data->getser(SNAP(xi,otherlody)              ,maxY+1+(1<<otherlody)        ,SNAP(maxZ,otherlody)));
//                    Zstitch(xi,maxY+1,SNAP(maxZ,orilod),geo,data, lody,anchor2,
//                            data->getser(SNAP(xi,otherlodyz)             ,maxY+1                       ,maxZ+1           ));
//                    
//                    int anchor3 = data->getser(SNAP(xi,otherlodz),SNAP(maxY,otherlodz),maxZ+1);
//        
//                    Xstitch(xi,SNAP(maxY,orilod),maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(SNAP(xi+(1<<minyamum),otherlodz),SNAP(maxY,otherlodz)         ,maxZ+1               ));
//                    Ystitch(xi,SNAP(maxY,orilod),maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(SNAP(xi,otherlodyz)             ,maxY+1                       ,maxZ+1               ));
//                    Zstitch(xi,SNAP(maxY,orilod),maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(SNAP(xi,otherlodz)              ,SNAP(maxY,otherlodz)         ,maxZ+1+(1<<otherlodz)));
//                    
//                    int anchor4 = data->getser(SNAP(xi,otherlodyz),maxY+1,maxZ+1);
//        
//                    Xstitch(xi,maxY+1,maxZ+1,geo,data, lodyz,anchor4,
//                            data->getser(SNAP(xi+(1<<minyamum),otherlodyz),maxY+1                       ,maxZ+1                ));
//                    Ystitch(xi,maxY+1,maxZ+1,geo,data, lodyz,anchor4,
//                            data->getser(SNAP(xi,otherlodyz)              ,maxY+1+(1<<otherlodyz)       ,maxZ+1                ));
//                    Zstitch(xi,maxY+1,maxZ+1,geo,data, lodyz,anchor4,
//                            data->getser(SNAP(xi,otherlodyz)              ,maxY+1                       ,maxZ+1+(1<<otherlodyz)));
//                }
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
//                geoget(x+xo1,y,z+zo1)
//                int otherlodx = realworld.getgeomat(x+xo1+1,y,z+zo1)->currentlod;
//                int otherlodz = realworld.getgeomat(x+xo1,y,z+zo1+1)->currentlod;
//                int otherlodxz = realworld.getgeomat(x+xo1+1,y,z+zo1+1)->currentlod;
//                int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//                int lodx[] = {otherlodx,orilod,otherlodx,orilod,otherlodx,orilod,otherlodx,orilod};
//                int lodz[] = {otherlodz,otherlodz,otherlodz,otherlodz,orilod,orilod,orilod,orilod};
//                int lodxz[] = {otherlodxz,otherlodz,otherlodxz,otherlodz,otherlodx,orilod,otherlodx,orilod};
//                iterY data->hermitifyselection(maxX,yi,maxZ,data);}
//                iterY if (ifmask(yi,orilod)) {data->vertifyselection(maxX,yi,maxZ,orilod);}}
//                int minyamum = std::min(std::min(otherlodx,orilod),std::min(otherlodz,otherlodxz));
//                int maxyamum = std::max(std::max(otherlodx,orilod),std::max(otherlodz,otherlodxz));
//                giterY
//                    int anchor = data->getser(SNAP(maxX,orilod),SNAP(yi,orilod),SNAP(maxZ,orilod));
//                    
//                    Xstitch(SNAP(maxX,orilod),yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(maxX+1                       ,SNAP(yi,otherlodx)           ,SNAP(maxZ,otherlodx)));
//                    Ystitch(SNAP(maxX,orilod),yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,orilod)            ,SNAP(yi+(1<<minyamum),orilod),SNAP(maxZ,orilod)));
//                    Zstitch(SNAP(maxX,orilod),yi,SNAP(maxZ,orilod),geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,otherlodz)         ,SNAP(yi,otherlodz)           ,maxZ+1           ));
//                    
//                    int anchor2 = data->getser(maxX+1,SNAP(yi,otherlodx),SNAP(maxZ,otherlodx));
//                    
//                    Xstitch(maxX+1,yi,SNAP(maxZ,orilod),geo,data, lodx,anchor2,
//                            data->getser(maxX+1+(1<<otherlodx)        ,SNAP(yi,otherlodx)              ,SNAP(maxZ,otherlodx)));
//                    Ystitch(maxX+1,yi,SNAP(maxZ,orilod),geo,data, lodx,anchor2,
//                            data->getser(maxX+1                       ,SNAP(yi+(1<<minyamum),otherlodx),SNAP(maxZ,otherlodx)));
//                    Zstitch(maxX+1,yi,SNAP(maxZ,orilod),geo,data, lodx,anchor2,
//                            data->getser(maxX+1                       ,SNAP(yi,otherlodxz)             ,maxZ+1           ));
//                    
//                    int anchor3 = data->getser(SNAP(maxX,otherlodz),SNAP(yi,otherlodz),maxZ+1);
//                    
//                    Xstitch(SNAP(maxX,orilod),yi,maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(maxX+1                       ,SNAP(yi,otherlodxz)             ,maxZ+1               ));
//                    Ystitch(SNAP(maxX,orilod),yi,maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(SNAP(maxX,otherlodz)         ,SNAP(yi+(1<<minyamum),otherlodz),maxZ+1               ));
//                    Zstitch(SNAP(maxX,orilod),yi,maxZ+1,geo,data, lodz,anchor3,
//                            data->getser(SNAP(maxX,otherlodz)         ,SNAP(yi,otherlodz)              ,maxZ+1+(1<<otherlodz)));
//                    
//                    int anchor4 = data->getser(maxX+1,SNAP(yi,otherlodxz),maxZ+1);
//                    
//                    Xstitch(maxX+1,yi,maxZ+1,geo,data, lodxz,anchor4,
//                            data->getser(maxX+1+(1<<otherlodxz)       ,SNAP(yi,otherlodxz)              ,maxZ+1                ));
//                    Ystitch(maxX+1,yi,maxZ+1,geo,data, lodxz,anchor4,
//                            data->getser(maxX+1                       ,SNAP(yi+(1<<minyamum),otherlodxz),maxZ+1                ));
//                    Zstitch(maxX+1,yi,maxZ+1,geo,data, lodxz,anchor4,
//                            data->getser(maxX+1                       ,SNAP(yi,otherlodxz)              ,maxZ+1+(1<<otherlodxz)));
//                }
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
//                geoget(x+xo1,y+yo1,z)
//                int otherlodx  = realworld.getgeomat(x+xo1+1,y+yo1,z)->currentlod;
//                int otherlody  = realworld.getgeomat(x+xo1,y+yo1+1,z)->currentlod;
//                int otherlodxy = realworld.getgeomat(x+xo1+1,y+yo1+1,z)->currentlod;
//                int lodone[] = {orilod,orilod,orilod,orilod,orilod,orilod,orilod,orilod};
//                int lodx[]  = {otherlodx,orilod,otherlodx,orilod,otherlodx,orilod,otherlodx,orilod};
//                int lody[]  = {otherlody,otherlody,orilod,orilod,otherlody,otherlody,orilod,orilod};
//                int lodxy[] = {otherlodxy,otherlody,otherlodx,orilod,otherlodxy,otherlody,otherlodx,orilod};
//                iterZ data->hermitifyselection(maxX,maxY,zi,data);}
//                iterZ if (ifmask(zi,orilod)) {data->vertifyselection(maxX,maxY,zi,orilod);}}
//                int minyamum = std::min(std::min(otherlodx,orilod),std::min(otherlody,otherlodxy));
//                int maxyamum = std::max(std::max(otherlodx,orilod),std::max(otherlody,otherlodxy));
//                giterZ
//                    int anchor = data->getser(SNAP(maxX,orilod),SNAP(maxY,orilod),SNAP(zi,orilod));
//                    
//                    Xstitch(SNAP(maxX,orilod),SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                            data->getser(maxX+1                       ,SNAP(maxY,otherlodx),SNAP(zi,otherlodx)           ));
//                    Ystitch(SNAP(maxX,orilod),SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,otherlody)         ,maxY+1              ,SNAP(zi,otherlody)           ));
//                    Zstitch(SNAP(maxX,orilod),SNAP(maxY,orilod),zi,geo,data, lodone,anchor,
//                            data->getser(SNAP(maxX,orilod)            ,SNAP(maxY,orilod)   ,SNAP(zi+(1<<minyamum),orilod)));
//                    
//                    int anchor2 = data->getser(maxX+1,SNAP(maxY,otherlodx),SNAP(zi,otherlodx));
//                    
//                    Xstitch(maxX+1,SNAP(maxY,orilod),zi,geo,data, lodx,anchor2,
//                            data->getser(maxX+1+(1<<otherlodx)        ,SNAP(maxY,otherlodx),SNAP(zi,otherlodx)              ));
//                    Ystitch(maxX+1,SNAP(maxY,orilod),zi,geo,data, lodx,anchor2,
//                            data->getser(maxX+1                       ,maxY+1              ,SNAP(zi,otherlodxy)             ));
//                    Zstitch(maxX+1,SNAP(maxY,orilod),zi,geo,data, lodx,anchor2,
//                            data->getser(maxX+1                       ,SNAP(maxY,otherlodx),SNAP(zi+(1<<minyamum),otherlodx)));
//                    
//                    int anchor3 = data->getser(SNAP(maxX,otherlody),maxY+1,SNAP(zi,otherlody));
//                    
//                    Xstitch(SNAP(maxX,orilod),maxY+1,zi,geo,data, lody,anchor3,
//                            data->getser(maxX+1                       ,maxY+1               ,SNAP(zi,otherlodxy)             ));
//                    Ystitch(SNAP(maxX,orilod),maxY+1,zi,geo,data, lody,anchor3,
//                            data->getser(SNAP(maxX,otherlody)         ,maxY+1+(1<<otherlody),SNAP(zi,otherlody)              ));
//                    Zstitch(SNAP(maxX,orilod),maxY+1,zi,geo,data, lody,anchor3,
//                            data->getser(SNAP(maxX,otherlody)         ,maxY+1               ,SNAP(zi+(1<<minyamum),otherlody)));
//                    
//                    int anchor4 = data->getser(maxX+1,maxY+1,SNAP(zi,otherlodxy));
//                    
//                    Xstitch(maxX+1,maxY+1,zi,geo,data, lodxy,anchor4,
//                            data->getser(maxX+1+(1<<otherlodxy)       ,maxY+1                ,SNAP(zi,otherlodxy)              ));
//                    Ystitch(maxX+1,maxY+1,zi,geo,data, lodxy,anchor4,
//                            data->getser(maxX+1                       ,maxY+1+(1<<otherlodxy),SNAP(zi,otherlodxy)              ));
//                    Zstitch(maxX+1,maxY+1,zi,geo,data, lodxy,anchor4,
//                            data->getser(maxX+1                       ,maxY+1                ,SNAP(zi+(1<<minyamum),otherlodxy)));
//                }
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
//    for (int i = 0;i<touchedgeoms.size();i++) {
//        touchedgeoms[i]->matrixmap(&realworld.matrix);
//        touchedgeoms[i]->open = false;
//        touchedgeoms[i]->removeseal();
//    }
//#undef geoget
//}

void Xstitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anx) {
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
                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {//if (materialprops[fillvalue]<materialprops[anx]) {
            for (int ik=0;ik<6;ik++) {
                geometry->geometry[anx].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}
void Ystitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,BlockId fillvalue,BlockId any) {
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
                geometry->geometry[fillvalue].addVert(buf[gtable[0][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[any]) {
            for (int ik=0;ik<6;ik++) {
                geometry->geometry[any].addVert(buf[gtable[1][ik]]);
            }
        }
    }
}
void Zstitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anz) {
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
                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[anz]) {
            for (int ik=0;ik<6;ik++) {
                geometry->geometry[anz].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}
//void drawgeometry(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int lod,BlockId fillvalue) {
//    extern uint8_t gtable[2][6];
//    extern uint8_t materialprops[];
//    uint8_t anx = world->getser(x+(1<<lod),y,z);
//    uint8_t any = world->getser(x,y+(1<<lod),z);
//    uint8_t anz = world->getser(x,y,z+(1<<lod));
//    if (materialprops[fillvalue]!=materialprops[anx]) {
//        glm::vec3 buf[] = {
//            world->featwrt(x,y  ,z  ,lod),
//            world->featwrt(x,y-1,z  ,lod),
//            world->featwrt(x,y  ,z-1,lod),
//            world->featwrt(x,y-1,z-1,lod)
//        };
//        if (materialprops[fillvalue]>materialprops[anx]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
//            }
//        } else {//if (materialprops[fillvalue]<materialprops[anx]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[anx].addVert(buf[gtable[0][ik]]);
//            }
//        }
//    }
//    if (materialprops[fillvalue]!=materialprops[any]) {
//        glm::vec3 buf[] = {
//            world->featwrt(x  ,y,z  ,lod),
//            world->featwrt(x-1,y,z  ,lod),
//            world->featwrt(x  ,y,z-1,lod),
//            world->featwrt(x-1,y,z-1,lod)
//        };
//        if (materialprops[fillvalue]>materialprops[any]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[0][ik]]);
//            }
//        } else {// if (materialprops[fillvalue]<materialprops[any]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[any].addVert(buf[gtable[1][ik]]);
//            }
//        }
//    }
//    if (materialprops[fillvalue]!=materialprops[anz]) {
//        glm::vec3 buf[] = {
//            world->featwrt(x  ,y  ,z,lod),
//            world->featwrt(x-1,y  ,z,lod),
//            world->featwrt(x  ,y-1,z,lod),
//            world->featwrt(x-1,y-1,z,lod)
//        };
//        if (materialprops[fillvalue]>materialprops[anz]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
//            }
//        } else {// if (materialprops[fillvalue]<materialprops[anz]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[anz].addVert(buf[gtable[0][ik]]);
//            }
//        }
//    }
//}






//#define AV2(x,y) OIFY(SNAP(x,y))
//#define AB0(x,y) (SNAP(x,y))
//#define AB1(x,y) (SNAP(x-1,y))
//#define ABX(x,y) (AB1(x,y))
//#define ABY(x,y) (AB1(x,y))
//#define ABZ(x,y) (AB1(x,y))
//
//#define OIFY(x) (x-(ASBLOCKLOC(0)))
//#define AV0(x,y) OIFY(AB0(x,y))
//#define AVX(x,y) OIFY(ABX(x,y))
//#define AVY(x,y) OIFY(ABY(x,y))
//#define AVZ(x,y) OIFY(ABZ(x,y))
//
//void Xstitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId fillvalue,BlockId otherfill) {
//    extern uint8_t gtable[2][6];
//    extern uint8_t materialprops[];
//    if (materialprops[fillvalue]!=materialprops[otherfill]) {
//        glm::vec3 buf[] = {
//            glm::vec3(AV0(x,lod[0]),AV0(y,lod[0]),AV0(z,lod[0]))+world->feat(AB0(x,lod[0]),AB0(y,lod[0]),AB0(z,lod[0]),lod[0]).get(lod[0]),
//            glm::vec3(AV0(x,lod[2]),AVY(y,lod[2]),AV0(z,lod[2]))+world->feat(AB0(x,lod[2]),ABY(y,lod[2]),AB0(z,lod[2]),lod[2]).get(lod[2]),
//            glm::vec3(AV0(x,lod[4]),AV0(y,lod[4]),AVZ(z,lod[4]))+world->feat(AB0(x,lod[4]),AB0(y,lod[4]),ABZ(z,lod[4]),lod[4]).get(lod[4]),
//            glm::vec3(AV0(x,lod[6]),AVY(y,lod[6]),AVZ(z,lod[6]))+world->feat(AB0(x,lod[6]),ABY(y,lod[6]),ABZ(z,lod[6]),lod[6]).get(lod[6])
//        };
//        if (materialprops[fillvalue]>materialprops[otherfill]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
//            }
//        } else {//if (materialprops[fillvalue]<materialprops[anx]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[otherfill].addVert(buf[gtable[0][ik]]);
//            }
//        }
//    }
//}
//void Ystitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId fillvalue,BlockId otherfill) {
//    extern uint8_t gtable[2][6];
//    extern uint8_t materialprops[];
//    if (materialprops[fillvalue]!=materialprops[otherfill]) {
//        glm::vec3 buf[] = {
//            glm::vec3(AV0(x,lod[0]),AV0(y,lod[0]),AV0(z,lod[0]))+world->feat(AB0(x,lod[0]),AB0(y,lod[0]),AB0(z,lod[0]),lod[0]).get(lod[0]),
//            glm::vec3(AVX(x,lod[1]),AV0(y,lod[1]),AV0(z,lod[1]))+world->feat(ABX(x,lod[1]),AB0(y,lod[1]),AB0(z,lod[1]),lod[1]).get(lod[1]),
//            glm::vec3(AV0(x,lod[4]),AV0(y,lod[4]),AVZ(z,lod[4]))+world->feat(AB0(x,lod[4]),AB0(y,lod[4]),ABZ(z,lod[4]),lod[4]).get(lod[4]),
//            glm::vec3(AVX(x,lod[5]),AV0(y,lod[5]),AVZ(z,lod[5]))+world->feat(ABX(x,lod[5]),AB0(y,lod[5]),ABZ(z,lod[5]),lod[5]).get(lod[5])
//        };
//        if (materialprops[fillvalue]>materialprops[otherfill]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[0][ik]]);
//            }
//        } else {// if (materialprops[fillvalue]<materialprops[any]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[otherfill].addVert(buf[gtable[1][ik]]);
//            }
//        }
//    }
//}
//void Zstitch(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId fillvalue,BlockId otherfill) {
//    extern uint8_t gtable[2][6];
//    extern uint8_t materialprops[];
//    if (materialprops[fillvalue]!=materialprops[otherfill]) {
//        glm::vec3 buf[] = {
//            glm::vec3(AV0(x,lod[0]),AV0(y,lod[0]),AV0(z,lod[0]))+world->feat(AB0(x,lod[0]),AB0(y,lod[0]),AB0(z,lod[0]),lod[0]).get(lod[0]),
//            glm::vec3(AVX(x,lod[1]),AV0(y,lod[1]),AV0(z,lod[1]))+world->feat(ABX(x,lod[1]),AB0(y,lod[1]),AB0(z,lod[1]),lod[1]).get(lod[1]),
//            glm::vec3(AV0(x,lod[2]),AVY(y,lod[2]),AV0(z,lod[2]))+world->feat(AB0(x,lod[2]),ABY(y,lod[2]),AB0(z,lod[2]),lod[2]).get(lod[2]),
//            glm::vec3(AVX(x,lod[3]),AVY(y,lod[3]),AV0(z,lod[3]))+world->feat(ABX(x,lod[3]),ABY(y,lod[3]),AB0(z,lod[3]),lod[3]).get(lod[3])
//        };
//        if (materialprops[fillvalue]>materialprops[otherfill]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[fillvalue].addVert(buf[gtable[1][ik]]);
//            }
//        } else {// if (materialprops[fillvalue]<materialprops[anz]) {
//            for (int ik=0;ik<6;ik++) {
//                geometry->geometry[otherfill].addVert(buf[gtable[0][ik]]);
//            }
//        }
//    }
//}
//void drawgeometry(BlockLoc x,BlockLoc y,BlockLoc z,GeometryOctreeLeaf* geometry,OctreeSegment* world,int* lod,BlockId fillvalue) {
//
//    extern uint8_t gtable[2][6];
//    extern uint8_t materialprops[];
////    int antimask = ((1<<lod[0])-1);
////    int mask = (BLOCKLOCFULLMASK^((1<<lod[0])-1));
////    int xmask = (BLOCKLOCFULLMASK^((1<<lod[8])-1));
////    int ymask = (BLOCKLOCFULLMASK^((1<<lod[9])-1));
////    int zmask = (BLOCKLOCFULLMASK^((1<<lod[10])-1));
////#define MASK(f) (f&mask)
////#define XMASK(f) (f&xmask)
////#define YMASK(f) (f&ymask)
////#define ZMASK(f) (f&zmask)
////    uint8_t anx = world->getser((x|((1<<lod[7])-1))+1,y,z);
////    uint8_t any = world->getser(x,(y|((1<<lod[7])-1))+1,z);
////    uint8_t anz = world->getser(x,y,(z|((1<<lod[7])-1))+1);
//
//    uint8_t anx = world->getser(SNAP(x,lod[0])+(1<<lod[0]),SNAP(y,lod[8]),SNAP(z,lod[8]));
//    uint8_t any = world->getser(SNAP(x,lod[9]),SNAP(y,lod[0])+(1<<lod[0]),SNAP(z,lod[9]));
//    uint8_t anz = world->getser(SNAP(x,lod[10]),SNAP(y,lod[10]),SNAP(z,lod[0])+(1<<lod[0]));
////    uint8_t anx = world->getser(x+(1<<lod[7]),y,z);
////    uint8_t any = world->getser(x,y+(1<<lod[7]),z);
////    uint8_t anz = world->getser(x,y,z+(1<<lod[7]));
////    BlockLoc xo = SNAP(x,lod[0]) - (ASBLOCKLOC(0));
////    BlockLoc yo = SNAP(y,lod[0]) - (ASBLOCKLOC(0));
////    BlockLoc zo = SNAP(z,lod[0]) - (ASBLOCKLOC(0));
////    std::cout<<lod[0];
////#define AB2(x,y) (SNAP(x,y))
//    
//
//
//    try {
//    } catch (...) {}
//    try {
//    } catch (...) {}
//    try {
//    } catch (...) {}
//}




