//
//  octreeseparator.cpp
//  Total Control
//
//  Created by Parker on 5/16/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include <stdio.h>
#include "octree.h"




PathTesterBucket::PathTesterBucket(Location first) : node(first),next(NULL){}
void PathTesterBucket::addthing(Location toadd) {
    if (node == toadd) {return;}
    if (next==NULL) {
        next = new PathTesterBucket(toadd);
    } else {
        next->addthing(toadd);
    }
}
bool PathTesterBucket::contains(Location toadd) {
    if (node == toadd) {return true;}
    else if (next==NULL) {return false;}
    else {return next->contains(toadd);}
}

PathTesterPool::PathTesterPool(Location origin,int depth) : recur(depth), destination(origin) {}
int PathTesterPool::hash(Location target) {
    return destination.roughdistanceto(target)>>recur;
}
bool PathTesterPool::contains(Location target,int hash) {
    if (hash >= buckets.size()) {
        return false;
    }
    if (buckets[hash] == NULL) {
        return false;
    }
    return buckets[hash]->contains(target);
}
void PathTesterPool::add(Location target,int hash) {
    while (hash >= buckets.size()) {
        buckets.push_back(NULL);
    }
    if (buckets[hash] == NULL) {
        buckets[hash] = new PathTesterBucket(target);
    } else {
        buckets[hash]->addthing(target);
    }
}
int PathTesterPool::getpromising() {
    for (int k = 0;k<buckets.size();k++) {
        if (buckets[k] != NULL) {
            return k;
        }
    }
    return -1;
}

BranchRegistry::BranchRegistry(Environment& backref) : capsulepointer(backref) {
    
}
void BranchRegistry::add(Location place,OctreeBranch* toadd) {
    while (toadd->depth>=registry.size()) {
        registry.push_back(std::vector<std::pair<Location,OctreeBranch*>>());
    }
    registry[toadd->depth].push_back(std::pair<Location,OctreeBranch*>(place,toadd));
}
void BranchRegistry::prune(OctreeSegment* world) {
    for (int e=0;e<registry.size();e++) {
        for (int i=(int)registry[e].size()-1;i>=0;i--) {
            Location place = registry[e][i].first;
            if (registry[e][i].second->phase2check(place.x,place.y,place.z,world,&capsulepointer)) {
                registry[e].erase(registry[e].begin() + i);
            }
        }
    }
}
bool BranchRegistry::contains(OctreeBranch* ref) {
    if (ref->depth>=registry.size()) {
        return false;
    }
    for (int i=0;i<registry[ref->depth].size();i++) {
        if (registry[ref->depth][i].second == ref) {
            return true;
        }
    }
    return false;
}

Location PathTesterPool::poppromising(int hash) {
    Location toreturn = buckets[hash]->node;
    PathTesterBucket* temp = buckets[hash]->next;
    delete buckets[hash];
    buckets[hash] = temp;
    return toreturn;
}
PathTesterPool::~PathTesterPool() {
    for (int k = 0;k<buckets.size();k++) {
        while (buckets[k] != NULL) {
            PathTesterBucket* temp = buckets[k]->next;
            delete buckets[k];
            buckets[k] = temp;
        }
    }
}

ChunkDistancePool::ChunkDistancePool() {
    for (int k=0;k<MAX_WORLDFILE_GEOMSAVE+1;k++) {
        buckets[k]=NULL;
    }
}

int ChunkDistancePool::hash(Location target,Location destination) {
    return std::max(std::max(abs(target.x-destination.x),abs(target.y-destination.y)),abs(target.z-destination.z));
}


void ChunkDistancePool::add(Location newloc,int lod) {
    lod=lod>MAX_WORLDFILE_GEOMSAVE?MAX_WORLDFILE_GEOMSAVE+1:lod;
    if (buckets[lod] == NULL) {
        buckets[lod] = new PathTesterBucket(newloc);
    } else {
        buckets[lod]->addthing(newloc);
    }
}

