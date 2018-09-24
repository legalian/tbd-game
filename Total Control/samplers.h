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
#include <vox.h>
#include "constants.h"

class Structure;



class Sampler {
    GLuint test;
    GLuint ssbo;
public:
    Sampler();
    void populate(int,int,int,Structure&);
    Edgedat xedge(int,int,int);
    Edgedat yedge(int,int,int);
    Edgedat zedge(int,int,int);
    glm::vec3 normsamp(float,float,float);
};




#endif /* samplers_hpp */
