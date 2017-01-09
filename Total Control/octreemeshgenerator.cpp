//
//  octreemeshgenerator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"


const int lodlimits[] = {
20,
40,
50,
80
};
const int levelsofdetail = 4;

void MaterialPolyCount::addoffsets(int flag,std::map<uint8_t,int>* offsets) {
    if (flag) {
        data.nextpoint[0].addoffsets(0,offsets);
        data.nextpoint[1].addoffsets(descriptor.flags,offsets);
    } else {
        if (descriptor.ids[0]) {
            (*offsets)[descriptor.ids[0]]+=data.sizes[0];
            if (descriptor.ids[1]) {
                (*offsets)[descriptor.ids[1]]+=data.sizes[1];
            }
        }
    }
}
int MaterialPolyCount::add(int flag,uint8_t key,int value) {
    if (flag) {
        if        (data.nextpoint[0].descriptor.ids[0]==key) {
            data.nextpoint[0].data.sizes[0] += value;
        } else if (data.nextpoint[0].descriptor.ids[1]==key) {
            data.nextpoint[0].data.sizes[1] += value;
        } else {
            descriptor.flags |= data.nextpoint[1].add(descriptor.flags,key,value);
        }
    } else {
        if        (descriptor.ids[0]==key) {
            data.sizes[0] += value;
        } else if (descriptor.ids[1]==key) {
            data.sizes[1] += value;
        } else if (descriptor.ids[0]==0) {
            descriptor.ids[0]=key;
            data.sizes[0]=value;
        } else if (descriptor.ids[1]==0) {
            descriptor.ids[1]=key;
            data.sizes[1]=value;
        } else {
            data.nextpoint = new MaterialPolyCount[2] {MaterialPolyCount(*this),MaterialPolyCount()};
            descriptor.flags = 0;
            data.nextpoint[1].add(0,key,value);
            return 8;
        }
    }
    return 0;
}
int MaterialPolyCount::jointo(int flag,int oflag,MaterialPolyCount* other) {
    if (flag) {
        oflag |= data.nextpoint[0].jointo(0,oflag,other);
        oflag |= data.nextpoint[1].jointo(descriptor.flags,oflag,other);
    } else {
        if (descriptor.ids[0]) {
            oflag |= other->add(oflag,descriptor.ids[0],data.sizes[0]);
            if (descriptor.ids[1]) {
                oflag |= other->add(oflag,descriptor.ids[1],data.sizes[1]);
            }
        }
    }
    return oflag;
}
int MaterialPolyCount::get(int flag,uint8_t key) {
    if (flag) {
        if        (data.nextpoint[0].descriptor.ids[0]==key) {
            return data.nextpoint[0].data.sizes[0];
        } else if (data.nextpoint[0].descriptor.ids[1]==key) {
            return data.nextpoint[0].data.sizes[1];
        } else {
            return data.nextpoint[1].get(descriptor.flags,key);
        }
    } else {
        if        (descriptor.ids[0]==key) {
            return data.sizes[0];
        } else if (descriptor.ids[1]==key) {
            return data.sizes[1];
        }
        return 0;
    }
}
void MaterialPolyCount::destroy(int flag) {
    if (flag) {
        data.nextpoint[1].destroy(descriptor.flags);
        delete[] data.nextpoint;
    }
}

