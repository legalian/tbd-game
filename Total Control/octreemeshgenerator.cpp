//
//  octreemeshgenerator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//


#include "octree.h"

//const int lodlimits[] = {
//200,
//400,
//500,
//800
//};
//const int levelsofdetail = 4;

void OctreeSegment::determinelod(BlockLoc x,BlockLoc y,BlockLoc z,BlockLoc px,BlockLoc py,BlockLoc pz,bool force_redo,OctreeSegment *world) {}
void OctreePortionAwareBranch::determinelod(BlockLoc x,BlockLoc y,BlockLoc z,BlockLoc px,BlockLoc py,BlockLoc pz,bool force_redo,OctreeSegment *world) {
    extern const int levelsofdetail;
    extern const int lodlimits[];
    BlockLoc mask = 1<<depth;
    float distance = sqrtf((x+mask-px)*(x+mask-px) + (y+mask-py)*(y+mask-py) + (z+mask-pz)*(z+mask-pz));
    for(lod=0;lodlimits[lod]<distance&&lod<levelsofdetail;lod++) {}
}
void OctreeBranch::determinelod(BlockLoc x,BlockLoc y,BlockLoc z,BlockLoc px,BlockLoc py,BlockLoc pz,bool force_redo,OctreeSegment *world) {
    BlockLoc mask = 1<<depth;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    subdivisions[0][0][0]->determinelod(x     ,y     ,z     ,px,py,pz,force_redo,world);
    subdivisions[1][0][0]->determinelod(x|mask,y     ,z     ,px,py,pz,force_redo,world);
    subdivisions[0][1][0]->determinelod(x     ,y|mask,z     ,px,py,pz,force_redo,world);
    subdivisions[1][1][0]->determinelod(x|mask,y|mask,z     ,px,py,pz,force_redo,world);
    subdivisions[0][0][1]->determinelod(x     ,y     ,z|mask,px,py,pz,force_redo,world);
    subdivisions[1][0][1]->determinelod(x|mask,y     ,z|mask,px,py,pz,force_redo,world);
    subdivisions[0][1][1]->determinelod(x     ,y|mask,z|mask,px,py,pz,force_redo,world);
    subdivisions[1][1][1]->determinelod(x|mask,y|mask,z|mask,px,py,pz,force_redo,world);
}
void OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z) {}
void OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z) {
    BlockLoc mask = 1<<depth;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    subdivisions[0][0][0]->geomify(x     ,y     ,z     );
    subdivisions[1][0][0]->geomify(x|mask,y     ,z     );
    subdivisions[0][1][0]->geomify(x     ,y|mask,z     );
    subdivisions[1][1][0]->geomify(x|mask,y|mask,z     );
    subdivisions[0][0][1]->geomify(x     ,y     ,z|mask);
    subdivisions[1][0][1]->geomify(x|mask,y     ,z|mask);
    subdivisions[0][1][1]->geomify(x     ,y|mask,z|mask);
    subdivisions[1][1][1]->geomify(x|mask,y|mask,z|mask);
}
bool OctreePortionAwareBranch::surroundinglod(BlockLoc x,BlockLoc y,BlockLoc z) {
    const int wahoo[6][3] = {
        {-1,0,0},
        {0,-1,0},
        {0,0,-1},
        {-1,-1,0},
        {-1,0,-1},
        {0,-1,-1}
    };
    bool needupdate=false;
    bool homo=true;
    for (int u=0;u<6;u++) {
        int flod = g_world->getlod(x+wahoo[u][0],y+wahoo[u][1],z+wahoo[u][2]);
        if (flod!=otherlods[u]) {
            otherlods[u]=flod;
            needupdate=true;
        }
        if (flod!=lod) {
            homo=false;
        }
    }
    return needupdate and !homo;
}
//void OctreePortionAwareBranch::updatevisual(BlockLoc x,BlockLoc y,BlockLoc z) {
//    for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
//        iterator->second.croptolod0();
//    }
//    surroundinglod(x,y,z);
//    if (newlod==0){
//        lod = newlod;
//        if (otherlods[0]!=0 || otherlods[3]!=0 ||
//            otherlods[1]!=0 || otherlods[4]!=0 ||
//            otherlods[2]!=0 || otherlods[5]!=0) {
//            g_geometry = &geometry;
//            g_lod = lod;
//            g_olods = otherlods;
//
//            manifestgeomsnippets(x,y,z,7);
//            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
//                iterator->second.factorextreme();
//            }
//        }
//        return;
//    }
//    lod = newlod;
//    g_lod = lod;
//    g_olods = otherlods;
//    g_geometry = &geometry;
//    manifestgeom(x,y,z);
//    for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
//        iterator->second.factorextreme();
//    }
//    lod = newlod;
//    if (otherlods[0]!=lod || otherlods[3]!=lod ||
//        otherlods[1]!=lod || otherlods[4]!=lod ||
//        otherlods[2]!=lod || otherlods[5]!=lod) {
//        g_geometry = &geometry;
//        g_lod = lod;
//        g_olods = otherlods;
//        manifestgeomsnippets(x,y,z,7);
//    }
//}
void OctreePortionAwareBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (lod<MIN_WORLDFILE_GEOMSAVE and !hardload) return;
    if (frustrumcul(glm::vec4(x+(1<<depth)-ASBLOCKLOC(0),y+(1<<depth)-ASBLOCKLOC(0),z+(1<<depth)-ASBLOCKLOC(0),1),2<<depth)) {
        if (g_world->dataloadedat(x-CHSIZE,y,z) and
            g_world->dataloadedat(x,y-CHSIZE,z) and
            g_world->dataloadedat(x,y,z-CHSIZE) and
            g_world->dataloadedat(x,y-CHSIZE,z-CHSIZE) and
            g_world->dataloadedat(x-CHSIZE,y,z-CHSIZE) and
            g_world->dataloadedat(x-CHSIZE,y-CHSIZE,z)) {
            if (surroundinglod(x,y,z)) {
                for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                    iterator->second.emptyExlay();
                }
                g_geometry = &geometry;
                g_lod = lod;
                g_olods = otherlods;
                manifestgeomsnippets(x,y,z,7);
            }
        }
        if (otherlods[0]==lod && otherlods[3]==lod &&
            otherlods[1]==lod && otherlods[4]==lod &&
            otherlods[2]==lod && otherlods[5]==lod) {
            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                registergeom(iterator->first, GeomLense(this,&iterator->second,lod));
            }
        } else {
            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                registergeom(iterator->first, GeomLense(this,&iterator->second,lod));
//                registergeom(iterator->first, GeomLense(this,&iterator->second,iterator->second.coresize,iterator->second.size()));
            }
        }
