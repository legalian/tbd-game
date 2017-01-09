//
//  Organizational.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/13/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "renderterrain.h"

void GeomTerrain::addVert(double x,double y,double z) {
    glm::vec3 vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    indexed_vertices.push_back(vertex);
}
void GeomTerrain::addNormal(double x,double y,double z) {
    glm::vec3 normal;
    normal.x = x;
    normal.y = y;
    normal.z = z;
    indexed_normals.push_back(normal);
}
void GeomTerrain::addVert(glm::vec3 vertex) {
    indexed_vertices.push_back(vertex);
}
void GeomTerrain::addNormal(glm::vec3 normal) {
    indexed_normals.push_back(normal);
}
void GeomTerrain::erase(int start,int size) {
    indexed_vertices.erase(indexed_vertices.begin()+start*3,indexed_vertices.begin()+(start+size)*3);
//    indexed_normals.erase(indexed_normals.begin()+start*3,indexed_normals.begin()+(start+size)*3);
}
#ifdef WIREFRAMEDEBUG
void GeomTerrain::addWireVert(double x,double y,double z) {
    glm::vec3 vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    wire_debug_vertices.push_back(vertex);
}
void GeomTerrain::addWireColor(double x,double y,double z) {
    glm::vec3 color;
    color.x = x;
    color.y = y;
    color.z = z;
    wire_debug_colors.push_back(color);
}
void GeomTerrain::addWireVert(glm::vec3 vertex) {
    wire_debug_vertices.push_back(vertex);
}
void GeomTerrain::addWireColor(glm::vec3 color) {
    wire_debug_colors.push_back(color);
}
#endif
void GeomTerrain::bake(){
    if (!baked) {
        size = (int)indexed_vertices.size();
        indexed_normals.clear();
        for (int i=(int)indexed_normals.size();i<indexed_vertices.size();i+=3) {
            glm::vec3 trinormal = glm::normalize(glm::cross(indexed_vertices[i+1]-indexed_vertices[i],indexed_vertices[i+2]-indexed_vertices[i]));
            indexed_normals.push_back(trinormal);
            indexed_normals.push_back(trinormal);
            indexed_normals.push_back(trinormal);
        }
        
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
#ifdef WIREFRAMEDEBUG
        
        wiresize = (int)wire_debug_vertices.size();
        glGenBuffers(1, &wire_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, wire_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, wire_debug_vertices.size() * sizeof(glm::vec3), &wire_debug_vertices[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &wire_colorbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, wire_colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, wire_debug_colors.size() * sizeof(glm::vec3), &wire_debug_colors[0], GL_STATIC_DRAW);
        
#endif
        baked = true;
        
    }
}
//void GeomTerrain::markcoregeom() {
//    coregeomsize = (int)indexed_vertices.size();
//}
//void GeomTerrain::dumpeverything() {
//    baked = false;
//    glDeleteBuffers(1, &vertexbuffer);
//    glDeleteBuffers(1, &normalbuffer);
//    indexed_vertices.erase(indexed_vertices.begin(),indexed_vertices.end());
//    indexed_normals.erase(indexed_normals.begin(),indexed_normals.end());
//}
//void GeomTerrain::dumpextrabits() {
//    if (baked) {
////        glDeleteBuffers(1, &vertexbuffer);
////        glDeleteBuffers(1, &normalbuffer);
//        baked = false;
////        throw;
//    }
//    
////    std::cout<<coregeomsize;
////    indexed_vertices.erase(indexed_vertices.begin()+coregeomsize,indexed_vertices.end());
////    indexed_normals.erase(indexed_normals.begin()+coregeomsize,indexed_normals.end());
//}
//void GeomTerrain::unbake(){
//    glDeleteBuffers(1, &vertexbuffer);
//    glDeleteBuffers(1, &normalbuffer);
//}
//void GeomTerrain::rebake(){
//    unbake();
//    bake();
//}

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
void ShaderTerrain::draw(GeomTerrain* geom) {
    glm::mat4 composed_matrix;
    extern glm::mat4 camera;
    composed_matrix = camera*(*geom->matrix);
    
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, geom->vertexbuffer);
    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glDrawArrays(GL_TRIANGLES, 0, (int)geom->size);
}

#ifdef WIREFRAMEDEBUG
void DebugShader::mountshaders() {
    programID = LoadShaders( "/Users/legalian/dev/wasteland_kings/Total Control/shaders/wirevertshader.vert", "/Users/legalian/dev/wasteland_kings/Total Control/shaders/fragshader.frag" );
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexColor");
}
void DebugShader::draw(GeomTerrain* geom) {
//    std::cout<<"debugshader drawn with :"<<geom->wiresize<<" vertecies.\n";
    glm::mat4 composed_matrix;
    extern glm::mat4 camera;
    composed_matrix = camera*(*geom->matrix);
    
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
    
    glBindBuffer(GL_ARRAY_BUFFER, geom->wire_vertexbuffer);
    glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, geom->wire_colorbuffer);
    glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glDrawArrays(GL_LINES, 0, (int)geom->wiresize);
}
#endif





