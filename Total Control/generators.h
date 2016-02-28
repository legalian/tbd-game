//
//  generators.h
//  Total Control
//
//  Created by Parker Lawrence on 1/16/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#ifndef __Total_Control__generators__
#define __Total_Control__generators__

#include <stdio.h>
#include "vox.h"

#include <random>

class Structure;
class Location;
class DetailedPortion;
class Environment;
class OctreePortion;

#define RANDSIZE 64
#define CHSIZE 256

class NoiseVolume {
private:
    float data[RANDSIZE][RANDSIZE][RANDSIZE];
//    inline double interp(float&, float&, float&);
    
public:
    NoiseVolume();
    float sample(double, double, double);
};

class Generator {
private:
    NoiseVolume sample1;
    NoiseVolume sample2;
    NoiseVolume sample3;
public:
    std::vector<Structure>* pool;
    void firstdump();
    OctreePortion* terrain_update(Structure* target,Location po);
    OctreePortion* makeportion(std::string filename,int (*)[CHSIZE][CHSIZE]);
    
};



#endif /* defined(__Total_Control__generators__) */
