//
//  octree.cpp
//  Total Control
//
//  Created by Parker Lawrence on 2/29/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"

//int OctreeSegment::m_x1;
//int OctreeSegment::m_y1;
//int OctreeSegment::m_z1;
//int OctreeSegment::m_nup;
thread_local int g_lod;
thread_local int* g_olods;
thread_local OctreeSegment* g_world;
thread_local std::map<uint8_t,GeomTerrain>* g_geometry;
thread_local std::vector<glm::vec3>* g_vertecies;
thread_local std::vector<glm::vec3>* g_normals;
thread_local std::ostream* s_file;

thread_local Sampler* g_sampler;
thread_local float* g_samplefield;
thread_local BlockId* g_idfield;


glm::vec3 readvoxvert(int x,int y,int z,uint8_t a,uint8_t b,uint8_t c,int dep) {
    return glm::vec3(x-ASBLOCKLOC(0)+(a/256.0)*(1<<dep),y-ASBLOCKLOC(0)+(b/256.0)*(1<<dep),z-ASBLOCKLOC(0)+(c/256.0)*(1<<dep));
}
glm::vec3 readvoxnorm(int8_t a,int8_t b,int8_t c) {
    return glm::vec3(a/128.0,b/128.0,c/128.0);
}
uint8_t savevoxvert(int x,float a,int dep) {
    return (a-((x-ASBLOCKLOC(0))&~((1<<(dep+1))-1)))/256.0;
}
int8_t savevoxnorm(float a) {
    return a/128.0;
}

//int OctreeSegment::m_lod;
//int OctreeSegment::l_lod;
//int* OctreeSegment::m_olods;
//int* OctreeSegment::l_olods;
//OctreeSegment* OctreeSegment::m_world;
////OctreePortionAwareBranch* OctreeSegment::suppliment=NULL;
//OctreeSegment* OctreeSegment::l_world;
//
//std::map<uint8_t,GeomTerrain>* OctreeSegment::m_geometry;
//std::map<uint8_t,GeomTerrain>* OctreeSegment::l_geometry;
//
//std::ostream* OctreeSegment::s_file;

Edgedat silence = Edgedat((float)0,0,0,.5);
glm::vec3 errorpoint = glm::vec3(0,0,0);

OctreePortionAwareBranch* OctreeSegment::getvoxunit(int x,int y,int z) {return NULL;}
OctreeSegment* OctreeSegment::indivref(int x,int y,int z) {throw;}
BlockId OctreeSegment::getser(int x,int y,int z) {throw;}
Edgedat& OctreeSegment::xcon(int x,int y,int z) {return silence;}
Edgedat& OctreeSegment::ycon(int x,int y,int z) {return silence;}
Edgedat& OctreeSegment::zcon(int x,int y,int z) {return silence;}
//glm::vec3& OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {throw;}
int OctreeSegment::feat(int x,int y,int z) {return 0;}
glm::vec3& OctreeSegment::getfeature(int x,int y,int z) {return errorpoint;}
glm::vec3& OctreeSegment::getnormal(int x,int y,int z) {return errorpoint;}


void OctreeSegment::insertinto(int x,int y,int z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
OctreeSegment* OctreeSegment::pullaway(int x,int y,int z,int recur,OctreeSegment*& self) {self = new OctreeBud(1);return this;}

OctreeFeature::OctreeFeature(BlockId fill,uint8_t conn) : fillvalue(fill) , conflag(conn) {}
BlockId OctreeFeature::getser(int x,int y,int z) {return fillvalue;}
//glm::vec3& OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return point;}//.get(lod,x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)));}
int OctreeFeature::feat(int x,int y,int z)  {return point;}//.get(lod,x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)));}


OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn) : OctreeFeature(fill,conn) {}
OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn,int8_t n[12]) : OctreeFeature(fill,conn),
    xcondat(n[0],n[1],n[2],n[3]),
    ycondat(n[4],n[5],n[6],n[7]),
    zcondat(n[8],n[9],n[10],n[11])  {}

Edgedat& OctreeLeaf::xcon(int x,int y,int z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(int x,int y,int z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(int x,int y,int z) {return zcondat;}

OctreeBud::OctreeBud(BlockId fill) : fillvalue(fill) {}

BlockId OctreeBud::getser(int x,int y,int z) {return fillvalue;}

void OctreeBud::insertinto(int x,int y,int z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (thisdep==recur) {
        delete self;
        self = toinsert;
    } else {
        delete self;
        if (thisdep==CHPOWER) {
            self = new OctreePortionAwareBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                                new OctreeBud(fillvalue),new OctreeBud(fillvalue),true);
        } else {
            self = new OctreeBranch(new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),
                                    new OctreeBud(fillvalue),new OctreeBud(fillvalue),thisdep-1);
        }
        self->insertinto(x,y,z,recur,thisdep,toinsert,self);
    }
}

