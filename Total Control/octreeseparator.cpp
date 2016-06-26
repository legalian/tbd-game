//
//  octreeseparator.cpp
//  Total Control
//
//  Created by Parker on 5/16/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include <stdio.h>
#include "octree.h"


#define XYZINDEX [(x&(1<<depth))>>depth][(y&(1<<depth))>>depth][(z&(1<<depth))>>depth]



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




void OctreeSegment::runseperation(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
void OctreeSegment::testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {}
//void OctreeSegment::createflags(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {}
//uint8_t OctreeSegment::grabedgeflags(BlockLoc x,BlockLoc y,BlockLoc z) {throw;}
//void OctreeSegment::calculateflags(BlockLoc x,BlockLoc y,BlockLoc z,int recur) {}
bool OctreeSegment::giveXflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {throw;}
bool OctreeSegment::giveYflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {throw;}
bool OctreeSegment::giveZflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {throw;}
bool OctreeSegment::giveWflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {throw;}



bool OctreeFeature::giveXflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    extern uint8_t materialattribs[];
    if (materialattribs[fillvalue]&1) {
        return materialattribs[world->getser(x+1,y,z)]&1;
    }
    return false;
}
bool OctreeFeature::giveYflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    extern uint8_t materialattribs[];
    if (materialattribs[fillvalue]&1) {
        return materialattribs[world->getser(x,y+1,z)]&1;
    }
    return false;
}
bool OctreeFeature::giveZflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    extern uint8_t materialattribs[];
    if (materialattribs[fillvalue]&1) {
        return materialattribs[world->getser(x,y,z+1)]&1;
    }
    return false;
}
bool OctreeFeature::giveWflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    extern uint8_t materialattribs[];
    return materialattribs[fillvalue]&1;
}
//uint8_t OctreeFeature::grabedgeflags(BlockLoc x,BlockLoc y,BlockLoc z) {
//    extern uint8_t materialattribs[];
//    if (materialattribs[fillvalue]&1) {
//        return 255;
//    } else  {
//        return 0;
//    }
//}

bool OctreeBud::giveXflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {extern uint8_t materialattribs[];return materialattribs[fillvalue]&1;}
bool OctreeBud::giveYflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {extern uint8_t materialattribs[];return materialattribs[fillvalue]&1;}
bool OctreeBud::giveZflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {extern uint8_t materialattribs[];return materialattribs[fillvalue]&1;}
bool OctreeBud::giveWflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {extern uint8_t materialattribs[];return materialattribs[fillvalue]&1;}
//uint8_t OctreeBud::grabedgeflags(BlockLoc x,BlockLoc y,BlockLoc z) {
//    extern uint8_t materialattribs[];
//    if (materialattribs[fillvalue]&1) {
//        return 255;
//    } else  {
//        return 0;
//    }
//}

