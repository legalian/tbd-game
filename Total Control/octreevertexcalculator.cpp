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







void OctreeSegment::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {}
void OctreeFeature::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {
    point = qef.evaluate(0);
}
void OctreeBranch::vertify(BlockLoc x,BlockLoc y,BlockLoc z,int target) {

//    if (!(bakeddetails&(1<<target))) {
//        bakeddetails |= (1<<target);
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
//    }
}

void OctreeSegment::vertifySoftLoads() {}
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




