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

OctreePortionAwareBranch* OctreeSegment::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {return NULL;}
OctreeSegment* OctreeSegment::indivref(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
BlockId OctreeSegment::getser(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
Edgedat& OctreeSegment::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
Edgedat& OctreeSegment::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
Edgedat& OctreeSegment::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return silence;}
//glm::vec3& OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod) {throw;}
int OctreeSegment::feat(BlockLoc x,BlockLoc y,BlockLoc z) {return 0;}
glm::vec3& OctreeSegment::getfeature(BlockLoc x,BlockLoc y,BlockLoc z) {return errorpoint;}
glm::vec3& OctreeSegment::getnormal(BlockLoc x,BlockLoc y,BlockLoc z) {return errorpoint;}


void OctreeSegment::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {throw;}
OctreeSegment* OctreeSegment::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {self = new OctreeBud(1);return this;}

OctreeFeature::OctreeFeature(BlockId fill,uint8_t conn) : fillvalue(fill) , conflag(conn) {}
BlockId OctreeFeature::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}
//glm::vec3& OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z,int lod)  {return point;}//.get(lod,x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)));}
int OctreeFeature::feat(BlockLoc x,BlockLoc y,BlockLoc z)  {return point;}//.get(lod,x-(ASBLOCKLOC(0)),y-(ASBLOCKLOC(0)),z-(ASBLOCKLOC(0)));}


OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn) : OctreeFeature(fill,conn) {}
OctreeLeaf::OctreeLeaf(BlockId fill,uint8_t conn,int8_t n[12]) : OctreeFeature(fill,conn),
    xcondat(n[0],n[1],n[2],n[3]),
    ycondat(n[4],n[5],n[6],n[7]),
    zcondat(n[8],n[9],n[10],n[11])  {}

Edgedat& OctreeLeaf::xcon(BlockLoc x,BlockLoc y,BlockLoc z) {return xcondat;}
Edgedat& OctreeLeaf::ycon(BlockLoc x,BlockLoc y,BlockLoc z) {return ycondat;}
Edgedat& OctreeLeaf::zcon(BlockLoc x,BlockLoc y,BlockLoc z) {return zcondat;}

OctreeBud::OctreeBud(BlockId fill) : fillvalue(fill) {}

BlockId OctreeBud::getser(BlockLoc x,BlockLoc y,BlockLoc z) {return fillvalue;}

void OctreeBud::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
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

