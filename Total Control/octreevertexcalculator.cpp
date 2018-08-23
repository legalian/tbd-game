//
//  octreevertexcalculator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"

Edgedat::Edgedat() {}
Edgedat::Edgedat(uint8_t tc,int8_t xc,int8_t yc,int8_t zc) : x(xc),y(yc),z(zc),t(tc) {}
Edgedat::Edgedat(float xi,float yi,float zi,float ti) {
    x = (int8_t)128*xi;
    y = (int8_t)128*yi;
    z = (int8_t)128*zi;
    t = (uint8_t)255*ti;
}
MatrixCarriage Edgedat::calcxqef()  {return calcqef(1*(t/255.0)*(x/128.0),(t/255.0),0,0);}
MatrixCarriage Edgedat::calcyqef()  {return calcqef(1*(t/255.0)*(y/128.0),0,(t/255.0),0);}
MatrixCarriage Edgedat::calczqef()  {return calcqef(1*(t/255.0)*(z/128.0),0,0,(t/255.0));}
glm::vec3      Edgedat::getnorm()   {return glm::vec3(x/128.0,y/128.0,z/128.0);}
float          Edgedat::getoffset() {return t/255.0;}
MatrixCarriage Edgedat::calcqef(QefPerc df,QefPerc mx,QefPerc my,QefPerc mz) {
    QefPerc xf = x/128.0;
    QefPerc yf = y/128.0;
    QefPerc zf = z/128.0;
    return MatrixCarriage(df*xf,
                          df*yf,
                          df*zf,
                          xf*xf,
                          yf*yf,
                          zf*zf,
                          xf*yf,
                          xf*zf,
                          yf*zf,mx,my,mz,1);
}
MatrixCarriage operator+(const MatrixCarriage &m1, const MatrixCarriage &m2){
    return MatrixCarriage(m1.Abx+m2.Abx,
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
                          m1.Mt+m2.Mt);
}
MatrixCarriage::MatrixCarriage() :
Abx(0), Aby(0), Abz(0), AAxx(0), AAyy(0), AAzz(0), AAxy(0), AAxz(0), AAyz(0),Mx(0),My(0),Mz(0),Mt(0) {};
MatrixCarriage::MatrixCarriage(QefPerc b,QefPerc c,QefPerc d,QefPerc e,QefPerc f,QefPerc g,QefPerc h,QefPerc i,QefPerc j,QefPerc x,QefPerc y,QefPerc z,int co) :
Abx(b), Aby(c), Abz(d), AAxx(e), AAyy(f), AAzz(g), AAxy(h), AAxz(i), AAyz(j) ,Mx(x),My(y),Mz(z),Mt(co){};
MatrixCarriage MatrixCarriage::translate(QefPerc x,QefPerc y,QefPerc z) {
    return MatrixCarriage(Abx+x*AAxx+y*AAxy+z*AAxz,
                          Aby+x*AAxy+y*AAyy+z*AAyz,
                          Abz+x*AAxz+y*AAyz+z*AAzz,
                          AAxx,
                          AAyy,
                          AAzz,
                          AAxy,
                          AAxz,
                          AAyz,
                          Mx+x*Mt,
                          My+y*Mt,
                          Mz+z*Mt,
                          Mt);
}
glm::vec3 MatrixCarriage::evaluate(int lod) {
    glm::dmat3 ATA = glm::dmat3(0.0);
    glm::dvec3 ATb = glm::dvec3(Abx,Aby,Abz);
    glm::dvec3 x = glm::dvec3(0.0);
    ATA[0][0] += AAxx;
    ATA[0][1] += AAxy;
    ATA[0][2] += AAxz;
    ATA[1][1] += AAyy;
    ATA[1][2] += AAyz;
    ATA[2][2] += AAzz;
    glm::dvec3 masspoint = glm::dvec3(Mx,My,Mz)*(1.0/Mt);
    ATb -= svd_vmul_sym(ATA, masspoint);
    svd_solve_ATA_ATb(ATA, ATb, x);
    //qef_calc_error(ATA, x, ATb);
    x += masspoint;
    if (x.x<0 or x.y<0 or x.z<0 or x.x>1<<lod or x.y>1<<lod or x.z>1<<lod) {
        x=masspoint;
    }
    return x;
}

