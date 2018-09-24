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

void OctreeSegment::determinelod(int x,int y,int z,int px,int py,int pz,bool force_redo,OctreeSegment *world) {}
void OctreePortionAwareBranch::determinelod(int x,int y,int z,int px,int py,int pz,bool force_redo,OctreeSegment *world) {
//    extern const int levelsofdetail;
    extern const int lodlimits[];
    int mask = 1<<depth;
//    lod = (((x/CHSIZE)-(z/CHSIZE)+2*(y/CHSIZE))+9)%4;
//    lod = (((x/CHSIZE)+(z/CHSIZE))+9)%4;
    float distance = sqrtf((x+mask-px)*(x+mask-px) + (y+mask-py)*(y+mask-py) + (z+mask-pz)*(z+mask-pz));
    for(lod=0;lodlimits[lod]<distance&&lod<=MAX_WORLDFILE_GEOMSAVE;lod++) {}
}
void OctreeBranch::determinelod(int x,int y,int z,int px,int py,int pz,bool force_redo,OctreeSegment *world) {
    int mask = 1<<depth;
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
void OctreeSegment::geomify(int x,int y,int z) {}
void OctreeBranch::geomify(int x,int y,int z) {
    int mask = 1<<depth;
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
bool OctreePortionAwareBranch::surroundinglod(int x,int y,int z) {
    const int wahoo[7][3] = {
        {-1,0,0},
        {0,-1,0},
        {0,0,-1},
        {0,-1,-1},
        {-1,0,-1},
        {-1,-1,0},
        {0,0,0}
    };
    bool needupdate=false;
    for (int u=0;u<7;u++) {
        int flod = g_world->getlod(x+wahoo[u][0],y+wahoo[u][1],z+wahoo[u][2]);
        if (flod!=otherlods[u]) {
            otherlods[u]=flod;
            needupdate=true;
        }
    }
    return needupdate;
}
void OctreePortionAwareBranch::geomify(int x,int y,int z) {
    if (!(
    (lod<MIN_WORLDFILE_GEOMSAVE and voxed) or
    (lod>=MIN_WORLDFILE_GEOMSAVE and (psuedovoxed or voxed))
    )) return;
    if (!prepared) return;
    if (frustrumcul(glm::vec4(x+(1<<depth)-ASBLOCKLOC(0),y+(1<<depth)-ASBLOCKLOC(0),z+(1<<depth)-ASBLOCKLOC(0),1),2<<depth)) {
        if (g_world->vertsloadedat(x-CHSIZE,y,z) and
            g_world->vertsloadedat(x,y-CHSIZE,z) and
            g_world->vertsloadedat(x,y,z-CHSIZE) and
            g_world->vertsloadedat(x,y-CHSIZE,z-CHSIZE) and
            g_world->vertsloadedat(x-CHSIZE,y,z-CHSIZE) and
            g_world->vertsloadedat(x-CHSIZE,y-CHSIZE,z)) {
            if (surroundinglod(x,y,z)) {
//                std::cout<<Location((x-ASBLOCKLOC(0))/CHSIZE,(y-ASBLOCKLOC(0))/CHSIZE,(z-ASBLOCKLOC(0))/CHSIZE).tostring()<<"<g\n";
                for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                    iterator->second.emptyExlay();
                }
                g_geometry = &geometry;
                g_lod = lod;
                g_olods = otherlods;
//                std::cout<<otherlods[0]<<otherlods[1]<<otherlods[2]<<otherlods[3]<<otherlods[4]<<otherlods[5]<<lod<<"\n";
                g_vertecies = vertecies;
                g_normals = normals;
                manifestgeomsnippets(x,y,z,7);
                for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                    iterator->second.exbaked = false;
                }
            }
//        }
//        if (voxed) {
            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                registergeom(iterator->first, GeomLense(this,&iterator->second,lod));
//                registergeom(iterator->first, GeomLense(this,&iterator->second,iterator->second.coresize,iterator->second.size()));
            }
        }
//        changed=false;
    }
}



