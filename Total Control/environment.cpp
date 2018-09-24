//
//  environment.cpp
//  Total Control
//
//  Created by Parker Lawrence on 1/9/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "environment.h"


//
void Environment::beginthread() {
//    Structure k;
//    structures.push_back(k);
//    gen.pool = &structures;
//    gen.firstdump();
    pthread_create(&loadingthread , NULL, loaderthread, (void*)this);
//    pthread_t loadingthread2;
//    pthread_create(&loadingthread2 , NULL, loaderthread, (void*)this);
}
Structure* Environment::getstructure(std::string targetid) {
    for (int istr = 0;istr<structures.size();istr++) {
        if (structures[istr]->structureid == targetid) {
            return structures[istr];
        }
    }
    throw;
}
void Environment::addstructure(Structure* newst) {

    structures.push_back(newst);
}
void Environment::loadnextchunk() {
    if (structures.size()>0) {
        Structure* min = structures[0];
        for (int ind=1;ind<structures.size();ind++) {
            if (structures[ind]->loadstage<min->loadstage) {
                min=structures[ind];
            }
        }
        min->updatequeue(epix,epiy,epiz);
    }
}

void Environment::opensavedirectory() {
//    boost::filesystem::create_directory(SAVEDIR);
//    for (int k=0;k<structures.size();k++) {
//        boost::filesystem::create_directory(SAVEDIR "/" + structures[k]->structureid);
//    }
}
void Environment::draw() {
    glm::vec4 thisvert;
//    bool hitplanes[6] = {false,false,false,false,false,false};
//    static int phase=0;
    for (int k=0;k<structures.size();k++) {
//        if (phase++%3==0) {
            structures[k]->updatelod(epix,epiy,epiz);
//        }
        structures[k]->render();
    }
    renderall();
}

void Environment::checkup(double vx,double vy, double vz) {
    epix = vx;
    epiy = vy;
    epiz = vz;
}
void Environment::cleanup() {
    keepexecution = false;
    pthread_join(loadingthread,NULL);
}
std::string Environment::nextshardname() {
    return std::to_string(shards++);
}
//
void* loaderthread(void* me) {
    Environment* dest = (Environment*)me;
    while (dest->keepexecution) {
        dest->loadnextchunk();
//        break;
    }
    return NULL;
}
//






void tearaway(int x,int y,int z,int recur,OctreeSegment* world,Environment* overflowbucket) {
//    return;
    Structure* newguy = new Structure(overflowbucket->nextshardname(),*overflowbucket,false);
//    std::cout<<"working\n";
//    return;
    Location startpoint = Location(x,y,z);
    PathTesterPool nodes = PathTesterPool(startpoint,recur);
    PathTesterPool spent = PathTesterPool(startpoint,recur);
    nodes.add(startpoint,nodes.hash(startpoint));
    Location toinserts[6];
    g_lod = recur;
    while (true) {
        int dist = nodes.getpromising();
        if (dist==-1) {break;}
        Location xyz = nodes.poppromising(dist);
        
        spent.add(xyz,dist);
        int l = 0;
        if (world->giveconflag(xyz.x,xyz.y,xyz.z)&16) {toinserts[l] = Location(xyz.x+(1<<recur),xyz.y,xyz.z);l++;}
        if (world->giveconflag(xyz.x,xyz.y,xyz.z)&32) {toinserts[l] = Location(xyz.x,xyz.y+(1<<recur),xyz.z);l++;}
        if (world->giveconflag(xyz.x,xyz.y,xyz.z)&64) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z+(1<<recur));l++;}
        if (world->giveconflag(xyz.x-(1<<recur),xyz.y,xyz.z)&16) {toinserts[l] = Location(xyz.x-(1<<recur),xyz.y,xyz.z);l++;}
        if (world->giveconflag(xyz.x,xyz.y-(1<<recur),xyz.z)&32) {toinserts[l] = Location(xyz.x,xyz.y-(1<<recur),xyz.z);l++;}
        if (world->giveconflag(xyz.x,xyz.y,xyz.z-(1<<recur))&64) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z-(1<<recur));l++;}
        for (int k=0;k<l;k++) {
            Location toinsert = toinserts[k];
            int hash = nodes.hash(toinsert);
            if (!spent.contains(toinsert, hash)) {
                nodes.add(toinsert,hash);
                
                if (world->getser(toinsert.x,toinsert.y,toinsert.z)==0) {
                    throw;
                }
            }
        }
    }
    
    while (true) {
        int dist = spent.getpromising();
        if (dist==-1) {break;}
        Location xyz = spent.poppromising(dist);
        
        OctreeSegment* tomove = world->pullaway(xyz.x,xyz.y,xyz.z,recur,world);
        if (tomove->giveimconflag()&8) {
            newguy->expandarbit(xyz.x,xyz.y,xyz.z,recur);
            newguy->data->insertinto(xyz.x,xyz.y,xyz.z,recur,newguy->depth,tomove,newguy->data);
        }
        
    }
    
    
//    overflowbucket->addstructure(newguy);
    
}




