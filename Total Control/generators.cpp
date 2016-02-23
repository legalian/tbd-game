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
    std::string filename = "structures/"+target->structureid+po.tostring()+".str";

    return data;
}


PortionPointer Generator::makeportion(std::string filename,int (*dat)[CHSIZE][CHSIZE]) {
    
    ComplexPortion* tenguess = new ComplexPortion();
    tenguess->defaults[0] = dat[0][0][0];
    tenguess->defaults[1] = dat[0][0][0];
    tenguess->defaults[2] = dat[0][0][0];
    tenguess->defaults[3] = dat[CHSIZE-1][CHSIZE-1][CHSIZE-1];
    tenguess->defaults[4] = dat[CHSIZE-1][CHSIZE-1][CHSIZE-1];
    tenguess->defaults[5] = dat[CHSIZE-1][CHSIZE-1][CHSIZE-1];
    bool solidsofar[] = {true,true,true,true,true,true};
    for (int z=0;z<CHSIZE;z++) {
        for (int x=0;x<CHSIZE;x++) {
            for (int y=0;y<CHSIZE;y++) {
                if (dat[z][x][y]!=tenguess->defaults[0]) {
                    if (solidsofar[0]) {
                        tenguess->depths[0] = z;
                    }
                    solidsofar[0] = false;
                }
                if (dat[x][z][y]!=tenguess->defaults[1]) {
                    if (solidsofar[1]) {
                        tenguess->depths[1] = z;
                    }
                    solidsofar[1] = false;
                }
                if (dat[x][y][z]!=tenguess->defaults[2]) {
                    if (solidsofar[2]) {
                        tenguess->depths[2] = z;
                    }
                    solidsofar[2] = false;
                }
                if (dat[CHSIZE-1-z][x][y]!=tenguess->defaults[3]) {
                    if (solidsofar[3]) {
                        tenguess->depths[3] = CHSIZE-1-z;
                    }
                    solidsofar[3] = false;
                }
                if (dat[x][CHSIZE-1-z][y]!=tenguess->defaults[4]) {
                    if (solidsofar[4]) {
                        tenguess->depths[4] = CHSIZE-1-z;
                    }
                    solidsofar[4] = false;
                }
                if (dat[x][y][CHSIZE-1-z]!=tenguess->defaults[5]) {
                    if (solidsofar[5]) {
                        tenguess->depths[5] = CHSIZE-1-z;
                    }
                    solidsofar[5] = false;
                }
            }
        }
    }
    std::ofstream myFile(filename, std::ios::out | std::ios::binary);
    if (solidsofar[0]) {
        int solidId = dat[0][0][0];
        char info[1] = {'s'};
        myFile.write(info,1);
        myFile.write((char*)&solidId,sizeof(unsigned int));
        delete tenguess;
        return PortionPointer(new SolidPortion(solidId));
    } else if (false) {
        //            portions.insert(std::pair<Location,PortionPointer>(po,PortionPointer(new DetailedPortion(dat))));
        //            char info[1] = {'d'};
        //            myFile.write(info,1);
        //            myFile.write((char*)dat,sizeof(unsigned int)*CHSIZE*CHSIZE*CHSIZE);
    } else {
        char info[1] = {'c'};
        myFile.write(info,1);
        myFile.write((char*)tenguess->defaults,sizeof(unsigned int)*6);
        myFile.write((char*)tenguess->depths,sizeof(int)*6);
        int size = (1+tenguess->depths[3]-tenguess->depths[0])*(1+tenguess->depths[4]-tenguess->depths[1])*(1+tenguess->depths[5]-tenguess->depths[2]);
        tenguess->data = new unsigned int[size];
        int index=0;
        for (int z = tenguess->depths[2];z<=tenguess->depths[5];z++) {
            for (int y = tenguess->depths[1];y<=tenguess->depths[4];y++) {
                for (int x = tenguess->depths[0];x<=tenguess->depths[3];x++) {
                    tenguess->data[index] = dat[x][y][z];
                    index++;
                }
            }
        }
        myFile.write((char*)(tenguess->data),sizeof(unsigned int)*size);
        return PortionPointer(tenguess);
    }
    delete[] dat;
    myFile.close();

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








