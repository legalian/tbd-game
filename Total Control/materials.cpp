//
//  materials.cpp
//  Total Control
//
//  Created by Parker on 3/16/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "materials.h"

int numberofmaterials = 3;

ShaderTerrain* materials[] = {
#ifdef WIREFRAMEDEBUG
    new DebugShader(),
#else
    NULL,
#endif
    NULL,
    new ShaderTerrain()
};//ShaderTerrain* debugshader = new DebugShader();


uint8_t materialprops[] = {
    //precidence; don't draw polygons between values of equal precidence, otherwise use to determine polygon winding order
    0,
    0,
    1
};

uint8_t materialattribs[] = {
    //1stbit - structual stability
    0,
    0,
    1
};

glm::mat4 camera;

std::vector<GeomTerrain*> thisframerender[] = {
    std::vector<GeomTerrain*>(),
    std::vector<GeomTerrain*>(),
    std::vector<GeomTerrain*>()
};

void registergeom(uint8_t mat,GeomTerrain* newgeom) {
    thisframerender[mat].push_back(newgeom);
}
void renderall() {
    for (int imaterial=0;imaterial<numberofmaterials;imaterial++) {
        if (thisframerender[imaterial].size()>0) {
            materials[imaterial]->open();
            for (int igeom=0;igeom<thisframerender[imaterial].size();igeom++){
                thisframerender[imaterial][igeom]->bake();
                if (thisframerender[imaterial][igeom]->baked) {
                    materials[imaterial]->draw(thisframerender[imaterial][igeom]);
                    
                }
            }
            materials[imaterial]->close();
            thisframerender[imaterial].clear();
        }
    }
}
void cleanup() {
    for (int imaterial=0;imaterial<numberofmaterials;imaterial++) {
        if (materials[imaterial] != NULL) {
            materials[imaterial]->cleanup();
            delete materials[imaterial];
        }
    }
}