//OctreePortionAwareBranch::OctreePortionAwareBranch(float* a) : OctreeBranch(0,0,0,0,0,0,0,0,CHPOWER-1),softload(false),grdata(a) {}
OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,OctreeSegment* b,
                                                   OctreeSegment* c,OctreeSegment* d,
                                                   OctreeSegment* e,OctreeSegment* f,
                                                   OctreeSegment* g,OctreeSegment* h,bool hl) :
OctreeBranch(a,b,c,d,e,f,g,h,CHPOWER-1),hardload(hl) {}//bakeddetails=hard?0:WORLDFILE_VBITMASK;}

OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,OctreeSegment* b,
                                                   OctreeSegment* c,OctreeSegment* d,
                                                   OctreeSegment* e,OctreeSegment* f,
                                                   OctreeSegment* g,OctreeSegment* h,bool hl,glm::vec3 po,glm::vec3 no) :
OctreeBranch(a,b,c,d,e,f,g,h,CHPOWER-1,po,no),hardload(hl) {}//bakeddetails=WORLDFILE_VBITMASK;}
OctreePortionAwareBranch::OctreePortionAwareBranch(BlockId bi,glm::vec3 po,glm::vec3 no,uint8_t con) : OctreeBranch(bi,CHPOWER-1,po,no,con),hardload(false) {}
//OctreePortionAwareBranch::OctreePortionAwareBranch(OctreeSegment* a,int dep,uint8_t x,uint8_t y,uint8_t z,uint8_t con) :
//OctreeBranch(a,dep,x,y,z,con) {}//bakeddetails=WORLDFILE_VBITMASK;}

