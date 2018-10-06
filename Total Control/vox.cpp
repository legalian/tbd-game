//
//  vox.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/21/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "vox.h"

Structure::Structure(std::string name,Environment& backref,bool load) : structureid(name),loadin(load),currenttests(backref),popular((int)!load),depth(CHPOWER) {
    int pop=(int)!load;
    std::ifstream file = std::ifstream(SAVEDIR "/"+name+"/massfile",std::ios::in|std::ios::binary);
    if (file) {
        file.read((char*)&depth,sizeof(int));
        data = loadWorldFile(file,ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(0),depth);
        file.close();
    } else {
        data = new OctreeBud(pop);
    }
    expand(CHPOWER);
}
Structure::~Structure() {
    delete source;
}

int Structure::underpressure(int x,int y,int z) {
    x ^= ASBLOCKLOC(0);
    y ^= ASBLOCKLOC(0);
    z ^= ASBLOCKLOC(0);
    int r;
    for (r=0;(1<<r)<=x or (1<<r)<=y or (1<<r)<=z;r++) {}
    return r;
}
void Structure::expand(int x,int y,int z) {throw;expand(underpressure(x,y,z));}
void Structure::expandchunk(int x,int y,int z) {
    expand(std::max(underpressure(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z)),
                    underpressure(ASCHUNKLOC(x+1)-1,ASCHUNKLOC(y+1)-1,ASCHUNKLOC(z+1)-1)));
}
void Structure::expandarbit(int x,int y,int z,int recur) {
    expand(std::max(underpressure(x,y,z),
                    underpressure(x+(1<<recur)-1,y+(1<<recur)-1,z+(1<<recur)-1)));
}
void Structure::expand(int desireddepth) {
    while (desireddepth>=depth+1) {
        depth++;
        if (depth!=CHPOWER) {
            if (depth&1) {
                data = new OctreeBranch(new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),data                  ,depth-1);
            } else {
                data = new OctreeBranch(data                  ,new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),
                                        new OctreeBud(popular),new OctreeBud(popular),depth-1);
            }
        } else {
            if (depth&1) {
                data = new OctreePortionAwareBranch(new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),data                  ,true);
            } else {
                data = new OctreePortionAwareBranch(data                  ,new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),
                                                    new OctreeBud(popular),new OctreeBud(popular),true);
            }
        }
    }
}

