//
//  samplers.cpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "samplers.h"


//NoiseVolume sample1;
//NoiseVolume sample2;
//NoiseVolume sample3;











//NoiseVolume::NoiseVolume() {
//    for (int xi=0;xi<RANDSIZE;xi++) {
//        for (int yi=0;yi<RANDSIZE;yi++) {
//            for (int zi=0;zi<RANDSIZE;zi++) {
//                data[xi][yi][zi] = (((float)std::rand())/(RAND_MAX/2))-1;
//            }
//        }
//    }
//}
//float NoiseVolume::sample(double x, double y, double z) {
//    //    std::cout<<x<<","<<y<<","<<z<<"\n";
////    int xmax = MOD((int)ceil(x*RANDSIZE),RANDSIZE);
////    int xmin = MOD((int)floor(x*RANDSIZE),RANDSIZE);
//    int xmax = (int)ceil(x*RANDSIZE)&63;
//    int xmin = (int)floor(x*RANDSIZE)&63;
//    float xi = (x*RANDSIZE)-floor(x*RANDSIZE);
//    int ymax = (int)ceil(y*RANDSIZE)&63;
//    int ymin = (int)floor(y*RANDSIZE)&63;
//    float yi = (y*RANDSIZE)-floor(y*RANDSIZE);
//    int zmax = (int)ceil(z*RANDSIZE)&63;
//    int zmin = (int)floor(z*RANDSIZE)&63;
//    float zi = (z*RANDSIZE)-floor(z*RANDSIZE);
////    return 0.0;
////    std::cout<<xmin<<","<<ymin<<","<<zmin<<"\n";
////    return data[xmin][ymin][zmin];
////    return interp(interp(interp(data[xmax][ymax][zmax],data[xmax][ymax][zmin],zi),interp(data[xmax][ymin][zmax],data[xmax][ymin][zmin],zi),yi),interp(interp(data[xmin][ymax][zmax],data[xmin][ymax][zmin],zi),interp(data[xmin][ymin][zmax],data[xmin][ymin][zmin],zi),yi),xi);
//    return
//    ((data[xmax][ymax][zmax]*zi+data[xmax][ymax][zmin]*(1-zi))*yi+
//    (data[xmax][ymin][zmax]*zi+data[xmax][ymin][zmin]*(1-zi))*(1-yi))*xi+
//    ((data[xmin][ymax][zmax]*zi+data[xmin][ymax][zmin]*(1-zi))*yi+
//    (data[xmin][ymin][zmax]*zi+data[xmin][ymin][zmin]*(1-zi))*(1-yi))*(1-xi);
//}
//
//void Sampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Structure& world){throw;}
//void EmptySampler::populate(BlockLoc x,BlockLoc y, BlockLoc z,Structure& world) {}
//
//void BoxSample::populate(BlockLoc x,BlockLoc y, BlockLoc z,Structure& world) {
//
//    BlockId (*test)[CHSIZE+1][CHSIZE+1] = new BlockId[CHSIZE+1][CHSIZE+1][CHSIZE+1]();
//    //    long alt = (((LONG_MAX/3)<<1)|1);
//    for (int xi=0;xi<CHSIZE+1;xi++) {
//        for (int yi=0;yi<CHSIZE+1;yi++) {
//            for (int zi=0;zi<CHSIZE+1;zi++) {
//                if (12<xi and xi<17 and 12<yi and yi<17 and 12<zi and zi<17) {
//                    test[xi][yi][zi] = 2;
//                } else {
//                    test[xi][yi][zi] = 1;
//                }
//            }
//        }
//    }
//    world.loadportion(x,y,z,test);
//    for (int xi=0;xi<CHSIZE-1;xi++) {
//        for (int yi=0;yi<CHSIZE-1;yi++) {
//            for (int zi=0;zi<CHSIZE-1;zi++) {
//                BlockLoc xt = ASBLOCKLOC(xi + (x<<CHPOWER));
//                BlockLoc yt = ASBLOCKLOC(yi + (y<<CHPOWER));
//                BlockLoc zt = ASBLOCKLOC(zi + (z<<CHPOWER));
//                if (test[xi][yi][zi] != test[xi+1][yi][zi]) {
//                    world.data->xcon(xt,yt,zt) = Edgedat((float)1,(float)0,(float)0,(float).5);
//                }
//                if (test[xi][yi][zi] != test[xi][yi+1][zi]) {
//                    world.data->ycon(xt,yt,zt) = Edgedat((float)0,(float)1,(float)0,(float).5);
//                }
//                if (test[xi][yi][zi] != test[xi][yi][zi+1]) {
//                    world.data->zcon(xt,yt,zt) = Edgedat((float)0,(float)0,(float)1,(float).5);
//                }
//            }
//        }
//    }
//    delete [] test;
//}