OctreeSegment* OctreePortionAwareBranch::pullaway(int x,int y,int z,int recur,OctreeSegment*& self) {
//    changed = true; it's this line here
    std::cout<<"THIS REQUIRES YOUR ATTENTION\n";
    if (depth<recur) {
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}
OctreePortionAwareBranch* OctreePortionAwareBranch::getvoxunit(int x,int y,int z) {
//    if (this==NULL) throw;
    return this;
}
OctreeSegment* OctreeFeature::indivref(int x,int y,int z) {
//    if (this==NULL) throw;
    return this;
}
OctreeSegment* OctreeBud::indivref(int x,int y,int z) {
//    if (this==NULL) throw;
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
    (subdivisions[0][0][0]->giveimconflag()&(17*(8+0)) )|
    (subdivisions[1][0][0]->giveimconflag()&(17*(8+1)) )|
    (subdivisions[0][1][0]->giveimconflag()&(17*(8+2)) )|
    (subdivisions[1][1][0]->giveimconflag()&(17*(8+3)) )|
    (subdivisions[0][0][1]->giveimconflag()&(17*(8+4)) )|
    (subdivisions[1][0][1]->giveimconflag()&(17*(8+5)) )|
    (subdivisions[0][1][1]->giveimconflag()&(17*(8+6)) )|
    (subdivisions[1][1][1]->giveimconflag()&(17*(8+7)) );
}
OctreeBranch::OctreeBranch(OctreeSegment* a,OctreeSegment* b,
                           OctreeSegment* c,OctreeSegment* d,
                           OctreeSegment* e,OctreeSegment* f,
                           OctreeSegment* g,OctreeSegment* h, int dep,glm::vec3 pof,glm::vec3 nof) : depth(dep) {
    
    
    point = (int)g_vertecies[depth+1].size();
    g_vertecies[depth+1].push_back(pof);
    g_normals[depth+1].push_back(nof);
//    g_vertecies[depth+1].push_back(q.evaluate(depth+1)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0)));
//    point = glm::vec3(x*(1<<depth)/128.0,y*(1<<depth)/128.0,z*(1<<depth)/128.0);
//
    
    subdivisions[0][0][0] = a;
    subdivisions[1][0][0] = b;
    subdivisions[0][1][0] = c;
    subdivisions[1][1][0] = d;
    subdivisions[0][0][1] = e;
    subdivisions[1][0][1] = f;
    subdivisions[0][1][1] = g;
    subdivisions[1][1][1] = h;
    connections =
    (subdivisions[0][0][0]->giveimconflag()&(17*(8+0)) )|
    (subdivisions[1][0][0]->giveimconflag()&(17*(8+1)) )|
    (subdivisions[0][1][0]->giveimconflag()&(17*(8+2)) )|
    (subdivisions[1][1][0]->giveimconflag()&(17*(8+3)) )|
    (subdivisions[0][0][1]->giveimconflag()&(17*(8+4)) )|
    (subdivisions[1][0][1]->giveimconflag()&(17*(8+5)) )|
    (subdivisions[0][1][1]->giveimconflag()&(17*(8+6)) )|
    (subdivisions[1][1][1]->giveimconflag()&(17*(8+7)) );
}
OctreeBranch::OctreeBranch(BlockId a,int dep,glm::vec3 pof,glm::vec3 nof,uint8_t conn) : depth(dep),connections(conn) {
//    point = glm::vec3(x*(1<<depth)/128.0,y*(1<<depth)/128.0,z*(1<<depth)/128.0);
    point = (int)g_vertecies[depth+1].size();
    g_vertecies[depth+1].push_back(pof);
    g_normals[depth+1].push_back(nof);
    subdivisions[0][0][0] = new OctreeBud(a);
    subdivisions[1][0][0] = new OctreeBud(a);
    subdivisions[0][1][0] = new OctreeBud(a);
    subdivisions[1][1][0] = new OctreeBud(a);
    subdivisions[0][0][1] = new OctreeBud(a);
    subdivisions[1][0][1] = new OctreeBud(a);
    subdivisions[0][1][1] = new OctreeBud(a);
    subdivisions[1][1][1] = new OctreeBud(a);
}

OctreePortionAwareBranch* OctreeBranch::getvoxunit(int x,int y,int z) {
    return subdivisions XYZINDEX->getvoxunit(x,y,z);
}
OctreeSegment* OctreeBranch::indivref(int x,int y,int z) {
    return subdivisions XYZINDEX->indivref(x,y,z);
}

BlockId OctreeBranch::getser(int x,int y,int z) {
    return subdivisions XYZINDEX->getser(x,y,z);
}
Edgedat& OctreeBranch::xcon(int x,int y,int z) {
    return subdivisions XYZINDEX->xcon(x,y,z);
}
Edgedat& OctreeBranch::ycon(int x,int y,int z) {
    return subdivisions XYZINDEX->ycon(x,y,z);
}
Edgedat& OctreeBranch::zcon(int x,int y,int z) {
    return subdivisions XYZINDEX->zcon(x,y,z);
}
int OctreeBranch::feat(int x,int y,int z) {
    if (depth<g_lod) return point;
    else return subdivisions XYZINDEX->feat(x,y,z);
}
glm::vec3& OctreeBranch::getfeature(int x,int y,int z) {
    return subdivisions XYZINDEX->getfeature(x,y,z);
}
glm::vec3& OctreeBranch::getnormal(int x,int y,int z) {
    return subdivisions XYZINDEX->getnormal(x,y,z);
}
glm::vec3& OctreePortionAwareBranch::getfeature(int x,int y,int z) {
    return vertecies[g_lod][feat(x,y,z)];
}
glm::vec3& OctreePortionAwareBranch::getnormal(int x,int y,int z) {
    return normals[g_lod][feat(x,y,z)];
}
void OctreeBranch::insertinto(int x,int y,int z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==thisdep) {
        delete self;//possible memory leak; todo:make branches delete their children in deconstructors
        self = toinsert;
    } else {
        subdivisions XYZINDEX->insertinto(x,y,z,recur,depth,toinsert,subdivisions XYZINDEX);
    }
}

int OctreeSegment::getlod(int x,int y,int z) {return 0;}
int OctreeBranch::getlod(int x,int y,int z) {
    return subdivisions XYZINDEX->getlod(x,y,z);
}
int OctreePortionAwareBranch::getlod(int x,int y,int z) {
    return lod;
}