void OctreeSegment::determinelod(BlockLoc x,BlockLoc y,BlockLoc z,BlockLoc px,BlockLoc py,BlockLoc pz,int skips,bool keepeval) {}
void OctreeBranch::determinelod(BlockLoc x,BlockLoc y,BlockLoc z,BlockLoc px,BlockLoc py,BlockLoc pz,int skips,bool keepeval) {
    extern const int levelsofdetail;
    extern const int lodlimits[];
    lodflags1&=~1;
    if (skips>0) {
        skips--;
        BlockLoc mask = 1<<depth;
        subdivisions[0][0][0]->determinelod(x     ,y     ,z     ,px,py,pz,skips,keepeval);
        subdivisions[1][0][0]->determinelod(x|mask,y     ,z     ,px,py,pz,skips,keepeval);
        subdivisions[0][1][0]->determinelod(x     ,y|mask,z     ,px,py,pz,skips,keepeval);
        subdivisions[1][1][0]->determinelod(x|mask,y|mask,z     ,px,py,pz,skips,keepeval);
        subdivisions[0][0][1]->determinelod(x     ,y     ,z|mask,px,py,pz,skips,keepeval);
        subdivisions[1][0][1]->determinelod(x|mask,y     ,z|mask,px,py,pz,skips,keepeval);
        subdivisions[0][1][1]->determinelod(x     ,y|mask,z|mask,px,py,pz,skips,keepeval);
        subdivisions[1][1][1]->determinelod(x|mask,y|mask,z|mask,px,py,pz,skips,keepeval);
    }
    if (keepeval) {
        lodflags1|=1;
        return;
    }
    float distance = sqrtf((x-px)*(x-px) + (y-py)*(y-py) + (z-pz)*(z-pz));
    if (depth<levelsofdetail and lodlimits[depth]<depth) {
        lodflags1|=1;
        return;
    }
    float rad = 1.73205080757 * (1<<depth);
    float gdist = rad+distance;
    float sdist = rad-distance;
    int k;
    for (k=1;depth-k>=0 and (depth-k>=levelsofdetail or lodlimits[depth-k]>gdist);k++) {}
    keepeval = depth-k==-1 or lodlimits[depth-k]<sdist;
    skips = k-1;
    BlockLoc mask = 1<<depth;
    subdivisions[0][0][0]->determinelod(x     ,y     ,z     ,px,py,pz,skips,keepeval);
    subdivisions[1][0][0]->determinelod(x|mask,y     ,z     ,px,py,pz,skips,keepeval);
    subdivisions[0][1][0]->determinelod(x     ,y|mask,z     ,px,py,pz,skips,keepeval);
    subdivisions[1][1][0]->determinelod(x|mask,y|mask,z     ,px,py,pz,skips,keepeval);
    subdivisions[0][0][1]->determinelod(x     ,y     ,z|mask,px,py,pz,skips,keepeval);
    subdivisions[1][0][1]->determinelod(x|mask,y     ,z|mask,px,py,pz,skips,keepeval);
    subdivisions[0][1][1]->determinelod(x     ,y|mask,z|mask,px,py,pz,skips,keepeval);
    subdivisions[1][1][1]->determinelod(x|mask,y|mask,z|mask,px,py,pz,skips,keepeval);
}

void MaterialPolyCount::deletesegment(int flag,std::map<uint8_t,GeomTerrain>* geometry,std::map<uint8_t,int>* offsets) {
    if (flag) {
        data.nextpoint[0].deletesegment(0,geometry,offsets);
        data.nextpoint[1].deletesegment(descriptor.flags,geometry,offsets);
    } else {
        if (descriptor.ids[0]) {
            (*geometry)[descriptor.ids[0]].erase((*offsets)[descriptor.ids[0]],data.sizes[0]);
            if (descriptor.ids[1]) {
                (*geometry)[descriptor.ids[1]].erase((*offsets)[descriptor.ids[1]],data.sizes[1]);
            }
        }
    }
}







MaterialPolyCountPackage OctreeSegment::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,std::map<uint8_t,int>* offsets,OctreeSegment* world,bool reprexist) {
    return MaterialPolyCountPackage();
}

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

MaterialPolyCountPackage OctreeLeaf::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,std::map<uint8_t,int>* offsets,OctreeSegment* world,bool reprexist) {
    MaterialPolyCount* due = new MaterialPolyCount();
    int d=0;
    Xstitch(x,y,z,geometry,world,fillvalue,world->getser(x+1,y,z),due,d);
    Ystitch(x,y,z,geometry,world,fillvalue,world->getser(x,y+1,z),due,d);
    Zstitch(x,y,z,geometry,world,fillvalue,world->getser(x,y,z+1),due,d);
    return MaterialPolyCountPackage(d,due);
}


MaterialPolyCountPackage OctreePortionAwareBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,std::map<uint8_t,int>* offsets,OctreeSegment* world,bool reprexist) {
    MaterialPolyCountPackage res = OctreeBranch::geomify(x,y,z,geometry,offsets,world,reprexist);
    if (res.flagalone&1) {
    
    }
    return res;
    
}



