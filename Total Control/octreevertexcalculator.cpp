//
//  octreevertexcalculator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"



void OctreeSegment::hermitify(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
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

MatrixCarriage OctreeSegment::vertifysmall() {return MatrixCarriage();}
MatrixCarriage OctreeFeature::vertifysmall() {
    point = qef.evaluate(0);
    return qef;
}
MatrixCarriage OctreeBranch::vertifysmall() {
    if (depth+1<MIN_WORLDFILE_GEOMSAVE) {
        if (!(lodflags1&4)) {
            lodflags1|=4;
            MatrixCarriage q =
               subdivisions[0][0][0]->vertifysmall()+
               subdivisions[1][0][0]->vertifysmall().translate(1<<(depth),0         ,0)+
               subdivisions[0][1][0]->vertifysmall().translate(0         ,1<<(depth),0)+
               subdivisions[1][1][0]->vertifysmall().translate(1<<(depth),1<<(depth),0)+
               subdivisions[0][0][1]->vertifysmall().translate(0         ,0         ,1<<(depth))+
               subdivisions[1][0][1]->vertifysmall().translate(1<<(depth),0         ,1<<(depth))+
               subdivisions[0][1][1]->vertifysmall().translate(0         ,1<<(depth),1<<(depth))+
               subdivisions[1][1][1]->vertifysmall().translate(1<<(depth),1<<(depth),1<<(depth));
            point = q.evaluate(depth+1);
            return q;
        }
    } else {
        subdivisions[0][0][0]->vertifysmall();
        subdivisions[1][0][0]->vertifysmall();
        subdivisions[0][1][0]->vertifysmall();
        subdivisions[1][1][0]->vertifysmall();
        subdivisions[0][0][1]->vertifysmall();
        subdivisions[1][0][1]->vertifysmall();
        subdivisions[0][1][1]->vertifysmall();
        subdivisions[1][1][1]->vertifysmall();
    }
    if (depth+2<MIN_WORLDFILE_GEOMSAVE) {
        return getqef();
    }
    return MatrixCarriage();
}

MatrixCarriage OctreeSegment::vertifyall() {return MatrixCarriage();}
MatrixCarriage OctreeFeature::vertifyall() {
    point = qef.evaluate(0);
    return qef;
}
MatrixCarriage OctreeBranch::vertifyall() {
    if (depth+1<=MAX_WORLDFILE_GEOMSAVE) {
        if (!(lodflags1&4)) {
            lodflags1|=4;
            MatrixCarriage q =
               subdivisions[0][0][0]->vertifyall()+
               subdivisions[1][0][0]->vertifyall().translate(1<<(depth),0         ,0)+
               subdivisions[0][1][0]->vertifyall().translate(0         ,1<<(depth),0)+
               subdivisions[1][1][0]->vertifyall().translate(1<<(depth),1<<(depth),0)+
               subdivisions[0][0][1]->vertifyall().translate(0         ,0         ,1<<(depth))+
               subdivisions[1][0][1]->vertifyall().translate(1<<(depth),0         ,1<<(depth))+
               subdivisions[0][1][1]->vertifyall().translate(0         ,1<<(depth),1<<(depth))+
               subdivisions[1][1][1]->vertifyall().translate(1<<(depth),1<<(depth),1<<(depth));
            point = q.evaluate(depth+1);
            return q;
        }
    } else {
        subdivisions[0][0][0]->vertifyall();
        subdivisions[1][0][0]->vertifyall();
        subdivisions[0][1][0]->vertifyall();
        subdivisions[1][1][0]->vertifyall();
        subdivisions[0][0][1]->vertifyall();
        subdivisions[1][0][1]->vertifyall();
        subdivisions[0][1][1]->vertifyall();
        subdivisions[1][1][1]->vertifyall();
    }
    return MatrixCarriage();
}

MatrixCarriage OctreeSegment::getqef() {return MatrixCarriage();}
MatrixCarriage OctreeFeature::getqef() {return qef;}
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