//        changed=false;
    }
}



//this might be annoying but functions evaluate parameters even if they are not used. macros do not.
#define STITCH4(g,a,bl,B,cl,C,dl,D,f,x,v) \
if (materialprops[f]!=materialprops[x]) { \
    bl;auto b=B; \
    cl;auto c=C; \
    dl;auto d=D; \
    if (materialprops[f]>materialprops[x]) { \
        (*g)[f].v(d); (*g)[f].v(a); (*g)[f].v(b); \
        (*g)[f].v(a); (*g)[f].v(d); (*g)[f].v(c); \
    } else { \
        (*g)[x].v(a); (*g)[x].v(d); (*g)[x].v(b); \
        (*g)[x].v(d); (*g)[x].v(a); (*g)[x].v(c); \
    } \
}
#define STITCH3(g,a,bl,b,cl,c,f,x,v) \
if (materialprops[f]!=materialprops[x]) { \
    if (materialprops[f]>materialprops[x]) { \
        bl;(*g)[f].v(a);(*g)[f].v(b);cl;(*g)[f].v(c); \
    } else { \
        bl;(*g)[x].v(b);(*g)[x].v(a);cl;(*g)[x].v(c); \
    } \
}



void OctreeSegment::manifestgeomsnippets(BlockLoc x,BlockLoc y,BlockLoc z,int f) {}
void OctreeBranch::manifestgeomsnippets(BlockLoc x,BlockLoc y,BlockLoc z,int f) {
    if (f) {
        if (g_lod>depth) {
            extern uint8_t materialprops[];
            int lod=g_lod;
            int xlod=x&CHMASK?lod:g_olods[0],ylod=y&CHMASK?lod:g_olods[1],zlod=z&CHMASK?lod:g_olods[2];
            int xylod=y&CHMASK?xlod:x&CHMASK?ylod:g_olods[5];
            int yzlod=z&CHMASK?ylod:y&CHMASK?zlod:g_olods[3];
            int zxlod=x&CHMASK?zlod:z&CHMASK?xlod:g_olods[4];
            int lodmin=std::min(std::min(std::min(std::min(std::min(xlod,ylod),zlod),xylod),yzlod),zxlod);
            for (int t=0;t<(1<<lod);t+=(1<<lodmin)) {
                STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=ylod,g_world->getfeature(x+t,y-1,z),g_lod=zlod,g_world->getfeature(x+t,y,z-1),g_lod=yzlod,g_world->getfeature(x+t,y-1,z-1),
                getser(t,0,0),g_world->getser(x+t+(1<<lodmin),y,z),addVertExlay);
                STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=zlod,g_world->getfeature(x,y+t,z-1),g_lod=xlod,g_world->getfeature(x-1,y+t,z),g_lod=zxlod,g_world->getfeature(x-1,y+t,z-1),
                getser(0,t,0),g_world->getser(x,y+t+(1<<lodmin),z),addVertExlay);
                STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=xlod,g_world->getfeature(x-1,y,z+t),g_lod=ylod,g_world->getfeature(x,y-1,z+t),g_lod=xylod,g_world->getfeature(x-1,y-1,z+t),
                getser(0,0,t),g_world->getser(x,y,z+t+(1<<lodmin)),addVertExlay);
                for (int i=(1<<lodmin);i<(1<<lod);i+=(1<<lodmin)) {
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=zlod,g_world->getfeature(x+t,y+i-1,z-1),g_lod=zlod,g_world->getfeature(x+t,y+i,z-1),
                    getser(t,i,0),g_world->getser(x+t+(1<<lodmin),y+i,z),addVertExlay);
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=ylod,g_world->getfeature(x+t,y-1,z+i),g_lod=ylod,g_world->getfeature(x+t,y-1,z+i-1),
                    getser(t,0,i),g_world->getser(x+t+(1<<lodmin),y,z+i),addVertExlay);
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=xlod,g_world->getfeature(x-1,y+t,z+i-1),g_lod=xlod,g_world->getfeature(x-1,y+t,z+i),
                    getser(0,t,i),g_world->getser(x,y+t+(1<<lodmin),z+i),addVertExlay);
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=zlod,g_world->getfeature(x+i,y+t,z-1),g_lod=zlod,g_world->getfeature(x+i-1,y+t,z-1),
                    getser(i,t,0),g_world->getser(x+i,y+t+(1<<lodmin),z),addVertExlay);
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=ylod,g_world->getfeature(x+i-1,y-1,z+t),g_lod=ylod,g_world->getfeature(x+i,y-1,z+t),
                    getser(i,0,t),g_world->getser(x+i,y,z+t+(1<<lodmin)),addVertExlay);
                    STITCH3(g_geometry,g_vertecies[g_lod][point],g_lod=xlod,g_world->getfeature(x-1,y+i,z+t),g_lod=xlod,g_world->getfeature(x-1,y+i-1,z+t),
                    getser(0,i,t),g_world->getser(x,y+i,z+t+(1<<lodmin)),addVertExlay);
                }
                g_lod = lod;
            }
        } else {
            BlockLoc mask = 1<<depth;
            subdivisions[0][0][0]->manifestgeomsnippets(x     ,y     ,z     ,f);
            subdivisions[1][0][0]->manifestgeomsnippets(x|mask,y     ,z     ,f&6);
            subdivisions[0][1][0]->manifestgeomsnippets(x     ,y|mask,z     ,f&5);
            subdivisions[1][1][0]->manifestgeomsnippets(x|mask,y|mask,z     ,f&4);
            subdivisions[0][0][1]->manifestgeomsnippets(x     ,y     ,z|mask,f&3);
            subdivisions[1][0][1]->manifestgeomsnippets(x|mask,y     ,z|mask,f&2);
            subdivisions[0][1][1]->manifestgeomsnippets(x     ,y|mask,z|mask,f&1);
        }
    }
}
void OctreeLeaf::manifestgeomsnippets(BlockLoc x,BlockLoc y,BlockLoc z,int f) {
    if (f) {
        extern uint8_t materialprops[];
        int lod=g_lod;
        int xlod=x&CHMASK?lod:g_olods[0],ylod=y&CHMASK?lod:g_olods[1],zlod=z&CHMASK?lod:g_olods[2];
        int xylod=y&CHMASK?xlod:x&CHMASK?ylod:g_olods[5];
        int yzlod=z&CHMASK?ylod:y&CHMASK?zlod:g_olods[3];
        int zxlod=x&CHMASK?zlod:z&CHMASK?xlod:g_olods[4];
        STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=ylod,g_world->getfeature(x,y-1,z),g_lod=zlod,g_world->getfeature(x,y,z-1),g_lod=yzlod,g_world->getfeature(x,y-1,z-1),
        fillvalue,g_world->getser(x+1,y,z),addVertExlay);
        STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=zlod,g_world->getfeature(x,y,z-1),g_lod=xlod,g_world->getfeature(x-1,y,z),g_lod=zxlod,g_world->getfeature(x-1,y,z-1),
        fillvalue,g_world->getser(x,y+1,z),addVertExlay);
        STITCH4(g_geometry,g_vertecies[g_lod][point],g_lod=xlod,g_world->getfeature(x-1,y,z),g_lod=ylod,g_world->getfeature(x,y-1,z),g_lod=xylod,g_world->getfeature(x-1,y-1,z),
        fillvalue,g_world->getser(x,y,z+1),addVertExlay);
        g_lod = lod;
    }
}