inline float fGetOffset(float fValue1, float fValue2) {
    float fDelta = fValue1 - fValue2;
    if(fDelta == 0.0) {
        return 0.5;
    }
    return fValue1/fDelta;
}
Sampler::Sampler() {
    test = LoadComputeShader("/Users/legalian/dev/wasteland_kings/Total Control/samplers/standard.comp");
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    //just allocate enough amount of memory
    glBufferData(GL_SHADER_STORAGE_BUFFER,(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1)*sizeof(float),0, GL_DYNAMIC_READ);
}
void Sampler::populate(int x,int y, int z,Structure& world) {
    //assign both g_variables.
    g_sampler=this;
//    glUseProgram(test);
//    glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame*0.01f);
//    glDispatchCompute(512/16, 512/16, 1); // 512^2 threads in blocks of 16^2
//    checkErrors("Dispatch compute shader");


   
//    int blockIndex = glGetProgramResourceIndex(test,GL_SHADER_STORAGE_BLOCK,"OutputBuffer");
//    if (blockIndex != GL_INVALID_INDEX) {
//        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,9,ssbo);
//        glShaderStorageBlockBinding(test, blockIndex,9);
//    } else {
//        System.err.println("Warning: binding " + name + " not found");
//    }

    g_idfield = new BlockId[(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1)];
    for (int x=0;x<=CHSIZE;x++) {
    for (int y=0;y<=CHSIZE;y++) {
    for (int z=0;z<=CHSIZE;z++) {
    g_idfield[x+y*(CHSIZE+1)+z*(CHSIZE+1)*(CHSIZE+1)] = (x>3 and x<50 and y>3 and y<50 and z>3 and z<50)?2:1;
    }
    }
    }
    std::cout<<"MARK\n";


    world.expandchunk(x,y,z);
    world.data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,world.depth,makeOctree(0,0,0,CHPOWER),world.data);
    
    delete [] g_idfield;

