//
//  Organizational.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/13/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "renderterrain.h"
#include "octree.h"

GeomLense::GeomLense(OctreePortionAwareBranch* opab,GeomTerrain* a,int l) : target(a),reference(opab),lod(l) {}

void GeomTerrain::addVert(int vertex) {
    indexed.push_back(vertex);
}
void GeomTerrain::addVertExt(int vertex) {
    extreme.push_back(vertex);
}
void GeomTerrain::addVertExlay(glm::vec3 vertex) {
    exlay.push_back(vertex);
}
void GeomTerrain::addVertExlayNormal(glm::vec3 normal) {
    exlaynormal.push_back(normal);
}

void GeomTerrain::factor(int target) {
    coresize[target] = (int)indexed.size();
    if (lodlayers[target]!=0) delete[] lodlayers[target];
    lodlayers[target] = new int[indexed.size()+extreme.size()];
    for (int g=0;g<indexed.size();g++) lodlayers[target][g]=indexed[g];
    for (int g=0;g<extreme.size();g++) lodlayers[target][indexed.size()+g]=extreme[g];
//    throw;
//    if (extreme.size()) {
//        indexed_vertices.reserve(indexed.size()+extreme.size());
//        for (int y=0;y<extreme.size();y++) {
//            indexed.push_back(extrme[y]);
//        }
//        extreme.clear();
//    }
}

void GeomTerrain::emptyExlay() {
    exlay.clear();
    exlaynormal.clear();
}
GeomTerrain::GeomTerrain() {
    coresize = new int[MAX_WORLDFILE_GEOMSAVE+1];
    totalsize = new int[MAX_WORLDFILE_GEOMSAVE+1];
    lodlayers = new int*[MAX_WORLDFILE_GEOMSAVE+1];
    primbuffer = new GLuint[MAX_WORLDFILE_GEOMSAVE+1];
    for (int h=0;h<MAX_WORLDFILE_GEOMSAVE+1;h++) {
        coresize[h]=0;
        totalsize[h]=0;
        lodlayers[h]=0;
    }
}

//void GeomTerrain::lod0factorextreme() {
//    lod0coresize = (int)indexed_vertices.size();
//    if (extreme_batch.size()) {
//        indexed_vertices.reserve(indexed_vertices.size()+extreme_batch.size());
//        for (int y=0;y<extreme_batch.size();y++) {
//            indexed_vertices.push_back(extreme_batch[y]);
//        }
//        extreme_batch.clear();
//    }
//    lod0totalsize = (int)indexed_vertices.size();
//    coresize = lod0totalsize;
//}
//void GeomTerrain::crop() {
//    indexed_vertices.resize(coresize);
//    bakeamt = coresize;
//}
//void GeomTerrain::croptolod0() {
//    indexed_vertices.resize(lod0totalsize);
//    coresize = lod0totalsize;
//    bakeamt = lod0totalsize;
//}

//#ifdef WIREFRAMEDEBUG
//void GeomTerrain::addWireVert(glm::vec3 vertex) {
//    wire_debug_vertices.push_back(vertex);
//}
//void GeomTerrain::addWireColor(glm::vec3 color) {
//    wire_debug_colors.push_back(color);
//}
//#endif
//inline int GeomTerrain::size() {return (int)indexed_vertices.size();}
void GeomTerrain::bake(){
//    if (bakeamt!=indexed_normals.size() or indexed_normals.size()!=indexed_vertices.size()) {
////        size = (int)indexed_vertices.size();
//        if (bakeamt>indexed_normals.size()) {
//            indexed_normals.clear();
//            bakeamt=0;
//        }
//        indexed_normals.resize(bakeamt);
//        for (int i=bakeamt;i<indexed_vertices.size();i+=3) {
//            glm::vec3 trinormal = glm::normalize(glm::cross(indexed_vertices[i+1]-indexed_vertices[i],indexed_vertices[i+2]-indexed_vertices[i]));
////            glm::vec3 trinormal = glm::vec3(0,0,0);
//            indexed_normals.push_back(trinormal);
//            indexed_normals.push_back(trinormal);
//            indexed_normals.push_back(trinormal);
//        }
//        bakeamt=size();
//
//        glGenBuffers(1, &vertexbuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
//
//        glGenBuffers(1, &normalbuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
//        glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
//
//
//
//
//    }
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
//    for (auto it = endpoints.begin(); it != endpoints.end(); it++ ){
//        GeomTerrain* geom = it->first;
//        glm::mat4 composed_matrix;
//        extern glm::mat4 camera;
//        composed_matrix = camera*(*geom->matrix);
//        
//        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
//        
//        glBindBuffer(GL_ARRAY_BUFFER, geom->vertexbuffer);
//        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//        
//        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
//        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//        glDrawArrays(GL_TRIANGLES, it->second.first,it->second.second-it->second.first);
//    }
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
}
void ShaderTerrain::cleanup() {
    glDeleteProgram(programID);
}
void ShaderTerrain::draw(GeomLense gl) {
//    GeomTerrain* geom = gl.target;
//    if (gl.start!=-1) {
//        if (endpoints.count(gl.target)) {
//            if (endpoints[gl.target].second==gl.start) {
//                endpoints[gl.target].second=gl.end;
//                return;
//            }
//            int swap1 = endpoints[gl.target].first;
//            int swap2 = endpoints[gl.target].second;
//            endpoints[gl.target].first=gl.start;
//            endpoints[gl.target].second=gl.end;
//            gl.start=swap1;
//            gl.end=swap2;
//        } else {
//            endpoints[gl.target].first=gl.start;
//            endpoints[gl.target].second=gl.end;
//            return;
//        }
//    }
    glm::mat4 composed_matrix;
    extern glm::mat4 camera;
    composed_matrix = camera*(*gl.target->matrix);
    
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, gl.reference->vertbuffer);
    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, gl.reference->normbuffer);
    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//    if (gl.start==-1) {
//        glDrawArrays(GL_TRIANGLES, 0,geom->size());
//    } else {
//        glDrawArrays(GL_TRIANGLES, gl.start,gl.end-gl.start);
//    }
    
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.target->primbuffer[gl.lod]);

    glDrawElements(GL_TRIANGLES,gl.target->totalsize[gl.lod],GL_INT,(void*)0);

    
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