void Structure::render() {
    setdefaultmatrix(&transform);
//    std::set<Location> updates;
    g_world = data;
//    OctreeSegment::m_nup = 0;
//    OctreeSegment::suppliment=NULL;

//    OctreeSegment::m_updates = &updates;
    data->geomify(ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(0));
//    if (OctreeSegment::m_nup==0 and OctreeSegment::suppliment!=NULL) {
//        OctreeSegment::suppliment->geometry.clear();
//        OctreeSegment::m_geometry = &OctreeSegment::suppliment->geometry;
//        OctreeSegment::suppliment->updatevisual(OctreeSegment::m_x1,OctreeSegment::m_y1,OctreeSegment::m_z1);
//    }
}
void Structure::updatelod(double x, double y, double z) {
    glm::vec4 epicenter = glm::inverse(transform)*glm::vec4(x,y,z,1);
    Location newloc = Location((int)(epicenter.x),
                               (int)(epicenter.y),
                               (int)(epicenter.z));
    data->determinelod(ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(newloc.x),ASBLOCKLOC(newloc.y),ASBLOCKLOC(newloc.z),false,data);
}
void Structure::updatequeue(double x, double y, double z) {
    glm::vec4 epicenter = glm::inverse(transform)*glm::vec4(x,y,z,1);
    Location newloc = Location(TRUNC_DIV((int)(epicenter.x),CHSIZE),
                               TRUNC_DIV((int)(epicenter.y),CHSIZE),
                               TRUNC_DIV((int)(epicenter.z),CHSIZE));
    if (not (cameraloc==newloc)) {
        cameraloc = newloc;
        loadstage = 0;
    }
    for (int i=loadstage*2;i<(loadstage+1)*2;i++) {
        if (i==0) {
            if (attain(newloc,newloc)){return;}
        }
        else {
            for (int i1=-1*i;i1<=i;i1++) {
                for (int i2=-1*i;i2<=i;i2++) {
                    if (attain(Location(newloc.x+i1,newloc.y+i2,newloc.z+i),newloc)) {return;}
                    if (attain(Location(newloc.x+i1,newloc.y+i2,newloc.z-i),newloc)) {return;}
                }
                for (int i2=-1*i+1;i2<=i-1;i2++) {
                    if (attain(Location(newloc.x+i1,newloc.z+i,newloc.y+i2),newloc)) {return;}
                    if (attain(Location(newloc.x+i1,newloc.z-i,newloc.y+i2),newloc)) {return;}
                }
            }
            for (int i1=-1*i+1;i1<=i-1;i1++) {
                for (int i2=-1*i+1;i2<=i-1;i2++) {
                    if (attain(Location(newloc.z+i,newloc.x+i1,newloc.y+i2),newloc)) {return;}
                    if (attain(Location(newloc.z-i,newloc.x+i1,newloc.y+i2),newloc)) {return;}
                }
            }
        }
    }
//    
//    PathTesterBucket** c = &geomloaded.buckets[loadstage];
//    while (*c!=NULL) {
//        int lod =geomloaded.hash(newloc,(*c)->node)/2;
//        if (lod!=loadstage) {
//            Location testme = (*c)->node;
//            PathTesterBucket* recycle = *c;
//            *c=(*c)->next;
//            delete recycle;
//            geomloaded.add(testme,lod);
//            if (attain(testme)) {return;}
//        } else {
//            c = &((*c)->next);
//        }
//    }
    loadstage++;
}
OctreeSegment* Structure::loadportion(int x,int y,int z,BlockId *dat) {
    expandchunk(x,y,z);
    OctreeSegment* sg = makeOctree(dat,0,0,0,CHPOWER);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,depth,sg,data);
    return sg;
}
void Structure::filepushportion(std::string filebase,int x,int y,int z) {
    std::ofstream file = std::ofstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::out|std::ios::binary|std::ios::trunc);
    OctreePortionAwareBranch* look = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
    s_file = &file;
    if (look!=NULL) {
        look->filesave();
    } else {
        OctreeBud(data->getser(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))).filesave();
    }
    file.close();
}
void Structure::filepullportion(std::string filebase,int x,int y,int z) {
    std::ifstream file = std::ifstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::in|std::ios::binary);
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,depth,makeOctree(file,CHPOWER),data);
    file.close();
}
bool Structure::attain(Location pos,Location ppos) {
    bool load = false;
    bool generate = false;
    
//    extern const int levelsofdetail;
    extern const int lodlimits[];
    int mask = CHSIZE/2;
    float distance = sqrtf(((pos.x-ppos.x)*CHSIZE+mask)*((pos.x-ppos.x)*CHSIZE+mask) + ((pos.y-ppos.y)*CHSIZE+mask)*((pos.y-ppos.y)*CHSIZE+mask) + ((pos.z-ppos.z)*CHSIZE+mask)*((pos.z-ppos.z)*CHSIZE+mask));
    int lod;
    for(lod=0;lodlimits[lod]<distance&&lod<=MAX_WORLDFILE_GEOMSAVE;lod++) {}
    
    
    if (underpressure(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z))>depth) {
        generate = !popular;//bud, this is id
    } else {
        OctreePortionAwareBranch* grably = data->getvoxunit(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));
        if (grably==NULL) {
            generate = !data->getser(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));//bud, this is id
        } else {
            load = !(grably->hardload or (lod>=MIN_WORLDFILE_GEOMSAVE and lod<=MAX_WORLDFILE_GEOMSAVE));
        }
    }
    if (load){
        filepullportion(SAVEDIR "/"+structureid,pos.x,pos.y,pos.z);
    }
    if (generate) {
        source->populate(pos.x,pos.y,pos.z,*this);
    }
    OctreePortionAwareBranch* unit = data->getvoxunit(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));
    
    if (unit!=NULL) {
//        if (generate) {
//            filepushportion(SAVEDIR "/"+structureid,pos.x,pos.y,pos.z);
//        }
//        if (load or generate) {
//            unit->testconnected(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z),data,&currenttests);
//            currenttests.prune(data);
//        }
    }
    g_world=data;
    if (depth>CHPOWER+1) {
        data->prepare(ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(0));
    }
    if (load or generate) {
        std::ofstream file = std::ofstream(SAVEDIR "/"+structureid+"/massfile",std::ios::out|std::ios::binary|std::ios::trunc);
        file.write((char*)&(depth),sizeof(int));
        s_file = &file;
        data->worldfilesave(ASBLOCKLOC(0),ASBLOCKLOC(0),ASBLOCKLOC(0));
        file.close();
    }
    return load or generate;
}
//void Structure::render() {
////    if (structureid!="test") {return;}
//    
//    world.render();
//}
//#include "lookuptables.cpp"
//
//inline int MOD(int n,int m) {
//    return ((n%m)+m)%m;
//}
//inline int TRUNC_DIV(int n, int m) {
//    return (n-MOD(n,m))/m;
//}
Location::Location(int qx, int qy, int qz) {
    x = qx;
    y = qy;
    z = qz;
}
Location::Location() {
    x = 0;
    y = 0;
    z = 0;
}
std::string Location::tostring() const {
    return "["+std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)+"]";
}
int Location::roughdistanceto(Location& other) {
    return abs(other.x-x)+abs(other.y-y)+abs(other.z-z);
}
bool operator< (const Location& l, const Location& r) {
    if (l.x<r.x) {return true;}
    if (l.x>r.x) {return false;}
    if (l.y<r.y) {return true;}
    if (l.y>r.y) {return false;}
    if (l.z<r.z) {return true;}
    if (l.z>r.z) {return false;}
    return false;
}
bool operator== (const Location& l, const Location& r) {
    if (l.x!=r.x) {return false;}
    if (l.y!=r.y) {return false;}
    if (l.z!=r.z) {return false;}
    return true;
}



Bounds::Bounds(int xl,int xu,int yl, int yu, int zl, int zu) : lx(xl),ux(xu),ly(yl),uy(yu),lz(zl),uz(zu),bounded(true) {}
Bounds::Bounds() : bounded(false) {}




