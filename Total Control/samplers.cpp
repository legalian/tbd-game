//
//  samplers.cpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "samplers.h"
#include "constants.h"


NoiseVolume sample1;
NoiseVolume sample2;
NoiseVolume sample3;


NoiseVolume::NoiseVolume() {
    for (int xi=0;xi<RANDSIZE;xi++) {
        for (int yi=0;yi<RANDSIZE;yi++) {
            for (int zi=0;zi<RANDSIZE;zi++) {
                data[xi][yi][zi] = (((float)std::rand())/(RAND_MAX/2))-1;
            }
        }
    }
}
float NoiseVolume::sample(double x, double y, double z) {
    //    std::cout<<x<<","<<y<<","<<z<<"\n";
//    int xmax = MOD((int)ceil(x*RANDSIZE),RANDSIZE);
//    int xmin = MOD((int)floor(x*RANDSIZE),RANDSIZE);
    int xmax = (int)ceil(x*RANDSIZE)&63;
    int xmin = (int)floor(x*RANDSIZE)&63;
    float xi = (x*RANDSIZE)-floor(x*RANDSIZE);
    int ymax = (int)ceil(y*RANDSIZE)&63;
    int ymin = (int)floor(y*RANDSIZE)&63;
    float yi = (y*RANDSIZE)-floor(y*RANDSIZE);
    int zmax = (int)ceil(z*RANDSIZE)&63;
    int zmin = (int)floor(z*RANDSIZE)&63;
    float zi = (z*RANDSIZE)-floor(z*RANDSIZE);
//    return 0.0;
//    std::cout<<xmin<<","<<ymin<<","<<zmin<<"\n";
//    return data[xmin][ymin][zmin];
//    return interp(interp(interp(data[xmax][ymax][zmax],data[xmax][ymax][zmin],zi),interp(data[xmax][ymin][zmax],data[xmax][ymin][zmin],zi),yi),interp(interp(data[xmin][ymax][zmax],data[xmin][ymax][zmin],zi),interp(data[xmin][ymin][zmax],data[xmin][ymin][zmin],zi),yi),xi);
    return
    ((data[xmax][ymax][zmax]*zi+data[xmax][ymax][zmin]*(1-zi))*yi+
    (data[xmax][ymin][zmax]*zi+data[xmax][ymin][zmin]*(1-zi))*(1-yi))*xi+
    ((data[xmin][ymax][zmax]*zi+data[xmin][ymax][zmin]*(1-zi))*yi+
    (data[xmin][ymin][zmax]*zi+data[xmin][ymin][zmin]*(1-zi))*(1-yi))*(1-xi);
}

void Sampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world){throw;}
void EmptySampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world) {}