void OctreeSegment::manifestgeom(BlockLoc x,BlockLoc y,BlockLoc z) {}
void OctreeBranch::manifestgeom(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (g_lod>depth) {
        extern uint8_t materialprops[];
        if (!(x&CHMASK) or !(y&CHMASK) or !(z&CHMASK)) {
            uint8_t an = getser(0,0,0);
            STITCH4(g_geometry,point,,g_world->feat(x,y-1,z),,g_world->feat(x,y,z-1),,g_world->feat(x,y-1,z-1),an,g_world->getser(x+(1<<g_lod),y,z),addVertExt);
            STITCH4(g_geometry,point,,g_world->feat(x,y,z-1),,g_world->feat(x-1,y,z),,g_world->feat(x-1,y,z-1),an,g_world->getser(x,y+(1<<g_lod),z),addVertExt);
            STITCH4(g_geometry,point,,g_world->feat(x-1,y,z),,g_world->feat(x,y-1,z),,g_world->feat(x-1,y-1,z),an,g_world->getser(x,y,z+(1<<g_lod)),addVertExt);
        } else {
            uint8_t an = getser(0,0,0);
            STITCH4(g_geometry,point,,g_world->feat(x,y-1,z),,g_world->feat(x,y,z-1),,g_world->feat(x,y-1,z-1),an,g_world->getser(x+(1<<g_lod),y,z),addVert);
            STITCH4(g_geometry,point,,g_world->feat(x,y,z-1),,g_world->feat(x-1,y,z),,g_world->feat(x-1,y,z-1),an,g_world->getser(x,y+(1<<g_lod),z),addVert);
            STITCH4(g_geometry,point,,g_world->feat(x-1,y,z),,g_world->feat(x,y-1,z),,g_world->feat(x-1,y-1,z),an,g_world->getser(x,y,z+(1<<g_lod)),addVert);
        }
    } else {
        BlockLoc mask = 1<<depth;
        subdivisions[0][0][0]->manifestgeom(x     ,y     ,z     );
        subdivisions[1][0][0]->manifestgeom(x|mask,y     ,z     );
        subdivisions[0][1][0]->manifestgeom(x     ,y|mask,z     );
        subdivisions[1][1][0]->manifestgeom(x|mask,y|mask,z     );
        subdivisions[0][0][1]->manifestgeom(x     ,y     ,z|mask);
        subdivisions[1][0][1]->manifestgeom(x|mask,y     ,z|mask);
        subdivisions[0][1][1]->manifestgeom(x     ,y|mask,z|mask);
        subdivisions[1][1][1]->manifestgeom(x|mask,y|mask,z|mask);
    }
}
void OctreeLeaf::manifestgeom(BlockLoc x,BlockLoc y,BlockLoc z) {
    extern uint8_t materialprops[];
    if (!(x&CHMASK) or !(y&CHMASK) or !(z&CHMASK)) {
        STITCH4(g_geometry,point,,g_world->feat(x,y-1,z),,g_world->feat(x,y,z-1),,g_world->feat(x,y-1,z-1),fillvalue,g_world->getser(x+1,y,z),addVertExt);
        STITCH4(g_geometry,point,,g_world->feat(x,y,z-1),,g_world->feat(x-1,y,z),,g_world->feat(x-1,y,z-1),fillvalue,g_world->getser(x,y+1,z),addVertExt);
        STITCH4(g_geometry,point,,g_world->feat(x-1,y,z),,g_world->feat(x,y-1,z),,g_world->feat(x-1,y-1,z),fillvalue,g_world->getser(x,y,z+1),addVertExt);
    } else {
        STITCH4(g_geometry,point,,g_world->feat(x,y-1,z),,g_world->feat(x,y,z-1),,g_world->feat(x,y-1,z-1),fillvalue,g_world->getser(x+1,y,z),addVert);
        STITCH4(g_geometry,point,,g_world->feat(x,y,z-1),,g_world->feat(x-1,y,z),,g_world->feat(x-1,y,z-1),fillvalue,g_world->getser(x,y+1,z),addVert);
        STITCH4(g_geometry,point,,g_world->feat(x-1,y,z),,g_world->feat(x,y-1,z),,g_world->feat(x-1,y-1,z),fillvalue,g_world->getser(x,y,z+1),addVert);
    }
}







