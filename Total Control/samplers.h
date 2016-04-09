//
//  samplers.hpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef samplers_hpp
#define samplers_hpp

#include <stdio.h>
#include <octree.h>

class Sampler {
public:
    virtual void populate(BlockLoc,BlockLoc,BlockLoc,Octree&);
};
class EmptySampler : public Sampler {
public:
    void populate(BlockLoc,BlockLoc,BlockLoc,Octree&) override;
};
class BoxSample : public Sampler {
public:
    void populate(BlockLoc,BlockLoc,BlockLoc,Octree&) override;
};
//class SimpleSample : public Sampler {
//public:
//    class Samplelayer {
//    public:
//        uint8_t material;
//        float void sampleAt(double,double,double);
//    };
//    std::vector<Samplelayer*> layers;
//    void populate(long,long,long,Octree&) override;
//    void addlayer(Samplelayer*);
//};



#endif /* samplers_hpp */