BranchRegistry* SWO;
MaterialPolyCountPackage OctreeBranch::geomify(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,std::map<uint8_t,int>* offsets,OctreeSegment* world,bool reprexist) {
    if (lodflags1&1) {
        int surlods[] = {
            world->getlod(x-1,y,z),
            world->getlod(x,y-1,z),
            world->getlod(x,y,z-1),
            world->getlod(x-1,y-1,z),
            world->getlod(x-1,y,z-1),
            world->getlod(x,y-1,z-1)
        };
        if (lodflags1&2 and reprexist) {
            if (((lodflags1>>8 )&31) == surlods[0] and
                ((lodflags1>>12)&31) == surlods[1] and
                ((lodflags2    )&31) == surlods[2] and
                ((lodflags2>>4 )&31) == surlods[3] and
                ((lodflags2>>8 )&31) == surlods[4] and
                ((lodflags2>>12)&31) == surlods[5]) {
                return MaterialPolyCountPackage(lodflags1&8,&materialpolys);
            }
        }
        lodflags1|=2;
        int lod = depth+1;
        if (reprexist) {
            materialpolys.deletesegment(lodflags1&8,geometry,offsets);
        }
        lodflags2 = (surlods[2])|(surlods[2]<<4)|(surlods[2]<<8)|(surlods[2]<<12);
        lodflags1 = (lodflags1&(511-8))|(surlods[0]<<8)|(surlods[1]<<12);
        materialpolys.destroy(lodflags1&8);
        materialpolys = MaterialPolyCount();
        
        int lodmin = std::min(std::min(std::min(surlods[0],surlods[1]),surlods[2]), std::min(std::min(surlods[3],surlods[4]),surlods[5]));
        if (lodmin>=lod) {
            uint8_t an  = getser(0,0,0);
            Xstitch(x,y,z,geometry,world,an,world->getser(x+(1<<lod),y,z),&materialpolys,lodflags1);
            Ystitch(x,y,z,geometry,world,an,world->getser(x,y+(1<<lod),z),&materialpolys,lodflags1);
            Zstitch(x,y,z,geometry,world,an,world->getser(x,y,z+(1<<lod)),&materialpolys,lodflags1);
        } else {
            for (int t=0;t<(1<<lod);t+=(1<<lodmin)) {
                Xstitch(x+t,y,z,geometry,world,getser(t,0,0),world->getser(x+t+(1<<lodmin),y,z),&materialpolys,lodflags1);
                Ystitch(x,y+t,z,geometry,world,getser(0,t,0),world->getser(x,y+t+(1<<lodmin),z),&materialpolys,lodflags1);
                Zstitch(x,y,z+t,geometry,world,getser(0,0,t),world->getser(x,y,z+t+(1<<lodmin)),&materialpolys,lodflags1);
                for (int i=(1<<lodmin);i<(1<<lod);i+=(1<<lodmin)) {
                    Xstitch(x+t,y+i,z,geometry,world,getser(t,i,0),world->getser(x+t+(1<<lodmin),y+i,z),&materialpolys,lodflags1);
                    Ystitch(x,y+t,z+i,geometry,world,getser(0,t,i),world->getser(x,y+t+(1<<lodmin),z+i),&materialpolys,lodflags1);
                    Zstitch(x+i,y,z+t,geometry,world,getser(i,0,t),world->getser(x+i,y,z+t+(1<<lodmin)),&materialpolys,lodflags1);
                    
                    Xstitch(x+t,y,z+i,geometry,world,getser(t,0,i),world->getser(x+t+(1<<lodmin),y,z+i),&materialpolys,lodflags1);
                    Ystitch(x+i,y+t,z,geometry,world,getser(i,t,0),world->getser(x+i,y+t+(1<<lodmin),z),&materialpolys,lodflags1);
                    Zstitch(x,y+i,z+t,geometry,world,getser(0,i,t),world->getser(x,y+i,z+t+(1<<lodmin)),&materialpolys,lodflags1);
                }
            }
        }
        return MaterialPolyCountPackage((lodflags1&8)|1,&materialpolys);
    } else {
        if (reprexist and lodflags1&2) {
            materialpolys.deletesegment(lodflags1&8,geometry,offsets);
            reprexist = false;
        }
        lodflags1&=~2;
        if (depth) {
            BlockLoc mask = 1<<depth;
            MaterialPolyCountPackage chs[] = {
                subdivisions[0][0][0]->geomify(x     ,y     ,z     ,geometry,offsets,world,reprexist),
                subdivisions[1][0][0]->geomify(x|mask,y     ,z     ,geometry,offsets,world,reprexist),
                subdivisions[0][1][0]->geomify(x     ,y|mask,z     ,geometry,offsets,world,reprexist),
                subdivisions[1][1][0]->geomify(x|mask,y|mask,z     ,geometry,offsets,world,reprexist),
                subdivisions[0][0][1]->geomify(x     ,y     ,z|mask,geometry,offsets,world,reprexist),
                subdivisions[1][0][1]->geomify(x|mask,y     ,z|mask,geometry,offsets,world,reprexist),
                subdivisions[0][1][1]->geomify(x     ,y|mask,z|mask,geometry,offsets,world,reprexist),
                subdivisions[1][1][1]->geomify(x|mask,y|mask,z|mask,geometry,offsets,world,reprexist)};
            if ((chs[0].flagalone&1)|(chs[1].flagalone&1)|(chs[2].flagalone&1)|(chs[3].flagalone&1)|
                (chs[4].flagalone&1)|(chs[5].flagalone&1)|(chs[6].flagalone&1)|(chs[7].flagalone&1)) {
                materialpolys.destroy(lodflags1&8);
                materialpolys = MaterialPolyCount();
                lodflags1&=~8;
                lodflags1|=chs[0].data->jointo(chs[0].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[1].data->jointo(chs[1].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[2].data->jointo(chs[2].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[3].data->jointo(chs[3].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[4].data->jointo(chs[4].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[5].data->jointo(chs[5].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[6].data->jointo(chs[6].flagalone,lodflags1&8,&materialpolys);
                lodflags1|=chs[7].data->jointo(chs[7].flagalone,lodflags1&8,&materialpolys);
                return MaterialPolyCountPackage((lodflags1&8)|1,&materialpolys);
            }
            return MaterialPolyCountPackage(lodflags1&8,&materialpolys);
        } else {
            int surlods[] = {
                world->getlod(x-1,y,z),
                world->getlod(x,y-1,z),
                world->getlod(x,y,z-1),
                world->getlod(x-1,y-1,z),
                world->getlod(x-1,y,z-1),
                world->getlod(x,y-1,z-1)
            };
            if (reprexist) {
                if (((lodflags1>>8 )&31) == surlods[0] and
                    ((lodflags1>>12)&31) == surlods[1] and
                    ((lodflags2    )&31) == surlods[2] and
                    ((lodflags2>>4 )&31) == surlods[3] and
                    ((lodflags2>>8 )&31) == surlods[4] and
                    ((lodflags2>>12)&31) == surlods[5]) {
                    return MaterialPolyCountPackage(lodflags1&8,&materialpolys);
                }
            }
            lodflags2 = (surlods[2])|(surlods[2]<<4)|(surlods[2]<<8)|(surlods[2]<<12);
            lodflags1 = (lodflags1&(511-8))|(surlods[0]<<8)|(surlods[1]<<12);
            materialpolys.destroy(lodflags1&8);
            materialpolys = MaterialPolyCount();
            BlockLoc mask = 1<<depth;
            MaterialPolyCountPackage chs[] = {
                subdivisions[0][0][0]->geomify(x     ,y     ,z     ,geometry,offsets,world,reprexist),
                subdivisions[1][0][0]->geomify(x|mask,y     ,z     ,geometry,offsets,world,reprexist),
                subdivisions[0][1][0]->geomify(x     ,y|mask,z     ,geometry,offsets,world,reprexist),
                subdivisions[1][1][0]->geomify(x|mask,y|mask,z     ,geometry,offsets,world,reprexist),
                subdivisions[0][0][1]->geomify(x     ,y     ,z|mask,geometry,offsets,world,reprexist),
                subdivisions[1][0][1]->geomify(x|mask,y     ,z|mask,geometry,offsets,world,reprexist),
                subdivisions[0][1][1]->geomify(x     ,y|mask,z|mask,geometry,offsets,world,reprexist),
                subdivisions[1][1][1]->geomify(x|mask,y|mask,z|mask,geometry,offsets,world,reprexist)};
            lodflags1|=chs[0].data->jointo(chs[0].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[1].data->jointo(chs[1].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[2].data->jointo(chs[2].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[3].data->jointo(chs[3].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[4].data->jointo(chs[4].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[5].data->jointo(chs[5].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[6].data->jointo(chs[6].flagalone,lodflags1&8,&materialpolys);
            lodflags1|=chs[7].data->jointo(chs[7].flagalone,lodflags1&8,&materialpolys);
            chs[0].data->destroy(chs[0].flagalone);chs[2].data->destroy(chs[2].flagalone);
            chs[4].data->destroy(chs[4].flagalone);chs[6].data->destroy(chs[6].flagalone);
            chs[1].data->destroy(chs[1].flagalone);chs[3].data->destroy(chs[3].flagalone);
            chs[5].data->destroy(chs[5].flagalone);chs[7].data->destroy(chs[7].flagalone);
            delete chs[0].data;delete chs[2].data;delete chs[4].data;delete chs[6].data;
            delete chs[1].data;delete chs[3].data;delete chs[5].data;delete chs[7].data;
            return MaterialPolyCountPackage((lodflags1&8)|1,&materialpolys);// |1 here to force update of polycount

        }
    }
}

void Xstitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anx,MaterialPolyCount* count,int& f) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[anx]) {
        glm::vec3 buf[] = {
            world->featwrt(x,y  ,z  ),
            world->featwrt(x,y-1,z  ),
            world->featwrt(x,y  ,z-1),
            world->featwrt(x,y-1,z-1)
        };
        if (materialprops[fillvalue]>materialprops[anx]) {
            f|=count->add(f&8,fillvalue,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {//if (materialprops[fillvalue]<materialprops[anx]) {
            f|=count->add(f&8,anx,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[anx].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}
void Ystitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId any,MaterialPolyCount* count,int& f) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[any]) {
        glm::vec3 buf[] = {
            world->featwrt(x  ,y,z  ),
            world->featwrt(x-1,y,z  ),
            world->featwrt(x  ,y,z-1),
            world->featwrt(x-1,y,z-1)
        };
        if (materialprops[fillvalue]>materialprops[any]) {
            f|=count->add(f&8,fillvalue,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[0][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[any]) {
            f|=count->add(f&8,any,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[any].addVert(buf[gtable[1][ik]]);
            }
        }
    }
}
void Zstitch(BlockLoc x,BlockLoc y,BlockLoc z,std::map<uint8_t,GeomTerrain>* geometry,OctreeSegment* world,BlockId fillvalue,BlockId anz,MaterialPolyCount* count,int& f) {
    extern uint8_t gtable[2][6];
    extern uint8_t materialprops[];
    if (materialprops[fillvalue]!=materialprops[anz]) {
        glm::vec3 buf[] = {
            world->featwrt(x  ,y  ,z),
            world->featwrt(x-1,y  ,z),
            world->featwrt(x  ,y-1,z),
            world->featwrt(x-1,y-1,z)
        };
        if (materialprops[fillvalue]>materialprops[anz]) {
            f|=count->add(f&8,fillvalue,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[fillvalue].addVert(buf[gtable[1][ik]]);
            }
        } else {// if (materialprops[fillvalue]<materialprops[anz]) {
            f|=count->add(f&8,anz,2);
            for (int ik=0;ik<6;ik++) {
                (*geometry)[anz].addVert(buf[gtable[0][ik]]);
            }
        }
    }
}



//void MaterialPolyCountPackage::add(uint8_t key,int value) {flagalone |= add(flagalone&8,key,value);}
MaterialPolyCountPackage::MaterialPolyCountPackage() {}
MaterialPolyCountPackage::MaterialPolyCountPackage(int f,MaterialPolyCount* g) : flagalone(f) , data(g) {}