void OctreeSegment::prepare(BlockLoc x,BlockLoc y,BlockLoc z) {}
void OctreeBranch::prepare(BlockLoc x,BlockLoc y,BlockLoc z) {
    BlockLoc mask = 1<<depth;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    subdivisions[0][0][0]->prepare(x     ,y     ,z     );
    subdivisions[1][0][0]->prepare(x|mask,y     ,z     );
    subdivisions[0][1][0]->prepare(x     ,y|mask,z     );
    subdivisions[1][1][0]->prepare(x|mask,y|mask,z     );
    subdivisions[0][0][1]->prepare(x     ,y     ,z|mask);
    subdivisions[1][0][1]->prepare(x|mask,y     ,z|mask);
    subdivisions[0][1][1]->prepare(x     ,y|mask,z|mask);
    subdivisions[1][1][1]->prepare(x|mask,y|mask,z|mask);
}
void OctreePortionAwareBranch::prepare(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (!prepared and hardload) {
        if (g_world->dataexistsat(x+CHSIZE,y,z) and
            g_world->dataexistsat(x,y+CHSIZE,z) and
            g_world->dataexistsat(x,y,z+CHSIZE) and
            g_world->dataexistsat(x,y+CHSIZE,z+CHSIZE) and
            g_world->dataexistsat(x+CHSIZE,y,z+CHSIZE) and
            g_world->dataexistsat(x+CHSIZE,y+CHSIZE,z)) {
//            hermitify(x,y,z);
            vertify(x,y,z);
            prepared=true;
        }
    }
    if (!psuedovoxed) {
        if (g_world->datapsuedoat(x-CHSIZE,y,z) and
            g_world->datapsuedoat(x,y-CHSIZE,z) and
            g_world->datapsuedoat(x,y,z-CHSIZE) and
            g_world->datapsuedoat(x,y-CHSIZE,z-CHSIZE) and
            g_world->datapsuedoat(x-CHSIZE,y,z-CHSIZE) and
            g_world->datapsuedoat(x-CHSIZE,y-CHSIZE,z)) {
            for (int tlod=0;tlod<levelsofdetail;tlod++) {
                g_geometry = &geometry;
                g_lod = tlod;
                manifestgeom(x,y,z);
                for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                    iterator->second.factor(tlod);
                }
            }
            psuedovoxed=true;
        }
    }
    if (prepared and !voxed) {
        if (g_world->dataloadedat(x-CHSIZE,y,z) and
            g_world->dataloadedat(x,y-CHSIZE,z) and
            g_world->dataloadedat(x,y,z-CHSIZE) and
            g_world->dataloadedat(x,y-CHSIZE,z-CHSIZE) and
            g_world->dataloadedat(x-CHSIZE,y,z-CHSIZE) and
            g_world->dataloadedat(x-CHSIZE,y-CHSIZE,z)) {
            for (int tlod=0;tlod<levelsofdetail;tlod++) {
                g_geometry = &geometry;
                int tsk[6] = {tlod,tlod,tlod,tlod,tlod,tlod};
                g_olods = tsk;
                g_lod = tlod;
                manifestgeom(x,y,z);
                for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                    iterator->second.factor(tlod);
                }
            }
            voxed=true;
        }
    }
}

//void OctreeSegment::hermitify(BlockLoc x,BlockLoc y,BlockLoc z) {}
//void OctreeFeature::hermitify(BlockLoc x,BlockLoc y,BlockLoc z) {
//    BlockId buf[2][2][2];
//    for (int xl=0;xl<2;xl++) {
//        for (int yl=0;yl<2;yl++) {
//            for (int zl=0;zl<2;zl++) {
//                buf[xl][yl][zl] = g_world->getser(x+xl,y+yl,z+zl);
//            }
//        }
//    }
//    for (int xl=0;xl<2;xl++) {
//        for (int yl=0;yl<2;yl++) {
//            if (buf[xl][yl][0]!=buf[xl][yl][1]) {
//                qef = qef+(g_world->zcon(x+xl,y+yl,z).calczqef().translate(xl,yl,0));
//            }
//        }
//    }
//    for (int xl=0;xl<2;xl++) {
//        for (int zl=0;zl<2;zl++) {
//            if (buf[xl][0][zl]!=buf[xl][1][zl]) {
//                qef = qef+(g_world->ycon(x+xl,y,z+zl).calcyqef().translate(xl,0,zl));
//            }
//        }
//    }
//    for (int yl=0;yl<2;yl++) {
//        for (int zl=0;zl<2;zl++) {
//            if (buf[0][yl][zl]!=buf[1][yl][zl]) {
//                qef = qef+(g_world->xcon(x,y+yl,z+zl).calcxqef().translate(0,yl,zl));
//            }
//        }
//    }
//}
//void OctreeBranch::hermitify(BlockLoc x,BlockLoc y,BlockLoc z) {
//    BlockLoc mask = 1<<depth;
//    subdivisions[0][0][0]->hermitify(x     ,y     ,z     );
//    subdivisions[1][0][0]->hermitify(x|mask,y     ,z     );
//    subdivisions[0][1][0]->hermitify(x     ,y|mask,z     );
//    subdivisions[1][1][0]->hermitify(x|mask,y|mask,z     );
//    subdivisions[0][0][1]->hermitify(x     ,y     ,z|mask);
//    subdivisions[1][0][1]->hermitify(x|mask,y     ,z|mask);
//    subdivisions[0][1][1]->hermitify(x     ,y|mask,z|mask);
//    subdivisions[1][1][1]->hermitify(x|mask,y|mask,z|mask);
//}

