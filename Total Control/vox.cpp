//
//  vox.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/21/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "vox.h"
//#include "environment.h"
//EmptySampler Structure::emptysampler = EmptySampler();

Structure::Structure(std::string name,Environment& backref,bool load) : structureid(name),loadin(load), world(name,transform,backref,(int)!load) {
    world.expand(CHPOWER);
    
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
            if (attain(newloc)){return;}
        }
        else {
            for (int i1=-1*i;i1<=i;i1++) {
                for (int i2=-1*i;i2<=i;i2++) {
                    if (attain(Location(newloc.x+i1,newloc.y+i2,newloc.z+i))) {return;}
                    if (attain(Location(newloc.x+i1,newloc.y+i2,newloc.z-i))) {return;}
                }
                for (int i2=-1*i+1;i2<=i-1;i2++) {
                    if (attain(Location(newloc.x+i1,newloc.z+i,newloc.y+i2))) {return;}
                    if (attain(Location(newloc.x+i1,newloc.z-i,newloc.y+i2))) {return;}
                }
            }
            for (int i1=-1*i+1;i1<=i-1;i1++) {
                for (int i2=-1*i+1;i2<=i-1;i2++) {
                    if (attain(Location(newloc.z+i,newloc.x+i1,newloc.y+i2))) {return;}
                    if (attain(Location(newloc.z-i,newloc.x+i1,newloc.y+i2))) {return;}
                }
            }
        }
    }
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



bool Structure::attain(Location pos) {
    bool load = false;
    bool generate = false;
    
//    int distance = std::max(std::max(abs(cameraloc.x-pos.x),abs(cameraloc.z-pos.z)),abs(cameraloc.y-pos.y));
//    int lod = (int)((distance)/2);
    
    if (world.underpressure(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z))>world.depth) {
        generate = !world.popular;//bud, this is id
    } else {
        OctreePortionAwareBranch* grably = world.data->getvoxunit(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));
        if (grably==NULL) {
            generate = !world.data->getser(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));//bud, this is id
        } else {
            load = !(grably->hardLoaded);// or (lod>=MIN_WORLDFILE_GEOMSAVE and lod<=MAX_WORLDFILE_GEOMSAVE));
        }
    }
    if (load){
        world.filepullportion(SAVEDIR "/"+structureid,pos.x,pos.y,pos.z);
    }
    if (generate) {
        source->populate(pos.x,pos.y,pos.z,world);
    }
    OctreePortionAwareBranch* unit = world.data->getvoxunit(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z));
    
    if (unit!=NULL) {
        if (generate) {
//            unit->vertifyall();
            world.filepushportion(SAVEDIR "/"+structureid,pos.x,pos.y,pos.z);
        }
        if (load or generate) {
            unit->testconnected(ASCHUNKLOC(pos.x),ASCHUNKLOC(pos.y),ASCHUNKLOC(pos.z),world.data,&world.currenttests);
            world.currenttests.prune(world.data);
        }
    }
//    if (load or generate) {
////        geomloaded.add(pos,lod);
//        world.h_manifest(pos.x-1,pos.y-1,pos.z-1); world.h_manifest(pos.x  ,pos.y-1,pos.z-1);
//        world.h_manifest(pos.x-1,pos.y  ,pos.z-1); world.h_manifest(pos.x  ,pos.y  ,pos.z-1);
//        world.h_manifest(pos.x-1,pos.y-1,pos.z  ); world.h_manifest(pos.x  ,pos.y-1,pos.z  );
//        world.h_manifest(pos.x-1,pos.y  ,pos.z  ); world.h_manifest(pos.x  ,pos.y  ,pos.z  );
//    }
//    if (unit!=NULL) {
////        unit->curlod = lod;
////        world.v_manifest(pos.x-1,pos.y-1,pos.z-1); world.v_manifest(pos.x  ,pos.y-1,pos.z-1);
////        world.v_manifest(pos.x-1,pos.y  ,pos.z-1); world.v_manifest(pos.x  ,pos.y  ,pos.z-1);
////        world.v_manifest(pos.x-1,pos.y-1,pos.z  ); world.v_manifest(pos.x  ,pos.y-1,pos.z  );
////        world.v_manifest(pos.x-1,pos.y  ,pos.z  ); world.v_manifest(pos.x  ,pos.y  ,pos.z  );
//        world.g_manifest(pos.x-1,pos.y-1,pos.z-1); world.g_manifest(pos.x  ,pos.y-1,pos.z-1); world.g_manifest(pos.x+1,pos.y-1,pos.z-1);
//        world.g_manifest(pos.x-1,pos.y  ,pos.z-1); world.g_manifest(pos.x  ,pos.y  ,pos.z-1); world.g_manifest(pos.x+1,pos.y  ,pos.z-1);
//        world.g_manifest(pos.x-1,pos.y+1,pos.z-1); world.g_manifest(pos.x  ,pos.y+1,pos.z-1); world.g_manifest(pos.x+1,pos.y+1,pos.z-1);
//        world.g_manifest(pos.x-1,pos.y-1,pos.z  ); world.g_manifest(pos.x  ,pos.y-1,pos.z  ); world.g_manifest(pos.x+1,pos.y-1,pos.z  );
//        world.g_manifest(pos.x-1,pos.y  ,pos.z  ); world.g_manifest(pos.x  ,pos.y  ,pos.z  ); world.g_manifest(pos.x+1,pos.y  ,pos.z  );
//        world.g_manifest(pos.x-1,pos.y+1,pos.z  ); world.g_manifest(pos.x  ,pos.y+1,pos.z  ); world.g_manifest(pos.x+1,pos.y+1,pos.z  );
//        world.g_manifest(pos.x-1,pos.y-1,pos.z+1); world.g_manifest(pos.x  ,pos.y-1,pos.z+1); world.g_manifest(pos.x+1,pos.y-1,pos.z+1);
//        world.g_manifest(pos.x-1,pos.y  ,pos.z+1); world.g_manifest(pos.x  ,pos.y  ,pos.z+1); world.g_manifest(pos.x+1,pos.y  ,pos.z+1);
//        world.g_manifest(pos.x-1,pos.y+1,pos.z+1); world.g_manifest(pos.x  ,pos.y+1,pos.z+1); world.g_manifest(pos.x+1,pos.y+1,pos.z+1);
//    }
    if (load or generate) {
        
        std::ofstream file = std::ofstream(SAVEDIR "/"+structureid+"/massfile",std::ios::out|std::ios::binary|std::ios::trunc);
        file.write((char*)&(world.depth),sizeof(int));
        world.data->worldfilesave(file,world.depth);
        file.close();
    }
    

    return load or generate;
        
}
void Structure::render() {
//    if (structureid!="test") {return;}
    
    world.render();
}
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