//this might be annoying but functions evaluate parameters even if they are not used. macros do not.
#define LSTITCH4(B,C,D,f,x) \
if (materialprops[f]!=materialprops[x]) { \
    auto a=point; \
    auto b=g_world->feat B; \
    auto c=g_world->feat C; \
    auto d=g_world->feat D; \
    if (materialprops[f]>materialprops[x]) { \
        (*g_geometry)[f].addVert(d); (*g_geometry)[f].addVert(a); (*g_geometry)[f].addVert(b); \
        (*g_geometry)[f].addVert(a); (*g_geometry)[f].addVert(d); (*g_geometry)[f].addVert(c); \
    } else { \
        (*g_geometry)[x].addVert(a); (*g_geometry)[x].addVert(d); (*g_geometry)[x].addVert(b); \
        (*g_geometry)[x].addVert(d); (*g_geometry)[x].addVert(a); (*g_geometry)[x].addVert(c); \
    } \
}
#define STITCH4(bl,B,cl,C,dl,D,f,x) \
if (materialprops[f]!=materialprops[x]) { \
    auto a=g_vertecies[lod][point],an=g_normals[lod][point];\
    g_lod=bl;auto b=g_world->getfeature B,bn=g_world->getnormal B;\
    g_lod=cl;auto c=g_world->getfeature C,cn=g_world->getnormal C;\
    g_lod=dl;auto d=g_world->getfeature D,dn=g_world->getnormal D;\
    if (materialprops[f]>materialprops[x]) { \
        (*g_geometry)[f].addVertExlay(d); (*g_geometry)[f].addVertExlay(a); (*g_geometry)[f].addVertExlay(b); \
        (*g_geometry)[f].addVertExlay(a); (*g_geometry)[f].addVertExlay(d); (*g_geometry)[f].addVertExlay(c); \
        (*g_geometry)[f].addVertExlayNormal(dn); (*g_geometry)[f].addVertExlayNormal(an); (*g_geometry)[f].addVertExlayNormal(bn); \
        (*g_geometry)[f].addVertExlayNormal(an); (*g_geometry)[f].addVertExlayNormal(dn); (*g_geometry)[f].addVertExlayNormal(cn); \
    } else { \
        (*g_geometry)[x].addVertExlay(a); (*g_geometry)[x].addVertExlay(d); (*g_geometry)[x].addVertExlay(b); \
        (*g_geometry)[x].addVertExlay(d); (*g_geometry)[x].addVertExlay(a); (*g_geometry)[x].addVertExlay(c); \
        (*g_geometry)[x].addVertExlayNormal(an); (*g_geometry)[x].addVertExlayNormal(dn); (*g_geometry)[x].addVertExlayNormal(bn); \
        (*g_geometry)[x].addVertExlayNormal(dn); (*g_geometry)[x].addVertExlayNormal(an); (*g_geometry)[x].addVertExlayNormal(cn); \
    } \
}
#define STITCH3(bl,B,cl,C,f,x) \
if (materialprops[f]!=materialprops[x]) { \
    auto a=g_vertecies[lod][point],an=g_normals[lod][point];\
    g_lod=bl;auto b=g_world->getfeature B,bn=g_world->getnormal B;\
    g_lod=cl;auto c=g_world->getfeature C,cn=g_world->getnormal C;\
    if (materialprops[f]>materialprops[x]) { \
        (*g_geometry)[f].addVertExlay(a);(*g_geometry)[f].addVertExlay(b);(*g_geometry)[f].addVertExlay(c); \
        (*g_geometry)[f].addVertExlayNormal(an); (*g_geometry)[f].addVertExlayNormal(bn); (*g_geometry)[f].addVertExlayNormal(cn); \
    } else { \
        (*g_geometry)[x].addVertExlay(a);(*g_geometry)[x].addVertExlay(c);(*g_geometry)[x].addVertExlay(b); \
        (*g_geometry)[x].addVertExlayNormal(an); (*g_geometry)[x].addVertExlayNormal(cn); (*g_geometry)[x].addVertExlayNormal(bn); \
    } \
}