uint8_t OctreeSegment::giveimconflag() {throw;}
uint8_t OctreeFeature::giveimconflag() {return conflag;}
uint8_t OctreeBranch::giveimconflag() {return connections;}
uint8_t OctreeBud::giveimconflag() {
    extern uint8_t materialattribs[];
    if (fillvalue == 0) {
        return 255;
    }
    return (materialattribs[fillvalue]&1)*(128+64+32+16);
}

uint8_t OctreeSegment::giveconflag(BlockLoc x,BlockLoc y,BlockLoc z) {return giveimconflag();}
uint8_t OctreeBranch::giveconflag(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (g_lod<=depth) {
        return subdivisions XYZINDEX->giveconflag(x,y,z);
    } else {
        return connections;
    }
}
uint8_t OctreePortionAwareBranch::giveconflag(BlockLoc x,BlockLoc y,BlockLoc z) {
    if (g_lod<MIN_WORLDFILE_GEOMSAVE && !hardload) {
        return 255;
    } else if (g_lod<=depth) {
        return subdivisions XYZINDEX->giveconflag(x,y,z);
    } else {
        return connections;
    }
}


void OctreeSegment::testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world,BranchRegistry*) {}
void OctreeBranch::testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world,BranchRegistry* registry) {
    int mask = 1<<depth;
    subdivisions[0][0][0]->testconnected(x     ,y     ,z     ,world,registry);
    subdivisions[1][0][0]->testconnected(x|mask,y     ,z     ,world,registry);
    subdivisions[0][1][0]->testconnected(x     ,y|mask,z     ,world,registry);
    subdivisions[1][1][0]->testconnected(x|mask,y|mask,z     ,world,registry);
    subdivisions[0][0][1]->testconnected(x     ,y     ,z|mask,world,registry);
    subdivisions[1][0][1]->testconnected(x|mask,y     ,z|mask,world,registry);
    subdivisions[0][1][1]->testconnected(x     ,y|mask,z|mask,world,registry);
    subdivisions[1][1][1]->testconnected(x|mask,y|mask,z|mask,world,registry);
    int target =  ((subdivisions[0][0][0]->giveimconflag()&128)?  1:0)|
                  ((subdivisions[1][0][0]->giveimconflag()&128)?  2:0)|
                  ((subdivisions[0][1][0]->giveimconflag()&128)?  4:0)|
                  ((subdivisions[1][1][0]->giveimconflag()&128)?  8:0)|
                  ((subdivisions[0][0][1]->giveimconflag()&128)? 16:0)|
                  ((subdivisions[1][0][1]->giveimconflag()&128)? 32:0)|
                  ((subdivisions[0][1][1]->giveimconflag()&128)? 64:0)|
                  ((subdivisions[1][1][1]->giveimconflag()&128)?128:0);
    if (target==0) {return;}
    int starter;
    if (target&170) {
        if (target&136) {
            if (target&128) {starter = 128;}
            else {starter = 8;}
        } else {
            if (target&32) {starter = 32;}
            else {starter = 2;}
        }
    } else {
        if (target&68) {
            if (target&64) {starter = 64;}
            else {starter = 4;}
        } else {
            if (target&16) {starter = 16;}
            else {starter = 1;}
        }
    }
    int compare;
    do {
        compare = starter;
        //X FLAGS
        if ((starter&3  ) && subdivisions[0][0][0]->giveimconflag()&16) {starter |= 3;}
        if ((starter&12 ) && subdivisions[0][1][0]->giveimconflag()&16) {starter |= 12;}
        if ((starter&48 ) && subdivisions[0][0][1]->giveimconflag()&16) {starter |= 48;}
        if ((starter&192) && subdivisions[0][1][1]->giveimconflag()&16) {starter |= 192;}
        //Y FLAGS
        if ((starter&5  ) && subdivisions[0][0][0]->giveimconflag()&32) {starter |= 5;}
        if ((starter&10 ) && subdivisions[1][0][0]->giveimconflag()&32) {starter |= 10;}
        if ((starter&80 ) && subdivisions[0][0][1]->giveimconflag()&32) {starter |= 80;}
        if ((starter&160) && subdivisions[1][0][1]->giveimconflag()&32) {starter |= 160;}
        //Z FLAGS
        if ((starter&17 ) && subdivisions[0][0][0]->giveimconflag()&64) {starter |= 17;}
        if ((starter&34 ) && subdivisions[1][0][0]->giveimconflag()&64) {starter |= 34;}
        if ((starter&68 ) && subdivisions[0][1][0]->giveimconflag()&64) {starter |= 68;}
        if ((starter&136) && subdivisions[1][1][0]->giveimconflag()&64) {starter |= 136;}
    } while (compare!=starter);
    if (starter!=target) {
        registry->add(Location(x,y,z),this);
    }
}
bool OctreeBranch::phase2check(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world,Environment* obuck) {
    int mask = 1<<depth;
    int meshtarget =  ((subdivisions[0][0][0]->giveimconflag()&8)?  1:0)|
                      ((subdivisions[1][0][0]->giveimconflag()&8)?  2:0)|
                      ((subdivisions[0][1][0]->giveimconflag()&8)?  4:0)|
                      ((subdivisions[1][1][0]->giveimconflag()&8)?  8:0)|
                      ((subdivisions[0][0][1]->giveimconflag()&8)? 16:0)|
                      ((subdivisions[1][0][1]->giveimconflag()&8)? 32:0)|
                      ((subdivisions[0][1][1]->giveimconflag()&8)? 64:0)|
                      ((subdivisions[1][1][1]->giveimconflag()&8)?128:0);
    if (meshtarget==0) {return true;}
    int commonend;
    if (meshtarget&170) {
        if (meshtarget&136) {
            if (meshtarget&128) {commonend = 128;}
            else {commonend = 8;}
        } else {
            if (meshtarget&32) {commonend = 32;}
            else {commonend = 2;}
        }
    } else {
        if (meshtarget&68) {
            if (meshtarget&64) {commonend = 64;}
            else {commonend = 4;}
        } else {
            if (meshtarget&16) {commonend = 16;}
            else {commonend = 1;}
        }
    }
    BlockLoc sx = x + (commonend&(2+8+32+128)?mask:0);
    BlockLoc sy = y + (commonend&(4+8+64+128)?mask:0);
    BlockLoc sz = z + (commonend&(16+32+64+128)?mask:0);
//    DEBUGDEBUG = meshtarget;
    meshtarget^=commonend;
    if ((meshtarget&1  ) && (subdivisions[0][0][0]->giveimconflag()&8) && !testConnection(x     ,y     ,z     ,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&2  ) && (subdivisions[1][0][0]->giveimconflag()&8) && !testConnection(x|mask,y     ,z     ,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&4  ) && (subdivisions[0][1][0]->giveimconflag()&8) && !testConnection(x     ,y|mask,z     ,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&8  ) && (subdivisions[1][1][0]->giveimconflag()&8) && !testConnection(x|mask,y|mask,z     ,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&16 ) && (subdivisions[0][0][1]->giveimconflag()&8) && !testConnection(x     ,y     ,z|mask,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&32 ) && (subdivisions[1][0][1]->giveimconflag()&8) && !testConnection(x|mask,y     ,z|mask,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&64 ) && (subdivisions[0][1][1]->giveimconflag()&8) && !testConnection(x     ,y|mask,z|mask,sx,sy,sz,depth,world,obuck)) {return false;}
    if ((meshtarget&128) && (subdivisions[1][1][1]->giveimconflag()&8) && !testConnection(x|mask,y|mask,z|mask,sx,sy,sz,depth,world,obuck)) {return false;}
    return true;
}