OctreeSegment* OctreeBranch::pullaway(int x,int y,int z,int recur,OctreeSegment*& self) {
    if (depth<recur) {
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}
#define ACCD(x,y,z) g_idfield[x+y*(CHSIZE+1)+z*(CHSIZE+1)*(CHSIZE+1)]
OctreeSegment* makeOctree(int x,int y,int z,int recur) {
    extern uint8_t materialattribs[];
    if (recur == 0) {
        uint8_t equi = ACCD(x,y,z);
        uint8_t meshcomp = (materialattribs[ACCD(x+1,y+1,z+1)]&1);
        uint8_t volumecomp = (materialattribs[equi]&1);
        
        uint8_t connectionflag =
        ((meshcomp^(materialattribs[ACCD(x  ,y  ,z  )]&1))*(8) ) |
        ((meshcomp^(materialattribs[ACCD(x+1,y  ,z  )]&1))*(1+8+16) ) |
        ((meshcomp^(materialattribs[ACCD(x  ,y+1,z  )]&1))*(2+8+32) ) |
        ((meshcomp^(materialattribs[ACCD(x  ,y  ,z+1)]&1))*(4+8+64) ) |
        ((meshcomp^(materialattribs[ACCD(x  ,y+1,z+1)]&1))*(2+4+8+32+64) ) |
        ((meshcomp^(materialattribs[ACCD(x+1,y  ,z+1)]&1))*(1+4+8+16+64) ) |
        ((meshcomp^(materialattribs[ACCD(x+1,y+1,z  )]&1))*(1+2+8+16+32) );
        if ((connectionflag&8)|volumecomp) {
            connectionflag |=
            ((materialattribs[ACCD(x+1,y,z)]&1)*16) |
            ((materialattribs[ACCD(x,y+1,z)]&1)*32) |
            ((materialattribs[ACCD(x,y,z+1)]&1)*64) |
            128;
        }
        
        if (equi==ACCD(x+1,y,z) and equi==ACCD(x,y+1,z) and equi==ACCD(x,y,z+1)) {
            if (equi==ACCD(x+1,y+1,z) and equi==ACCD(x+1,y,z+1) and equi==ACCD(x,y+1,z+1) and equi==ACCD(x+1,y+1,z+1)) {
                return new OctreeBud(ACCD(x,y,z));
            } else {
                return new OctreeFeature(ACCD(x,y,z),connectionflag);
            }
        } else {
            return new OctreeLeaf(ACCD(x,y,z),connectionflag);
        }
    }
    int add = (1<<recur)+1;
    BlockId equi = ACCD(x,y,z);
    for (int xi=x;xi<x+add;xi++) {
        for (int yi=y;yi<y+add;yi++) {
            for (int zi=z;zi<z+add;zi++) {
                if (ACCD(xi,yi,zi) != equi) {
                    int k = (1<<(recur-1));
                    OctreeSegment* a = makeOctree(x  ,y  ,z  ,recur-1);
                    OctreeSegment* b = makeOctree(x+k,y  ,z  ,recur-1);
                    OctreeSegment* c = makeOctree(x  ,y+k,z  ,recur-1);
                    OctreeSegment* d = makeOctree(x+k,y+k,z  ,recur-1);
                    OctreeSegment* e = makeOctree(x  ,y  ,z+k,recur-1);
                    OctreeSegment* f = makeOctree(x+k,y  ,z+k,recur-1);
                    OctreeSegment* g = makeOctree(x  ,y+k,z+k,recur-1);
                    OctreeSegment* h = makeOctree(x+k,y+k,z+k,recur-1);
                    if (recur == CHPOWER) {
                        OctreePortionAwareBranch* opab = new OctreePortionAwareBranch(a,b,c,d,e,f,g,h,true);
                        opab->vertecies = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                        opab->normals = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                        return opab;
                    }
                    else {return new OctreeBranch(a,b,c,d,e,f,g,h,recur-1);}
                }
            }
        }
    }
    return new OctreeBud(equi);
}
#undef ACCD
//Octree::Octree(std::string name,glm::mat4& trans,Environment& backref,int pop) : matrixRef(trans), currenttests(backref), popular(pop), depth(CHPOWER) {
//    std::ifstream file = std::ifstream(SAVEDIR "/"+name+"/massfile",std::ios::in|std::ios::binary);
//    if (file) {
//        file.read((char*)&depth,sizeof(int));
//        data = loadWorldFile(file,depth);
//        file.close();
//    } else {
//        data = new OctreeBud(pop);
//    }
//}
//bool OctreeSegment::datapsuedoat(BlockLoc x,BlockLoc y,BlockLoc z) {
//    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
//    if (vox==NULL) {
//        return this->getser(x,y,z)!=0;
//    } else {
//        return true;//vox->newlod>=0 and (vox->newlod>=MIN_WORLDFILE_GEOMSAVE or (vox->prepared and vox->hardLoaded));
//    }
//}




bool OctreeSegment::vertsloadedat(int x,int y,int z) {//used to determine if surrounding data is available for geomify.
    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
    if (vox==NULL) return this->getser(x,y,z)!=0;
    else {
        if (vox->lod<MIN_WORLDFILE_GEOMSAVE) {return vox->prepared;}
        else return vox->prepared or (vox->softload and !vox->hardload);
    }
}
bool OctreeSegment::serloadedat(int x,int y,int z) {//used to determine if surroung data is available for prepare.
    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
    if (vox==NULL) return this->getser(x,y,z)!=0;
    else {
        return vox->hardload;
    }
}




