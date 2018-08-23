//
//  materials.hpp
//  Total Control
//
//  Created by Parker on 3/16/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef materials_hpp
#define materials_hpp

#include <stdio.h>
#include "renderterrain.h"
#include "constants.h"
#include <vector>


//ShaderTerrain* materials[];


void registergeom(uint8_t,GeomLense);
void renderall();
void cleanup();
void setdefaultmatrix(glm::mat4*);
bool frustrumcul(glm::vec4,float);

#endif /* materials_hpp */
