//
//  samplers.cpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "samplers.h"


void Sampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world){throw;}
void EmptySampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world) {}


void BoxSample::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world) {
    uint8_t test[CHSIZE+1][CHSIZE+1][CHSIZE+1];
    //    long alt = (((LONG_MAX/3)<<1)|1);
    for (int xi=0;xi<CHSIZE+1;xi++) {
        for (int yi=0;yi<CHSIZE+1;yi++) {
            for (int zi=0;zi<CHSIZE+1;zi++) {
                if (12<xi and xi<17 and 12<yi and yi<17 and 12<zi and zi<17) {
                    test[xi][yi][zi] = 2;
                } else {
                    test[xi][yi][zi] = 1;
                }
            }
        }
    }
    world.loadportion(x,y,z,test);
    for (int xi=0;xi<CHSIZE-1;xi++) {
        for (int yi=0;yi<CHSIZE-1;yi++) {
            for (int zi=0;zi<CHSIZE-1;zi++) {
                BlockLoc xt = xi + (x<<CHPOWER);
                BlockLoc yt = yi + (y<<CHPOWER);
                BlockLoc zt = zi + (z<<CHPOWER);
                if (test[xi][yi][zi] != test[xi+1][yi][zi]) {
                    world.conx(xt,yt,zt) = Edgedat(1,0,0,.5);
                }
                if (test[xi][yi][zi] != test[xi][yi+1][zi]) {
                    world.cony(xt,yt,zt) = Edgedat(0,1,0,.5);
                }
                if (test[xi][yi][zi] != test[xi][yi][zi+1]) {
                    world.conz(xt,yt,zt) = Edgedat(0,0,1,.5);
                }
            }
        }
    }
    world.hermitify(x,y,z);
}


//float SimpleSample::Samplelayer::sampleAt(double x,double y,double z) {throw;}
//
//void SimpleSample::populate(long x,long y, long z,Octree& world) {
//    uint8_t layerindecies[CHSIZE+1][CHSIZE+1][CHSIZE+1];
//    uint8_t realindecies[CHSIZE+1][CHSIZE+1][CHSIZE+1];
//    //    long alt = (((LONG_MAX/3)<<1)|1);
//    for (int xi=0;xi<CHSIZE+1;xi++) {
//        for (int yi=0;yi<CHSIZE+1;yi++) {
//            for (int zi=0;zi<CHSIZE+1;zi++) {
//                int xt=xi+(x<<CHPOWER);
//                int yt=yi+(y<<CHPOWER);
//                int zt=zi+(z<<CHPOWER);
//                int li=0;
//                while (layers[li]->sampleAt((double) xt,(double) yt,(double) zt)>0) {li++; if (li>layers.size()) break;}
//                layerindecies[xi][yi][zi] = li;
//                layerindecies[xi][yi][zi] = layers[li]->material;
//            }
//        }
//    }
//    world.loadportion(x,y,z,realindecies);
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=0;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                if (layerindecies[xi][yi][zi] != layerindecies[xi+1][yi][zi]) {
//                    world.conx(xi,yi,zi) = Edgedat(1,0,0,.5);
//                }
//                if (layerindecies[xi][yi][zi] != layerindecies[xi][yi+1][zi]) {
//                    world.cony(xi,yi,zi) = Edgedat(0,1,0,.5);
//                }
//                if (layerindecies[xi][yi][zi] != layerindecies[xi][yi][zi+1]) {
//                    world.conz(xi,yi,zi) = Edgedat(0,0,1,.5);
//                }
//            }
//        }
//    }
//    world.hermitify(x,y,z);

//}
//void SimpleSample::addlayer(Samplelayer* newlayer) {
//    layers.push_back(newlayer);
//}

