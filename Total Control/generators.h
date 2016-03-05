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
#define CHSIZE 128

class NoiseVolume {
private:
    float data[RANDSIZE][RANDSIZE][RANDSIZE];
//    inline double interp(float&, float&, float&);
    
public:
    NoiseVolume();
    float sample(double, double, double);
};

class FeatureEngine {
public:
    NoiseVolume* sample1;
    NoiseVolume* sample2;
    NoiseVolume* sample3;
    
    FeatureEngine(NoiseVolume*,NoiseVolume*, NoiseVolume*);
    virtual float sample(float x, float y, float z);
    virtual uint8_t index(float density);
};
class WorldEngine : public FeatureEngine {
public:
    WorldEngine(NoiseVolume*,NoiseVolume*, NoiseVolume*);
    float sample(float x, float y, float z) override;
    uint8_t index(float density) override;
};
class MeteorEngine : public FeatureEngine {
public:
    MeteorEngine(NoiseVolume*,NoiseVolume*, NoiseVolume*);
    float sample(float x, float y, float z) override;
    uint8_t index(float density) override;
};


class Generator {
private:
    NoiseVolume sample1;
    NoiseVolume sample2;
    NoiseVolume sample3;
    MeteorEngine metsample;
    WorldEngine worsample;
    
public:
    std::vector<Structure>* pool;
    void firstdump();
    Generator() : metsample(&sample1,&sample2,&sample3) , worsample(&sample1,&sample2,&sample3) {}
    OctreePortion* terrain_update(Structure* target,Location po);
    OctreePortion* makeportion(std::string filename,uint8_t (*)[CHSIZE][CHSIZE]);
    
    
};



#endif /* defined(__Total_Control__generators__) */
