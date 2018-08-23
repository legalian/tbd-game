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

glm::vec3 clipplanes[6];
float clipdistances[6];



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
glm::mat4 clipcamera;
glm::mat4* defaultmatrix=NULL;

std::vector<GeomLense> thisframerender[] = {
    std::vector<GeomLense>(),
    std::vector<GeomLense>(),
    std::vector<GeomLense>()
};

void registergeom(uint8_t mat,GeomLense newgeom) {
    thisframerender[mat].push_back(newgeom);
}
void renderall() {
    for (int imaterial=0;imaterial<numberofmaterials;imaterial++) {
        if (thisframerender[imaterial].size()>0) {
            materials[imaterial]->open();
            for (int igeom=0;igeom<thisframerender[imaterial].size();igeom++){
                if (thisframerender[imaterial][igeom].target->matrix==NULL) {
                    thisframerender[imaterial][igeom].target->matrix=defaultmatrix;
                }
                thisframerender[imaterial][igeom].target->bake();
//                if (thisframerender[imaterial][igeom]->baked) {
                    materials[imaterial]->draw(thisframerender[imaterial][igeom]);
//                }
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
void setdefaultmatrix(glm::mat4* def) {
    defaultmatrix=def;
}
bool frustrumcul(glm::vec4 point,float radius) {
    glm::vec4 ar = (point*(*defaultmatrix));//*clipcamera;
    glm::vec3 p = glm::vec3(ar.x,ar.y,ar.z);
    return  glm::dot(p, clipplanes[0])+clipdistances[0]+radius>0 and
            glm::dot(p, clipplanes[1])+clipdistances[1]+radius>0 and
            glm::dot(p, clipplanes[2])+clipdistances[2]+radius>0 and
            glm::dot(p, clipplanes[3])+clipdistances[3]+radius>0 and
            glm::dot(p, clipplanes[4])+clipdistances[4]+radius>0 and
            glm::dot(p, clipplanes[5])+clipdistances[5]+radius>0;
}








