//
//  octreemeshgenerator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"


void OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {}

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
    Xstitch(x,y,z,geometry,world,fillvalue,world->getser(x+(1<<lod),y,z));
    Ystitch(x,y,z,geometry,world,fillvalue,world->getser(x,y+(1<<lod),z));
    Zstitch(x,y,z,geometry,world,fillvalue,world->getser(x,y,z+(1<<lod)));
}


BranchRegistry* SWO;
void OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,int lod) {
#ifdef WIREFRAMEDEBUG
    extern BranchRegistry* SWO;
    
//    if (DEBUGDEBUG!=0) {
//        drawdebugcube(x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)),(1<<(depth+1)),(1<<(depth+1)),(1<<(depth+1)),geometry,
//                      1,1,1);
//    }

#endif
    if (depth<lod) {
#define NULLSUB(a) a==NULL?56:a->curlod
        OctreePortionAwareBranch* a=world->getvoxunit(x-1,y,z);
        OctreePortionAwareBranch* b=world->getvoxunit(x,y-1,z);
        OctreePortionAwareBranch* c=world->getvoxunit(x,y,z-1);
        OctreePortionAwareBranch* d=world->getvoxunit(x-1,y-1,z);
        OctreePortionAwareBranch* e=world->getvoxunit(x-1,y,z-1);
        OctreePortionAwareBranch* f=world->getvoxunit(x,y-1,z-1);
        int lodmin = std::min(std::min(std::min(NULLSUB(a),NULLSUB(b)),NULLSUB(c)), std::min(std::min(NULLSUB(d),NULLSUB(e)),NULLSUB(f)));
#undef NULLSUB
        if (lodmin>=lod) {
            uint8_t an  = getser(0,0,0);
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