MatrixCarriage OctreeSegment::vertify(BlockLoc x,BlockLoc y,BlockLoc z) {return MatrixCarriage();}
MatrixCarriage OctreeFeature::vertify(BlockLoc x,BlockLoc y,BlockLoc z) {
    MatrixCarriage qef;
    BlockId buf[2][2][2];
    for (int xl=0;xl<2;xl++) {
        for (int yl=0;yl<2;yl++) {
            for (int zl=0;zl<2;zl++) {
                buf[xl][yl][zl] = g_world->getser(x+xl,y+yl,z+zl);
            }
        }
    }
    for (int xl=0;xl<2;xl++) {
        for (int yl=0;yl<2;yl++) {
            if (buf[xl][yl][0]!=buf[xl][yl][1]) {
                qef = qef+(g_world->zcon(x+xl,y+yl,z).calczqef().translate(xl,yl,0));
            }
        }
    }
    for (int xl=0;xl<2;xl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[xl][0][zl]!=buf[xl][1][zl]) {
                qef = qef+(g_world->ycon(x+xl,y,z+zl).calcyqef().translate(xl,0,zl));
            }
        }
    }
    for (int yl=0;yl<2;yl++) {
        for (int zl=0;zl<2;zl++) {
            if (buf[0][yl][zl]!=buf[1][yl][zl]) {
                qef = qef+(g_world->xcon(x,y+yl,z+zl).calcxqef().translate(0,yl,zl));
            }
        }
    }
//    throw;
    point = (int)g_vertecies->size();
    g_vertecies->push_back(qef.evaluate(0)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0)));
//    point = glm::vec3((x-ASBLOCKLOC(0))+0.5,(y-ASBLOCKLOC(0))+0.5,(z-ASBLOCKLOC(0))+0.5);
    return qef;
}
MatrixCarriage OctreeBranch::vertify(BlockLoc x,BlockLoc y,BlockLoc z) {
    BlockLoc mask = 1<<depth;
    MatrixCarriage q =
       subdivisions[0][0][0]->vertify(x     ,y     ,z     )+
       subdivisions[1][0][0]->vertify(x|mask,y     ,z     ).translate(1<<(depth),0         ,0)+
       subdivisions[0][1][0]->vertify(x     ,y|mask,z     ).translate(0         ,1<<(depth),0)+
       subdivisions[1][1][0]->vertify(x|mask,y|mask,z     ).translate(1<<(depth),1<<(depth),0)+
       subdivisions[0][0][1]->vertify(x     ,y     ,z|mask).translate(0         ,0         ,1<<(depth))+
       subdivisions[1][0][1]->vertify(x|mask,y     ,z|mask).translate(1<<(depth),0         ,1<<(depth))+
       subdivisions[0][1][1]->vertify(x     ,y|mask,z|mask).translate(0         ,1<<(depth),1<<(depth))+
       subdivisions[1][1][1]->vertify(x|mask,y|mask,z|mask).translate(1<<(depth),1<<(depth),1<<(depth));
    
    point = (int)g_vertecies[depth+1].size();
    g_vertecies[depth+1].push_back(q.evaluate(depth+1)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0)));
    
//    point = q.evaluate(depth+1)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0));
//    point = glm::vec3((x-ASBLOCKLOC(0))+mask,(y-ASBLOCKLOC(0))+mask,(z-ASBLOCKLOC(0))+mask);
    return q;
}

//MatrixCarriage OctreeSegment::getqef() {return MatrixCarriage();}
//MatrixCarriage OctreeFeature::getqef() {return qef;}
//MatrixCarriage OctreeBranch::getqef() {
//    return subdivisions[0][0][0]->getqef()+
//           subdivisions[1][0][0]->getqef().translate(1<<(depth),0         ,0)+
//           subdivisions[0][1][0]->getqef().translate(0         ,1<<(depth),0)+
//           subdivisions[1][1][0]->getqef().translate(1<<(depth),1<<(depth),0)+
//           subdivisions[0][0][1]->getqef().translate(0         ,0         ,1<<(depth))+
//           subdivisions[1][0][1]->getqef().translate(1<<(depth),0         ,1<<(depth))+
//           subdivisions[0][1][1]->getqef().translate(0         ,1<<(depth),1<<(depth))+
//           subdivisions[1][1][1]->getqef().translate(1<<(depth),1<<(depth),1<<(depth));
//}