//    float (*samplebuffer)[CHSIZE+1][CHSIZE+1] = new float[CHSIZE+1][CHSIZE+1][CHSIZE+1];
//    BlockId (*ids)[CHSIZE+1][CHSIZE+1] = new BlockId[CHSIZE+1][CHSIZE+1][CHSIZE+1];
////    BlockId ids[CHSIZE+1][CHSIZE+1][CHSIZE+1];
//    //    long alt = (((LONG_MAX/3)<<1)|1);
//    for (int xi=0;xi<CHSIZE+1;xi++) {
//        for (int yi=0;yi<CHSIZE+1;yi++) {
//            for (int zi=0;zi<CHSIZE+1;zi++) {//,yi,zi);//
////                samplebuffer[xi][yi][zi] = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER),zi+(z<<CHPOWER));
//                if (samplebuffer[xi][yi][zi]>0) {
//                    ids[xi][yi][zi] = 2;
//                } else {
//                    ids[xi][yi][zi] = 1;
//                }
//            }
//        }
//    }
//    world.loadportion(x,y,z,ids);
//    float offset = .01;
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=0;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                int xt = ASBLOCKLOC(xi + (x<<CHPOWER));
//                int yt = ASBLOCKLOC(yi + (y<<CHPOWER));
//                int zt = ASBLOCKLOC(zi + (z<<CHPOWER));
//                if (ids[xi][yi][zi] != ids[xi+1][yi][zi]) {
//                    float push = fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi+1][yi][zi]);
////                    float xdelta = sample(xi+(x<<CHPOWER)+push+offset,yi+(y<<CHPOWER),zi+(z<<CHPOWER))-sample(xi+(x<<CHPOWER)+push-offset,yi+(y<<CHPOWER),zi+(z<<CHPOWER));
////                    float ydelta = sample(xi+(x<<CHPOWER)+push,yi+(y<<CHPOWER)+offset,zi+(z<<CHPOWER))-sample(xi+(x<<CHPOWER)+push,yi+(y<<CHPOWER)-offset,zi+(z<<CHPOWER));
////                    float zdelta = sample(xi+(x<<CHPOWER)+push,yi+(y<<CHPOWER),zi+(z<<CHPOWER)+offset)-sample(xi+(x<<CHPOWER)+push,yi+(y<<CHPOWER),zi+(z<<CHPOWER)-offset);
////                    float delta = sqrt(xdelta*xdelta+ydelta*ydelta+zdelta*zdelta);
////                    world.data->xcon(xt,yt,zt) = Edgedat(xdelta/delta,ydelta/delta,zdelta/delta,push);
//                    world.data->xcon(xt,yt,zt) = Edgedat(1,0,0,push);
//                }
//                if (ids[xi][yi][zi] != ids[xi][yi+1][zi]) {
//                    float push = fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi][yi+1][zi]);
////                    float xdelta = sample(xi+(x<<CHPOWER)+offset,yi+(y<<CHPOWER)+push,zi+(z<<CHPOWER))-sample(xi+(x<<CHPOWER)-offset,yi+(y<<CHPOWER)+push,zi+(z<<CHPOWER));
////                    float ydelta = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)+push+offset,zi+(z<<CHPOWER))-sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)+push-offset,zi+(z<<CHPOWER));
////                    float zdelta = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)+push,zi+(z<<CHPOWER)+offset)-sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)+push,zi+(z<<CHPOWER)-offset);
////                    float delta = sqrt(xdelta*xdelta+ydelta*ydelta+zdelta*zdelta);
////                    world.data->ycon(xt,yt,zt) = Edgedat(xdelta/delta,ydelta/delta,zdelta/delta,push);
//                    world.data->ycon(xt,yt,zt) = Edgedat(0,1,0,push);
//                }
//                if (ids[xi][yi][zi] != ids[xi][yi][zi+1]) {
//                    float push = fGetOffset(samplebuffer[xi][yi][zi],samplebuffer[xi][yi][zi+1]);
////                    float xdelta = sample(xi+(x<<CHPOWER)+offset,yi+(y<<CHPOWER),zi+(z<<CHPOWER)+push)-sample(xi+(x<<CHPOWER)-offset,yi+(y<<CHPOWER),zi+(z<<CHPOWER)+push);
////                    float ydelta = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)+offset,zi+(z<<CHPOWER)+push)-sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER)-offset,zi+(z<<CHPOWER)+push);
////                    float zdelta = sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER),zi+(z<<CHPOWER)+push+offset)-sample(xi+(x<<CHPOWER),yi+(y<<CHPOWER),zi+(z<<CHPOWER)+push-offset);
////                    float delta = sqrt(xdelta*xdelta+ydelta*ydelta+zdelta*zdelta);
////                    world.data->zcon(xt,yt,zt) = Edgedat(xdelta/delta,ydelta/delta,zdelta/delta,push);
//                    world.data->zcon(xt,yt,zt) = Edgedat(0,0,1,push);
//                }
//            }
//        }
//    }
//    delete [] samplebuffer;
//    delete [] ids;
}
//inline float SimpleTerrainSample::fGetOffset(float fValue1, float fValue2)
//{
//    float fDelta = fValue1 - fValue2;
//    if(fDelta == 0.0)
//    {
//        return 0.5;
//    }
//    return fValue1/fDelta;
//}
//inline float Sampler::sample(float x,float y,float z) {
//    extern NoiseVolume sample1;
//    extern NoiseVolume sample2;
//    extern NoiseVolume sample3;
//    float density = -y/2.0;
//    const float mu = 128.0*3;
//    density -= 32*(TRUNC_DIV(y/2.0,32));
//    if (density < -127*3 or density > 127*3) {return density;}
//    density += sample3.sample(x/(mu*64),y/(mu*64),z/(mu*64))*148;
//    if (density < -63*3 or density > 63*3) {return density;}
//    density += sample3.sample(x/(mu*32),y/(mu*32),z/(mu*32))*72;
//    if (density < -31*3 or density > 31*3) {return density;}
//    density += sample2.sample(x/(mu*16),y/(mu*16),z/(mu*16))*48;
//    if (density < -15*3 or density > 15*3) {return density;}
//    density += sample1.sample(x/(mu*8 ),y/(mu*8 ),z/(mu*8 ))*24;
//    if (density < -7*3 or density > 7*3) {return density;}
//    density += sample3.sample(x/(mu*4 ),y/(mu*4 ),z/(mu*4 ))*12;
//    if (density < -3*3 or density > 3*3) {return density;}
//    density += sample2.sample(x/(mu*2 ),y/(mu*2 ),z/(mu*2 ))*6;
//    if (density < -1*3 or density > 1*3) {return density;}
//    density += sample1.sample(x/(mu   ),y/(mu   ),z/(mu   ))*3;
////    float density = 40-sqrt((x*x)+(y*y)+(z*z));
////    float density = 40-(abs(x)+abs(y)+abs(z));
////    float density = 40-std::max(std::max(abs(x),abs(y)),abs(z));
////    float density = 40-std::min(std::min(abs(x),abs(y)),abs(z));
////    float density = cos(x/5.0)*sin(y/5.0)+cos(y/5.0)*sin(z/5.0)+cos(z/5.0)*sin(x/5.0);
//    return density;
//
////    float xdif = fmodf(fabsf(x),CHSIZE) - (CHSIZE/2.0);
////    float ydif = fmodf(fabsf(y),CHSIZE) - (CHSIZE/2.0);
////    float zdif = fmodf(fabsf(z),CHSIZE) - (CHSIZE/2.0);
//////    float xdif = x - (CHSIZE/2.0);
//////    float ydif = y - (CHSIZE/2.0);
//////    float zdif = z - (CHSIZE/2.0);
////
////    float dist = sqrtf(xdif*xdif+ydif*ydif);
//////    float dist = fabsf(xdif)+fabsf(ydif)+fabsf(zdif);
//////    float dist = std::max(fabsf(zdif),std::max(fabsf(xdif),fabsf(ydif)));
////    return 25-dist;
//
//}

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