bool testConnection(BlockLoc x1,BlockLoc y1,BlockLoc z1,BlockLoc x2,BlockLoc y2,BlockLoc z2,int recur,OctreeSegment* world,Environment* overflowbucket) {
    Location endpoint = Location(x2,y2,z2);
    Location startpoint = Location(x1,y1,z1);
    
    PathTesterPool forwardnodes = PathTesterPool(endpoint,recur);
    PathTesterPool forwardspentnodes = PathTesterPool(endpoint,recur);
    PathTesterPool backwardnodes = PathTesterPool(startpoint,recur);
    PathTesterPool backwardspentnodes = PathTesterPool(startpoint,recur);
    
    forwardnodes.add(startpoint,forwardnodes.hash(startpoint));
    backwardnodes.add(endpoint,backwardnodes.hash(endpoint));

    Location toinserts[6];
    int l;
    int dist;
    Location xyz;
    bool lockforward = false;
    bool lockbackward = false;
    BlockLoc mask = 1<<recur;

    g_lod = recur;
    while (true) {
        
//        k++;
        if (!lockforward) {
            dist = forwardnodes.getpromising();
            if (dist==0) {return true;}
            else if (dist==-1) {tearaway(startpoint.x,startpoint.y,startpoint.z,recur,world,overflowbucket);return true;}
            xyz = forwardnodes.poppromising(dist);
            forwardspentnodes.add(xyz,dist);
            
            l = 0;
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&1) {toinserts[l] = Location(xyz.x+mask,xyz.y,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&2) {toinserts[l] = Location(xyz.x,xyz.y+mask,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&4) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z+mask);l++;}
            if (world->giveconflag(xyz.x-mask,xyz.y,xyz.z)&1) {toinserts[l] = Location(xyz.x-mask,xyz.y,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y-mask,xyz.z)&2) {toinserts[l] = Location(xyz.x,xyz.y-mask,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z-mask)&4) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z-mask);l++;}
            
            for (int k=0;k<l;k++) {
                Location toinsert = toinserts[k];
                int hash = forwardnodes.hash(toinsert);
                if (!forwardspentnodes.contains(toinsert, hash)) {
                    if (world->getser(toinsert.x,toinsert.y,toinsert.z)==0) {
                        
                        
                        lockforward = true;
                        
                    }
                    forwardnodes.add(toinsert,hash);
                }
            }
        }
        
        if (!lockbackward) {
            dist = backwardnodes.getpromising();
            if (dist==0) {return true;}
            else if (dist==-1) {tearaway(endpoint.x,endpoint.y,endpoint.z,recur,world,overflowbucket);return true;}
            xyz = backwardnodes.poppromising(dist);
            backwardspentnodes.add(xyz,dist);
            
            l = 0;
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&1) {toinserts[l] = Location(xyz.x+mask,xyz.y,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&2) {toinserts[l] = Location(xyz.x,xyz.y+mask,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z)&4) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z+mask);l++;}
            if (world->giveconflag(xyz.x-mask,xyz.y,xyz.z)&1) {toinserts[l] = Location(xyz.x-mask,xyz.y,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y-mask,xyz.z)&2) {toinserts[l] = Location(xyz.x,xyz.y-mask,xyz.z);l++;}
            if (world->giveconflag(xyz.x,xyz.y,xyz.z-mask)&4) {toinserts[l] = Location(xyz.x,xyz.y,xyz.z-mask);l++;}
            for (int k=0;k<l;k++) {
                Location toinsert = toinserts[k];
                int hash = backwardnodes.hash(toinsert);
                if (!backwardspentnodes.contains(toinsert, hash)) {
                    if (world->getser(toinsert.x,toinsert.y,toinsert.z)==0) {
                        lockbackward = true;
                    }
                    backwardnodes.add(toinsert,hash);
                }
            }
        }
        if (lockforward and lockbackward) {
            return false;
        }
    }
}