void OctreeSegment::manifestgeomsnippets(int x,int y,int z,int f) {}
void OctreeBranch::manifestgeomsnippets(int x,int y,int z,int f) {
    if (f) {
        if (g_lod>depth) {
            extern uint8_t materialprops[];
            int lod=g_lod;
            int xlod=x&CHMASK?lod:g_olods[0];
            int ylod=y&CHMASK?lod:g_olods[1];
            int zlod=z&CHMASK?lod:g_olods[2];
            int xylod=y&CHMASK?xlod:x&CHMASK?ylod:g_olods[5];
            int yzlod=z&CHMASK?ylod:y&CHMASK?zlod:g_olods[3];
            int zxlod=x&CHMASK?zlod:z&CHMASK?xlod:g_olods[4];
            int lodmin=std::min(std::min(std::min(std::min(std::min(xlod,ylod),zlod),xylod),yzlod),zxlod);
            for (int t=0;t<(1<<lod);t+=(1<<lodmin)) {
                STITCH4(ylod,(x+t,y-1,z),zlod,(x+t,y,z-1),yzlod,(x+t,y-1,z-1),getser(t,0,0),g_world->getser(x+t+(1<<lodmin),y,z));
                STITCH4(zlod,(x,y+t,z-1),xlod,(x-1,y+t,z),zxlod,(x-1,y+t,z-1),getser(0,t,0),g_world->getser(x,y+t+(1<<lodmin),z));
                STITCH4(xlod,(x-1,y,z+t),ylod,(x,y-1,z+t),xylod,(x-1,y-1,z+t),getser(0,0,t),g_world->getser(x,y,z+t+(1<<lodmin)));
                for (int i=(1<<lodmin);i<(1<<lod);i+=(1<<lodmin)) {
                    STITCH3(zlod,(x+t,y+i-1,z-1),zlod,(x+t,y+i,z-1),getser(t,i,0),g_world->getser(x+t+(1<<lodmin),y+i,z));
                    STITCH3(ylod,(x+t,y-1,z+i),ylod,(x+t,y-1,z+i-1),getser(t,0,i),g_world->getser(x+t+(1<<lodmin),y,z+i));
                    STITCH3(xlod,(x-1,y+t,z+i-1),xlod,(x-1,y+t,z+i),getser(0,t,i),g_world->getser(x,y+t+(1<<lodmin),z+i));
                    STITCH3(zlod,(x+i,y+t,z-1),zlod,(x+i-1,y+t,z-1),getser(i,t,0),g_world->getser(x+i,y+t+(1<<lodmin),z));
                    STITCH3(ylod,(x+i-1,y-1,z+t),ylod,(x+i,y-1,z+t),getser(i,0,t),g_world->getser(x+i,y,z+t+(1<<lodmin)));
                    STITCH3(xlod,(x-1,y+i,z+t),xlod,(x-1,y+i-1,z+t),getser(0,i,t),g_world->getser(x,y+i,z+t+(1<<lodmin)));
                }
            }
            g_lod = lod;
        } else {
            int mask = 1<<depth;
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
void OctreeLeaf::manifestgeomsnippets(int x,int y,int z,int f) {
    if (f) {
        extern uint8_t materialprops[];
        int lod=g_lod;
        int xlod=x&CHMASK?lod:g_olods[0];
        int ylod=y&CHMASK?lod:g_olods[1];
        int zlod=z&CHMASK?lod:g_olods[2];
        int xylod=y&CHMASK?xlod:x&CHMASK?ylod:g_olods[5];
        int yzlod=z&CHMASK?ylod:y&CHMASK?zlod:g_olods[3];
        int zxlod=x&CHMASK?zlod:z&CHMASK?xlod:g_olods[4];
        STITCH4(ylod,(x,y-1,z),zlod,(x,y,z-1),yzlod,(x,y-1,z-1),fillvalue,g_world->getser(x+1,y,z));
        STITCH4(zlod,(x,y,z-1),xlod,(x-1,y,z),zxlod,(x-1,y,z-1),fillvalue,g_world->getser(x,y+1,z));
        STITCH4(xlod,(x-1,y,z),ylod,(x,y-1,z),xylod,(x-1,y-1,z),fillvalue,g_world->getser(x,y,z+1));
        g_lod = lod;
    }
}



void OctreeSegment::manifestgeom(int x,int y,int z) {}
void OctreeBranch::manifestgeom(int x,int y,int z) {
    if (g_lod>depth) {
        extern uint8_t materialprops[];
        if ((x&CHMASK) and (y&CHMASK) and (z&CHMASK)) {
            uint8_t an = getser(0,0,0);
            LSTITCH4((x,y-1,z),(x,y,z-1),(x,y-1,z-1),an,g_world->getser(x+(1<<g_lod),y,z));
            LSTITCH4((x,y,z-1),(x-1,y,z),(x-1,y,z-1),an,g_world->getser(x,y+(1<<g_lod),z));
            LSTITCH4((x-1,y,z),(x,y-1,z),(x-1,y-1,z),an,g_world->getser(x,y,z+(1<<g_lod)));
        }
    } else {
        int mask = 1<<depth;
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
void OctreeLeaf::manifestgeom(int x,int y,int z) {
    extern uint8_t materialprops[];
    if ((x&CHMASK) and (y&CHMASK) and (z&CHMASK)) {
        LSTITCH4((x,y-1,z),(x,y,z-1),(x,y-1,z-1),fillvalue,g_world->getser(x+1,y,z));
        LSTITCH4((x,y,z-1),(x-1,y,z),(x-1,y,z-1),fillvalue,g_world->getser(x,y+1,z));
        LSTITCH4((x-1,y,z),(x,y-1,z),(x-1,y-1,z),fillvalue,g_world->getser(x,y,z+1));
    }
}







