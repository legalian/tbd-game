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


void registergeom(uint8_t,GeomTerrain*);
void renderall();
void cleanup();


#endif /* materials_hpp */