void OctreeBranch::runseperation(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    BlockLoc mask = 1<<depth;
    subdivisions[0][0][0]->runseperation(x     ,y     ,z     ,world);
    subdivisions[1][0][0]->runseperation(x|mask,y     ,z     ,world);
    subdivisions[0][1][0]->runseperation(x     ,y|mask,z     ,world);
    subdivisions[1][1][0]->runseperation(x|mask,y|mask,z     ,world);
    subdivisions[0][0][1]->runseperation(x     ,y     ,z|mask,world);
    subdivisions[1][0][1]->runseperation(x|mask,y     ,z|mask,world);
    subdivisions[0][1][1]->runseperation(x     ,y|mask,z|mask,world);
    subdivisions[1][1][1]->runseperation(x|mask,y|mask,z|mask,world);

    if (subdivisions[1][0][0]->giveXflag(x|mask,y     ,z     ,depth,world) or
        subdivisions[1][1][0]->giveXflag(x|mask,y|mask,z     ,depth,world) or
        subdivisions[1][0][1]->giveXflag(x|mask,y     ,z|mask,depth,world) or
        subdivisions[1][1][1]->giveXflag(x|mask,y|mask,z|mask,depth,world)) {
        connections |= 1;
    }
    if (subdivisions[0][1][0]->giveYflag(x     ,y|mask,z     ,depth,world) or
        subdivisions[1][1][0]->giveYflag(x|mask,y|mask,z     ,depth,world) or
        subdivisions[0][1][1]->giveYflag(x     ,y|mask,z|mask,depth,world) or
        subdivisions[1][1][1]->giveYflag(x|mask,y|mask,z|mask,depth,world)) {
        connections |= 2;
    }
    if (subdivisions[0][0][1]->giveZflag(x     ,y     ,z|mask,depth,world) or
        subdivisions[1][0][1]->giveZflag(x|mask,y     ,z|mask,depth,world) or
        subdivisions[0][1][1]->giveZflag(x     ,y|mask,z|mask,depth,world) or
        subdivisions[1][1][1]->giveZflag(x|mask,y|mask,z|mask,depth,world)) {
        connections |= 4;
    }
    if (subdivisions[0][0][0]->giveWflag(x     ,y     ,z     ,depth,world) or
        subdivisions[1][0][0]->giveWflag(x|mask,y     ,z     ,depth,world) or
        subdivisions[0][1][0]->giveWflag(x     ,y|mask,z     ,depth,world) or
        subdivisions[1][1][0]->giveWflag(x|mask,y|mask,z     ,depth,world) or
        subdivisions[0][0][1]->giveWflag(x     ,y     ,z|mask,depth,world) or
        subdivisions[1][0][1]->giveWflag(x|mask,y     ,z|mask,depth,world) or
        subdivisions[0][1][1]->giveWflag(x     ,y|mask,z|mask,depth,world) or
        subdivisions[1][1][1]->giveWflag(x|mask,y|mask,z|mask,depth,world)) {
        connections |= 8;
    }
}
bool OctreeBranch::giveXflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    if (recur<=depth) {
        return subdivisions XYZINDEX->giveXflag(x,y,z,recur,world);
    } else {
        return (connections&1)!=0;
    }
}
bool OctreeBranch::giveYflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    if (recur<=depth) {
        return subdivisions XYZINDEX->giveYflag(x,y,z,recur,world);
    } else {
        return (connections&2)!=0;
    }
}
bool OctreeBranch::giveZflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    if (recur<=depth) {
        return subdivisions XYZINDEX->giveZflag(x,y,z,recur,world);
    } else {
        return (connections&4)!=0;
    }
}
bool OctreeBranch::giveWflag(BlockLoc x,BlockLoc y,BlockLoc z,int recur,OctreeSegment* world) {
    if (recur<=depth) {
        return subdivisions XYZINDEX->giveWflag(x,y,z,recur,world);
    } else {
        return (connections&8)!=0;
    }
}
void OctreeBranch::testconnected(BlockLoc x,BlockLoc y,BlockLoc z,OctreeSegment* world) {
    int mask = 1<<depth;
    subdivisions[0][0][0]->testconnected(x     ,y     ,z     ,world);
    subdivisions[1][0][0]->testconnected(x|mask,y     ,z     ,world);
    subdivisions[0][1][0]->testconnected(x     ,y|mask,z     ,world);
    subdivisions[1][1][0]->testconnected(x|mask,y|mask,z     ,world);
    subdivisions[0][0][1]->testconnected(x     ,y     ,z|mask,world);
    subdivisions[1][0][1]->testconnected(x|mask,y     ,z|mask,world);
    subdivisions[0][1][1]->testconnected(x     ,y|mask,z|mask,world);
    subdivisions[1][1][1]->testconnected(x|mask,y|mask,z|mask,world);
    int target =  ((int)subdivisions[0][0][0]->giveWflag(x     ,y     ,z     ,depth,world))+
                 (((int)subdivisions[1][0][0]->giveWflag(x|mask,y     ,z     ,depth,world))<<1)+
                 (((int)subdivisions[0][1][0]->giveWflag(x     ,y|mask,z     ,depth,world))<<2)+
                 (((int)subdivisions[1][1][0]->giveWflag(x|mask,y|mask,z     ,depth,world))<<3)+
                 (((int)subdivisions[0][0][1]->giveWflag(x     ,y     ,z|mask,depth,world))<<4)+
                 (((int)subdivisions[1][0][1]->giveWflag(x|mask,y     ,z|mask,depth,world))<<5)+
                 (((int)subdivisions[0][1][1]->giveWflag(x     ,y|mask,z|mask,depth,world))<<6)+
                 (((int)subdivisions[1][1][1]->giveWflag(x|mask,y|mask,z|mask,depth,world))<<7);
    if (target==0) {return;}
    int starter;
    if (target&(2+8+32+128)) {
        if (target&(8+128)) {
            if (target&128) {starter = 128;}
            else {starter = 8;}
        } else {
            if (target&32) {starter = 32;}
            else {starter = 2;}
        }
    } else {
        if (target&(4+64)) {
            if (target&64) {starter = 64;}
            else {starter = 4;}
        } else {
            if (target&16) {starter = 16;}
            else {starter = 1;}
        }
    }
    int ostart = starter;
    int compare;
    do {
        compare = starter;
        //X FLAGS
        if ((starter&3  ) && subdivisions[0][0][0]->giveXflag(x     ,y     ,z     ,depth,world)) {starter = starter|3;}
        if ((starter&12 ) && subdivisions[0][1][0]->giveXflag(x     ,y|mask,z     ,depth,world)) {starter = starter|12;}
        if ((starter&48 ) && subdivisions[0][0][1]->giveXflag(x     ,y     ,z|mask,depth,world)) {starter = starter|48;}
        if ((starter&192) && subdivisions[0][1][1]->giveXflag(x     ,y|mask,z|mask,depth,world)) {starter = starter|192;}
        //Y FLAGS
        if ((starter&5  ) && subdivisions[0][0][0]->giveYflag(x     ,y     ,z     ,depth,world)) {starter = starter|5;}
        if ((starter&10 ) && subdivisions[1][0][0]->giveYflag(x|mask,y     ,z     ,depth,world)) {starter = starter|10;}
        if ((starter&80 ) && subdivisions[0][0][1]->giveYflag(x     ,y     ,z|mask,depth,world)) {starter = starter|80;}
        if ((starter&160) && subdivisions[1][0][1]->giveYflag(x|mask,y     ,z|mask,depth,world)) {starter = starter|160;}
        //Z FLAGS
        if ((starter&17 ) && subdivisions[0][0][0]->giveZflag(x     ,y     ,z     ,depth,world)) {starter = starter|17;}
        if ((starter&34 ) && subdivisions[1][0][0]->giveZflag(x|mask,y     ,z     ,depth,world)) {starter = starter|34;}
        if ((starter&68 ) && subdivisions[0][1][0]->giveZflag(x     ,y|mask,z     ,depth,world)) {starter = starter|68;}
        if ((starter&136) && subdivisions[1][1][0]->giveZflag(x|mask,y|mask,z     ,depth,world)) {starter = starter|136;}
    } while (compare!=starter);
    if (starter==target) {return;}
    connections|=16;
    DEBUGDEBUG = target;
    return;
    std::cout<<target<<" , "<<ostart<<" , "<<starter<<"\n";
    BlockLoc sx = x + (ostart&(2+8+32+128)?1:0);
    BlockLoc sy = y + (ostart&(4+8+64+128)?1:0);
    BlockLoc sz = z + (ostart&(16+32+64+128)?1:0);
    if ((target&1   and !(starter&1  )) && !testConnection(x     ,y     ,z     ,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&2   and !(starter&2  )) && !testConnection(x|mask,y     ,z     ,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&4   and !(starter&4  )) && !testConnection(x     ,y|mask,z     ,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&8   and !(starter&8  )) && !testConnection(x|mask,y|mask,z     ,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&16  and !(starter&16 )) && !testConnection(x     ,y     ,z|mask,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&32  and !(starter&32 )) && !testConnection(x|mask,y     ,z|mask,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&64  and !(starter&64 )) && !testConnection(x     ,y|mask,z|mask,sx,sy,sz,depth,world)) {connections |= 16;return;}
    if ((target&128 and !(starter&128)) && !testConnection(x|mask,y|mask,z|mask,sx,sy,sz,depth,world)) {connections |= 16;return;}
    return;
}
bool testConnection(BlockLoc x1,BlockLoc y1,BlockLoc z1,BlockLoc x2,BlockLoc y2,BlockLoc z2,int recur,OctreeSegment* world) {
    Location endpoint = Location(x2,y2,z2);
    Location startpoint = Location(x1,y1,z1);
    
    PathTesterPool emptypool = PathTesterPool(endpoint,recur);
    return testConnection(startpoint,endpoint,recur,world,&emptypool);
}
bool testConnection(Location startpoint,Location endpoint,int recur,OctreeSegment* world,PathTesterPool* deadpool) {
    if (startpoint==endpoint) {return true;}
    if (recur>4) {return false;}
    std::cout<<"started: "<<recur<<" \n";
//    Location endpoint = Location(x2,y2,z2);
#define SNAPBIGGER(x) (x&(BLOCKLOCFULLMASK^((1<<(recur+1))-1)))
    Location snappedendpoint = Location(SNAPBIGGER(endpoint.x),SNAPBIGGER(endpoint.y),SNAPBIGGER(endpoint.z));
    PathTesterPool nodes = PathTesterPool(endpoint,recur);
    PathTesterPool spentnodes = PathTesterPool(endpoint,recur);
    PathTesterPool ghostaccepted = PathTesterPool(snappedendpoint,recur+1);
    PathTesterPool ghostrejected = PathTesterPool(snappedendpoint,recur+1);
    
    nodes.add(startpoint,nodes.hash(startpoint));
//    Location toinsert = Location(x1,y1,z1);
//    whooshlist.push_back(startpoint);
//    whooshlist.insert(std::pair<int,Location>(startpoint.roughdistanceto(endpoint),startpoint));
    while (true) {
//        std::sort(whooshlist.begin(), whooshlist.end(),endpoint);
//        Location xyz = whooshlist[0];
        int dist = nodes.getpromising();
        if (dist==0) {return true;}
        else if (dist==-1) {return false;}
        Location xyz = nodes.poppromising(dist);
        spentnodes.add(xyz,dist);
        
        
        Location biggersnap = Location(SNAPBIGGER(xyz.x),SNAPBIGGER(xyz.y),SNAPBIGGER(xyz.z));
        for (int i=0;i<8;i++) {
            int xi = (i&1)<<recur;
            int yi = ((i&2)>>1)<<recur;
            int zi = ((i&4)>>2)<<recur;
            Location checker = Location(biggersnap.x+xi,biggersnap.y+yi,biggersnap.z+zi);
            int hash = nodes.hash(checker);
            if (!spentnodes.contains(checker,hash) and !deadpool->contains(checker,hash)) {
                break;
            }
        }
        
        std::vector<Location> toinserts;
        if (world->giveXflag(xyz.x,xyz.y,xyz.z,recur,world)) {
            toinserts.push_back(Location(xyz.x+(1<<recur),xyz.y,xyz.z));
        }
        if (world->giveYflag(xyz.x,xyz.y,xyz.z,recur,world)) {
            toinserts.push_back(Location(xyz.x,xyz.y+(1<<recur),xyz.z));
        }
        if (world->giveZflag(xyz.x,xyz.y,xyz.z,recur,world)) {
            toinserts.push_back(Location(xyz.x,xyz.y,xyz.z+(1<<recur)));
        }
        if (world->giveXflag(xyz.x-(1<<recur),xyz.y,xyz.z,recur,world)) {
            toinserts.push_back(Location(xyz.x-(1<<recur),xyz.y,xyz.z));
        }
        if (world->giveYflag(xyz.x,xyz.y-(1<<recur),xyz.z,recur,world)) {
            toinserts.push_back(Location(xyz.x,xyz.y-(1<<recur),xyz.z));
        }
        if (world->giveZflag(xyz.x,xyz.y,xyz.z-(1<<recur),recur,world)) {
            toinserts.push_back(Location(xyz.x,xyz.y,xyz.z-(1<<recur)));
        }
        for (int k=0;k<toinserts.size();k++) {
            Location toinsert = toinserts[k];
            Location biggersnap = Location(SNAPBIGGER(toinsert.x),SNAPBIGGER(toinsert.y),SNAPBIGGER(toinsert.z));
            int hash1 = nodes.hash(toinsert);
            int hash2 = ghostaccepted.hash(biggersnap);
            if (!spentnodes.contains(toinsert, hash1) and !deadpool->contains(biggersnap,hash2)) {
                if (!ghostaccepted.contains(biggersnap, hash2) and !ghostrejected.contains(biggersnap, hash2)) {
                    if (testConnection(biggersnap,snappedendpoint,recur+1,world, &ghostrejected)) {
                        ghostaccepted.add(biggersnap,hash2);
                    } else {
                        ghostrejected.add(biggersnap,hash2);
                    }
                }
                if (ghostaccepted.contains(biggersnap, hash2)) {
                    nodes.add(toinsert,hash1);
                }
            }
        }
    }
}




