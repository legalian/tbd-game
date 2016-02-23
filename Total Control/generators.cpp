//
//  generators.cpp
//  Total Control
//
//  Created by Parker Lawrence on 1/16/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "generators.h"


void Generator::firstdump() {
    Structure mainland;
    mainland.bounded = false;
    mainland.structureid = "mainland";
    mainland.transform = glm::translate(glm::mat4(),glm::vec3(3,3,3));
    pool->push_back(mainland);
    Structure spinrock;
    spinrock.bounded = true;
    spinrock.upperbounds = Location(0,0,0);
    spinrock.lowestbounds = Location(-1,-1,-1);
    spinrock.structureid = "spinnyrock";
    glm::mat4 mat1 = glm::translate(glm::mat4(),glm::vec3(0,100,0));
    spinrock.transform = mat1;
    pool->push_back(spinrock);
}
int (*Generator::terrain_update(Structure* target,Location po))[CHSIZE][CHSIZE] {
//    DetailedPortion* myportion = new DetailedPortion();
//    int*** = new int[CHSIZE][CHSIZE][CHSIZE];
    int (*data)[CHSIZE][CHSIZE] = new int[CHSIZE][CHSIZE][CHSIZE];
    
    for (int xi=0;xi<CHSIZE;xi++) {
        for (int yi=0;yi<CHSIZE;yi++) {
            for (int zi=0;zi<CHSIZE;zi++) {
                int xt = po.x*CHSIZE+xi;
                int yt = po.y*CHSIZE+yi;
                int zt = po.z*CHSIZE+zi;
                if (target->structureid == "mainland") {
//                    if ((xi==0 or yi==0 or zi==0) and ((xt+yt+zt)%10==0)) {
//                        myportion->data[xi][yi][zi] = 3;
//                    }
//                    continue;
    //                if ((MOD(xt,11)<4)and(MOD(yt,11)<4)and(MOD(zt,11)<4)) {
    //                    if (MOD(xt+yt+zt,2)==1) {
    //                        myportion->data[xi][yi][zi] = 5;
    //                    } else {
    //                        myportion->data[xi][yi][zi] = 4;
    //                    }
    //                    continue;
    //                }
    //                continue;
                    double density = -yt;
                    double mu = 64.0;
                    density += sample1.sample(xt/(mu*3),yt/(mu*3),zt/(mu*3))*3;
                    density += sample2.sample(xt/(mu*6),yt/(mu*6),zt/(mu*6))*6;
                    density += sample3.sample(xt/(mu*12),yt/(mu*12),zt/(mu*12))*12;
                    density += sample1.sample(xt/(mu*24),yt/(mu*24),zt/(mu*24))*24;
                    density += sample2.sample(xt/(mu*48),yt/(mu*48),zt/(mu*48))*48;
                    density += sample3.sample(xt/(mu*72),yt/(mu*72),zt/(mu*72))*72;
                    density += sample3.sample(xt/(mu*148),yt/(mu*148),zt/(mu*148))*148;
                    density -= 32*(TRUNC_DIV(yt,32));
                    double scale = 3;
                    if (density>scale) {
                        data[xi][yi][zi] = 3;
                    } else if (density>(scale/2.0)) {
                        data[xi][yi][zi] = 2;
                    } else if (density>0) {
                        data[xi][yi][zi] = 1;
                    } else {
                        data[xi][yi][zi] = 0;
                    }
                    
    //                if (xt*xt+zt*zt>yt*yt*2) {
    //                    myportion->data[xi][yi][zi] = 1;
    //                } else {
    //                    myportion->data[xi][yi][zi] = 0;
    //                }
                }
                else if (target->structureid == "spinnyrock") {
//                                        if ((xi==0 or yi==0 or zi==0) and ((xt+yt+zt)%10==0)) {
//                                            myportion->data[xi][yi][zi] = 3;
//                                        }
//                                        continue;
                    double density = 40-sqrt(xt*xt+yt*yt+zt*zt);
                    double mu = 64.0;
                    density += sample1.sample(xt/(mu*3),yt/(mu*3),zt/(mu*3))*3;
                    density += sample2.sample(xt/(mu*6),yt/(mu*6),zt/(mu*6))*6;
                    density += sample3.sample(xt/(mu*12),yt/(mu*12),zt/(mu*12))*12;
                    density += sample1.sample(xt/(mu*24),yt/(mu*24),zt/(mu*24))*24;
//                    density += sample2.sample(xt/(mu*48),yt/(mu*48),zt/(mu*48))*48;
//                    density += sample3.sample(xt/(mu*72),yt/(mu*72),zt/(mu*72))*72;
//                    density += sample3.sample(xt/(mu*148),yt/(mu*148),zt/(mu*148))*148;
//                    density -= 32*(TRUNC_DIV(yt,32));
                    //oesguh
                    double scale = 3;
                    if (density>scale) {
                        data[xi][yi][zi] = 3;
                    } else if (density>(scale/2.0)) {
                        data[xi][yi][zi] = 2;
                    } else if (density>0) {
                        data[xi][yi][zi] = 1;
                    } else {
                        data[xi][yi][zi] = 0;
                    }
                    
                }
                
            }
        }
    }
    return data;
}




//double NoiseVolume::interp(float &x, float &y, float &a) {
//    return (x*a)+(y*(1-a));
//}
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