void BoxSample::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world) {
    
    BlockId (*test)[CHSIZE+1][CHSIZE+1] = new BlockId[CHSIZE+1][CHSIZE+1][CHSIZE+1]();
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
                BlockLoc xt = ASBLOCKLOC(xi + (x<<CHPOWER));
                BlockLoc yt = ASBLOCKLOC(yi + (y<<CHPOWER));
                BlockLoc zt = ASBLOCKLOC(zi + (z<<CHPOWER));
                if (test[xi][yi][zi] != test[xi+1][yi][zi]) {
                    world.data->xcon(xt,yt,zt) = Edgedat((float)1,(float)0,(float)0,(float).5);
                }
                if (test[xi][yi][zi] != test[xi][yi+1][zi]) {
                    world.data->ycon(xt,yt,zt) = Edgedat((float)0,(float)1,(float)0,(float).5);
                }
                if (test[xi][yi][zi] != test[xi][yi][zi+1]) {
                    world.data->zcon(xt,yt,zt) = Edgedat((float)0,(float)0,(float)1,(float).5);
                }
            }
        }
    }
    world.hermitify(x,y,z);
    delete [] test;
}
void SimpleTerrainSample::populate(BlockLoc x,BlockLoc y, BlockLoc z,Octree& world) {
    
    float (*samplebuffer)[CHSIZE+1][CHSIZE+1] = new float[CHSIZE+1][CHSIZE+1][CHSIZE+1];
    BlockId (*ids)[CHSIZE+1][CHSIZE+1] = new BlockId[CHSIZE+1][CHSIZE+1][CHSIZE+1];
//    BlockId ids[CHSIZE+1][CHSIZE+1][CHSIZE+1];
    //    long alt = (((LONG_MAX/3)<<1)|1);
    for (int xi=0;xi<CHSIZE+1;xi++) {
        for (int yi=0;yi<CHSIZE+1;yi++) {
            for (int zi=0;zi<CHSIZE+1;zi++) {//,yi,zi);//
                samplebuffer[xi][yi][zi] = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER),zi+(z<<CHPOWER));
                if (samplebuffer[xi][yi][zi]>0) {
                    ids[xi][yi][zi] = 2;
                } else {
                    ids[xi][yi][zi] = 1;
                }
            }
        }
    }
    world.loadportion(x,y,z,ids);
    for (int xi=0;xi<CHSIZE;xi++) {
        for (int yi=0;yi<CHSIZE;yi++) {
            for (int zi=0;zi<CHSIZE;zi++) {
                BlockLoc xt = ASBLOCKLOC(xi + (x<<CHPOWER));
                BlockLoc yt = ASBLOCKLOC(yi + (y<<CHPOWER));
                BlockLoc zt = ASBLOCKLOC(zi + (z<<CHPOWER));
                if (ids[xi][yi][zi] != ids[xi+1][yi][zi]) {
                    world.data->xcon(xt,yt,zt) = Edgedat(1,0,0,fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi+1][yi][zi]));
                }
                if (ids[xi][yi][zi] != ids[xi][yi+1][zi]) {
                    world.data->ycon(xt,yt,zt) = Edgedat(0,1,0,fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi][yi+1][zi]));
                }
                if (ids[xi][yi][zi] != ids[xi][yi][zi+1]) {
                    world.data->zcon(xt,yt,zt) = Edgedat(0,0,1,fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi][yi][zi+1]));
                }
            }
        }
    }
    world.hermitify(x,y,z);
    delete [] samplebuffer;
    delete [] ids;
}
inline float SimpleTerrainSample::fGetOffset(float fValue1, float fValue2)
{
    float fDelta = fValue1 - fValue2;
    if(fDelta == 0.0)
    {
        return 0.5;
    }
    return fValue1/fDelta;
}
inline float SimpleTerrainSample::sample(float x,float y,float z) {
    extern NoiseVolume sample1;
    extern NoiseVolume sample2;
    extern NoiseVolume sample3;
    float density = -y/2.0;
    const float mu = 128.0*3;
    density += sample1.sample(x/(mu   ),y/(mu   ),z/(mu   ))*3;
    density += sample2.sample(x/(mu*2 ),y/(mu*2 ),z/(mu*2 ))*6;
    density += sample3.sample(x/(mu*4 ),y/(mu*4 ),z/(mu*4 ))*12;
    density += sample1.sample(x/(mu*8 ),y/(mu*8 ),z/(mu*8 ))*24;
    density += sample2.sample(x/(mu*16),y/(mu*16),z/(mu*16))*48;
    density += sample3.sample(x/(mu*32),y/(mu*32),z/(mu*32))*72;
    density += sample3.sample(x/(mu*64),y/(mu*64),z/(mu*64))*148;
    density -= 32*(TRUNC_DIV(y/2.0,32));
    return density;
    
//    float xdif = fmodf(fabsf(x),CHSIZE) - (CHSIZE/2.0);
//    float ydif = fmodf(fabsf(y),CHSIZE) - (CHSIZE/2.0);
//    float zdif = fmodf(fabsf(z),CHSIZE) - (CHSIZE/2.0);
////    float xdif = x - (CHSIZE/2.0);
////    float ydif = y - (CHSIZE/2.0);
////    float zdif = z - (CHSIZE/2.0);
//    
//    float dist = sqrtf(xdif*xdif+ydif*ydif);
////    float dist = fabsf(xdif)+fabsf(ydif)+fabsf(zdif);
////    float dist = std::max(fabsf(zdif),std::max(fabsf(xdif),fabsf(ydif)));
//    return 25-dist;
    
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

