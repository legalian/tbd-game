//
//  generators.cpp
//  Total Control
//
//  Created by Parker Lawrence on 1/16/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "generators.h"

//
//void Generator::firstdump() {
////    Structure mainland;
////    mainland.bounded = false;
////    mainland.structureid = "mainland";
////    mainland.transform = glm::translate(glm::mat4(),glm::vec3(3,3,3));
////    pool->push_back(mainland);
////    Structure spinrock;
////    spinrock.bounded = true;
////    spinrock.upperbounds = Location(0,0,0);
////    spinrock.lowestbounds = Location(-1,-1,-1);
////    spinrock.structureid = "spinnyrock";
////    glm::mat4 mat1 = glm::translate(glm::mat4(),glm::vec3(0,100,0));
////    spinrock.transform = mat1;
////    pool->push_back(spinrock);
//}
//OctreePortion* Generator::terrain_update(Structure* target,Location po) {
////    DetailedPortion* myportion = new DetailedPortion();
//    //    int*** = new int[CHSIZE][CHSIZE][CHSIZE];
//    float prepass[CHSIZE][CHSIZE][CHSIZE];
//    uint8_t data[CHSIZE][CHSIZE][CHSIZE];
//    
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=0;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                int xt = po.x*CHSIZE+xi;
//                int yt = po.y*CHSIZE+yi;
//                int zt = po.z*CHSIZE+zi;
//                if (target->structureid == "mainland") {
//                    prepass[xi][yi][zi] = worsample.sample(xt,yt,zt);
//                    data[xi][yi][zi] = worsample.index(prepass[xi][yi][zi]);
//                }
//                else if (target->structureid == "spinnyrock") {
//                    prepass[xi][yi][zi] = metsample.sample(xt,yt,zt);
//                    data[xi][yi][zi] = metsample.index(prepass[xi][yi][zi]);
//                    
//                }
//                
//            }
//        }
//    }
//    
//    
//    std::string filename = "structures/"+target->structureid+po.tostring()+".str";
//    OctreePortion* toreturn = new OctreePortion(data);
////    for (int xi=0;xi<CHSIZE-1;xi++) {
////        for (int yi=0;yi<CHSIZE-1;yi++) {
////            for (int zi=0;zi<CHSIZE-1;zi++) {
////                if (data[xi][yi][zi]!=data[xi+1][yi][zi]) {
////                    
////                    toreturn.data->setxcon(xi,yi,zi,
////                                           edgedat(prepass[xi][yi][zi]/(prepass[xi][yi][zi]-prepass[xi+1][yi][zi]),
////                                                   
////                                                   ));
////                    
////                }
////                if (data[xi][yi][zi]!=data[xi][yi+1][zi]) {
////                    (prepass[xi][yi][zi]/(prepass[xi][yi][zi]-prepass[xi+1][yi][zi]));
////                    
////                }
////                if (data[xi][yi][zi]!=data[xi][yi][zi+1]) {
////                    (prepass[xi][yi][zi]/(prepass[xi][yi][zi]-prepass[xi+1][yi][zi]));
////                    
////                }
////            }
////        }
////    }
//    return toreturn;
//
////    return makeportion(filename,data);
//}
//
//
//
//FeatureEngine::FeatureEngine(NoiseVolume* nya,NoiseVolume* nyu, NoiseVolume* nyo) {
//    sample1 = nya;
//    sample2 = nyu;
//    sample3 = nyo;
//}
//MeteorEngine::MeteorEngine(NoiseVolume* nya,NoiseVolume* nyu, NoiseVolume* nyo) : FeatureEngine(nya,nyu,nyo) {}
//WorldEngine::WorldEngine(NoiseVolume* nya,NoiseVolume* nyu, NoiseVolume* nyo) : FeatureEngine(nya,nyu,nyo) {}
//
//float FeatureEngine::sample(float x, float y, float z) {throw;}
//uint8_t FeatureEngine::index(float density) {throw;}
//
//float WorldEngine::sample(float x, float y, float z) {
//    float density = -y/2.0;
//    const float mu = 128.0;
//    density += sample1->sample(x/(mu*3),y/(mu*3),z/(mu*3))*3;
//    density += sample2->sample(x/(mu*6),y/(mu*6),z/(mu*6))*6;
//    density += sample3->sample(x/(mu*12),y/(mu*12),z/(mu*12))*12;
//    density += sample1->sample(x/(mu*24),y/(mu*24),z/(mu*24))*24;
//    density += sample2->sample(x/(mu*48),y/(mu*48),z/(mu*48))*48;
//    density += sample3->sample(x/(mu*72),y/(mu*72),z/(mu*72))*72;
//    density += sample3->sample(x/(mu*148),y/(mu*148),z/(mu*148))*148;
//    density -= 32*(TRUNC_DIV(y/2.0,32));
//    return density;
//    
//}
//uint8_t WorldEngine::index(float density) {
//    if (density>0) {
//        return 1;
//    } else {
//        return 0;
//    }
//}
//
//float MeteorEngine::sample(float x, float y, float z) {
//    float density = 80-sqrt(x*x+y*y+z*z)*2;
//    const float mu = 128.0;
//    density += sample1->sample(x/(mu*3),y/(mu*3),z/(mu*3))*3;
//    density += sample2->sample(x/(mu*6),y/(mu*6),z/(mu*6))*6;
//    density += sample3->sample(x/(mu*12),y/(mu*12),z/(mu*12))*12;
//    density += sample1->sample(x/(mu*24),y/(mu*24),z/(mu*24))*24;
//    return density;
//    
//}
//uint8_t MeteorEngine::index(float density) {
//    if (density>0) {
//        return 1;
//    } else {
//        return 0;
//    }
//}
//
//
//
//
//
////double NoiseVolume::interp(float &x, float &y, float &a) {
////    return (x*a)+(y*(1-a));
////}
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
//
//
//
//
//
//
//
