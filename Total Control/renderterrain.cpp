//
//  Organizational.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/13/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "renderterrain.h"
#include "octree.h"

int numberofmaterials = 3;

ShaderTerrain* materials[] = {
#ifdef WIREFRAMEDEBUG
    new DebugShader(),
#else
    NULL,
#endif
    NULL,
    new ShaderTerrain()
};

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


void glassert() {
    auto r = glGetError();
    if (r!=0) {
        switch (r) {
            case 0x0500:      std::cout<<"Invalid enum\n";throw;
            case 0x0501:     std::cout<<"Invalid value\n";throw;
            case 0x0502: std::cout<<"Invalid operation\n";throw;
            case 0x0503:    std::cout<<"Stack overflow\n";throw;
            case 0x0504:   std::cout<<"Stack underflow\n";throw;
            case 0x0505:     std::cout<<"Out of memory\n";throw;
            default:         std::cout<<"Unknown error\n";throw;
        }
    }
}



GeomLense::GeomLense(OctreePortionAwareBranch* opab,GeomTerrain* a,int l) : target(a),reference(opab),lod(l) {}

void GeomTerrain::addVert(int vertex) {
    indexed.push_back(vertex);
}
void GeomTerrain::addVertExlay(glm::vec3 vertex) {
    exlay.push_back(vertex);
}
void GeomTerrain::addVertExlayNormal(glm::vec3 normal) {
    exlaynormal.push_back(normal);
}

void GeomTerrain::factor(int target) {
    totalsize[target] = (unsigned int)indexed.size();
    if (lodlayers[target]!=0) delete[] lodlayers[target];
    lodlayers[target] = new unsigned int[indexed.size()];
    for (int g=0;g<indexed.size();g++) lodlayers[target][g]=indexed[g];
    baked = false;
    indexed.clear();
}

void GeomTerrain::emptyExlay() {
    exlay.clear();
    exlaynormal.clear();
}
GeomTerrain::GeomTerrain() {
    totalsize = new unsigned int[MAX_WORLDFILE_GEOMSAVE+1];
    lodlayers = new unsigned int*[MAX_WORLDFILE_GEOMSAVE+1];
    primbuffer = new GLuint[MAX_WORLDFILE_GEOMSAVE+1];
    for (int h=0;h<MAX_WORLDFILE_GEOMSAVE+1;h++) {
        totalsize[h]=0;
        lodlayers[h]=0;
    }
}

void OctreePortionAwareBranch::bake() {
    if (!mthreadinitialized) {
        glGenBuffers(MAX_WORLDFILE_GEOMSAVE+1,vertbuffer);
        glGenBuffers(MAX_WORLDFILE_GEOMSAVE+1,normbuffer);
        mthreadinitialized=true;
    }
    if (!baked) {
        for (int g=0;g<=MAX_WORLDFILE_GEOMSAVE;g++) {
            glBindBuffer(GL_ARRAY_BUFFER,vertbuffer[g]);
            glBufferData(GL_ARRAY_BUFFER,vertecies[g].size()*sizeof(glm::vec3),&(vertecies[g][0]),GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER,normbuffer[g]);
            glBufferData(GL_ARRAY_BUFFER,normals[g].size()*sizeof(glm::vec3),&(normals[g][0]),GL_STATIC_DRAW);
        }
        baked=true;
    }
}
void GeomTerrain::bake() {
    if (!mthreadinitialized) {
        glGenBuffers(MAX_WORLDFILE_GEOMSAVE+1,primbuffer);
        glGenBuffers(1,&exlaybuffer);
        glGenBuffers(1,&exlaynormalbuffer);
        mthreadinitialized=true;
    }
    if (!baked) {
        for (int g=0;g<=MAX_WORLDFILE_GEOMSAVE;g++) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,primbuffer[g]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,totalsize[g]*sizeof(unsigned int),lodlayers[g],GL_STATIC_DRAW);
        }
        baked=true;
    }
    if (!exbaked) {
        glBindBuffer(GL_ARRAY_BUFFER,exlaybuffer);
        glBufferData(GL_ARRAY_BUFFER,exlay.size()*sizeof(glm::vec3),&(exlay[0]),GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,exlaynormalbuffer);
        glBufferData(GL_ARRAY_BUFFER,exlaynormal.size()*sizeof(glm::vec3),&(exlaynormal[0]),GL_STATIC_DRAW);
        exbaked=true;
    }
}






void ShaderTerrain::mountshaders() {
    programID = LoadShaders( "/Users/legalian/dev/wasteland_kings/Total Control/shaders/vertshader.vert", "/Users/legalian/dev/wasteland_kings/Total Control/shaders/fragshader.frag" );
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
}
void ShaderTerrain::open() {
    glUseProgram(programID);
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
}
void ShaderTerrain::close() {
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
}
void ShaderTerrain::cleanup() {
    glDeleteProgram(programID);
}
void ShaderTerrain::draw(GeomLense gl) {
    glm::mat4 composed_matrix;
    extern glm::mat4 camera;
    composed_matrix = camera*(*gl.target->matrix);
    
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, gl.reference->vertbuffer[gl.lod]);
    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, gl.reference->normbuffer[gl.lod]);
    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.target->primbuffer[gl.lod]);
    glDrawElements(GL_TRIANGLES,gl.target->totalsize[gl.lod],GL_UNSIGNED_INT,(void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, gl.target->exlaybuffer);
    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, gl.target->exlaynormalbuffer);
    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES,0,(int)gl.target->exlay.size());
}

#ifdef WIREFRAMEDEBUG
void DebugShader::mountshaders() {
    programID = LoadShaders( "/Users/legalian/dev/wasteland_kings/Total Control/shaders/wirevertshader.vert", "/Users/legalian/dev/wasteland_kings/Total Control/shaders/fragshader.frag" );
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexColor");
}
void DebugShader::draw(GeomLense gl) {
    GeomTerrain* geom = gl.target;
//    std::cout<<"debugshader drawn with :"<<geom->wiresize<<" vertecies.\n";
    glm::mat4 composed_matrix;
    extern glm::mat4 camera;
    composed_matrix = camera*(*geom->matrix);
    
//    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
//    
//    glBindBuffer(GL_ARRAY_BUFFER, geom->wire_vertexbuffer);
//    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    
//    glBindBuffer(GL_ARRAY_BUFFER, geom->wire_colorbuffer);
//    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    
//    glDrawArrays(GL_LINES, 0, (int)geom->wiresize);
}
#endif






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
                thisframerender[imaterial][igeom].reference->bake();
                //TODO: ensure it's all drawn in the right order
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
            glm::dot(p, clipplanes[4])+clipdistances[4]+radius>0;// and
//            glm::dot(p, clipplanes[5])+clipdistances[5]+radius>0;
}