OctreeSegment* OctreePortionAwareBranch::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {
//    changed = true; it's this line here
    std::cout<<"THIS REQUIRES YOUR ATTENTION\n";
    if (depth<recur) {
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}
OctreePortionAwareBranch* OctreePortionAwareBranch::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {
//    if (this==NULL) throw;
    return this;
}
OctreeSegment* OctreeFeature::indivref(BlockLoc x,BlockLoc y,BlockLoc z) {
//    if (this==NULL) throw;
    return this;
}
OctreeSegment* OctreeBud::indivref(BlockLoc x,BlockLoc y,BlockLoc z) {
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

OctreePortionAwareBranch* OctreeBranch::getvoxunit(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getvoxunit(x,y,z);
}
OctreeSegment* OctreeBranch::indivref(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->indivref(x,y,z);
}

BlockId OctreeBranch::getser(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getser(x,y,z);
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
int OctreeBranch::feat(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (depth<g_lod) return point;
    else return subdivisions XYZINDEX->feat(x,y,z);
}
glm::vec3& OctreeBranch::getfeature(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getfeature(x,y,z);
}
glm::vec3& OctreeBranch::getnormal(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getnormal(x,y,z);
}
glm::vec3& OctreePortionAwareBranch::getfeature(BlockLoc x,BlockLoc y,BlockLoc z) {
    return vertecies[g_lod][feat(x,y,z)];
}
glm::vec3& OctreePortionAwareBranch::getnormal(BlockLoc x,BlockLoc y,BlockLoc z) {
    return normals[g_lod][feat(x,y,z)];
}
void OctreeBranch::insertinto(BlockLoc x,BlockLoc y,BlockLoc z,int recur,int thisdep,OctreeSegment* toinsert,OctreeSegment*& self) {
    if (recur==thisdep) {
        delete self;//possible memory leak; todo:make branches delete their children in deconstructors
        self = toinsert;
    } else {
        subdivisions XYZINDEX->insertinto(x,y,z,recur,depth,toinsert,subdivisions XYZINDEX);
    }
}

int OctreeSegment::getlod(BlockLoc x,BlockLoc y,BlockLoc z) {return 0;}
int OctreeBranch::getlod(BlockLoc x,BlockLoc y,BlockLoc z) {
    return subdivisions XYZINDEX->getlod(x,y,z);
}
int OctreePortionAwareBranch::getlod(BlockLoc x,BlockLoc y,BlockLoc z) {
    return lod;
}

OctreeSegment* OctreeBranch::pullaway(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment*& self) {
    if (depth<recur) {
        self = new OctreeBud(1);
        return this;
    } else {
        return subdivisions XYZINDEX->pullaway(x,y,z,recur,subdivisions XYZINDEX);
    }
}

OctreeSegment* makeOctree(BlockId (*data)[CHSIZE+1][CHSIZE+1],int x,int y,int z,int recur) {
    extern uint8_t materialattribs[];
    if (recur == 0) {
        uint8_t equi = data[x][y][z];
        uint8_t meshcomp = (materialattribs[data[x+1][y+1][z+1]]&1);
        uint8_t volumecomp = (materialattribs[equi]&1);
        
        uint8_t connectionflag =
        ((meshcomp^(materialattribs[data[x  ][y  ][z  ]]&1))*(8) ) |
        ((meshcomp^(materialattribs[data[x+1][y  ][z  ]]&1))*(1+8+16) ) |
        ((meshcomp^(materialattribs[data[x  ][y+1][z  ]]&1))*(2+8+32) ) |
        ((meshcomp^(materialattribs[data[x  ][y  ][z+1]]&1))*(4+8+64) ) |
        ((meshcomp^(materialattribs[data[x  ][y+1][z+1]]&1))*(2+4+8+32+64) ) |
        ((meshcomp^(materialattribs[data[x+1][y  ][z+1]]&1))*(1+4+8+16+64) ) |
        ((meshcomp^(materialattribs[data[x+1][y+1][z  ]]&1))*(1+2+8+16+32) );
        if ((connectionflag&8)|volumecomp) {
            connectionflag |=
            ((materialattribs[data[x+1][y][z]]&1)*16) |
            ((materialattribs[data[x][y+1][z]]&1)*32) |
            ((materialattribs[data[x][y][z+1]]&1)*64) |
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
bool OctreeSegment::datapsuedoat(BlockLoc x,BlockLoc y,BlockLoc z) {
    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
    if (vox==NULL) {
        return this->getser(x,y,z)!=0;
    } else {
        return true;//vox->newlod>=0 and (vox->newlod>=MIN_WORLDFILE_GEOMSAVE or (vox->prepared and vox->hardLoaded));
    }
}
bool OctreeSegment::dataloadedat(BlockLoc x,BlockLoc y,BlockLoc z) {
    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
    if (vox==NULL) {
        return this->getser(x,y,z)!=0;
    } else {
        if (vox->lod<MIN_WORLDFILE_GEOMSAVE) {return vox->prepared;}
        else return !vox->hardload;
//        return vox->prepared;//vox->newlod>=0 and (vox->newlod>=MIN_WORLDFILE_GEOMSAVE or (vox->prepared and vox->hardLoaded));
    }
}
bool OctreeSegment::dataexistsat(BlockLoc x,BlockLoc y,BlockLoc z) {
    OctreePortionAwareBranch* vox=this->getvoxunit(x,y,z);
    if (vox==NULL) {
        return this->getser(x,y,z)!=0;
    } else {
        return vox->hardload;
    }
}




