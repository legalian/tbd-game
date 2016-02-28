//
//  Organizational.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/13/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//


#include "render.h"

//using namespace glm;

void Geom::addVert(double x,double y,double z) {
    glm::vec3 vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    indexed_vertices.push_back(vertex);
}
void Geom::addNormal(double x,double y,double z) {
    glm::vec3 normal;
    normal.x = x;
    normal.y = y;
    normal.z = z;
    indexed_normals.push_back(normal);
}
void Geom::addVert(glm::vec3 vertex) {
    indexed_vertices.push_back(vertex);
}
void Geom::addNormal(glm::vec3 normal) {
    indexed_normals.push_back(normal);
}
void Geom::dumpverts() {
    for (int ivert=0;ivert<indexed_vertices.size();ivert++) {
        std::cout<<indexed_vertices[ivert][0]<<","<<indexed_vertices[ivert][1]<<","<<indexed_vertices[ivert][2]<<"\n";
    }
}
void Geom::bake(){
    size = indexed_vertices.size();
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
}
void Geom::unbake(){
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
}
void Geom::rebake(){
    unbake();
    bake();
}
void Geom::assumeNormals() {
    for (int i=(int)indexed_normals.size();i<indexed_vertices.size();i+=3) {
        glm::vec3 trinormal = glm::normalize(glm::cross(indexed_vertices[i+1]-indexed_vertices[i+0],indexed_vertices[i+2]-indexed_vertices[i+0]));
        indexed_normals.push_back(trinormal);
        indexed_normals.push_back(trinormal);
        indexed_normals.push_back(trinormal);
    }
}
void GeomVNC::addColor(double r,double g,double b) {
    glm::vec3 color;
    color.x = r;
    color.y = g;
    color.z = b;
    indexed_colors.push_back(color);
}
void GeomVNC::addColor(glm::vec3 color) {
    indexed_colors.push_back(color);
}
void GeomVNC::bake(){
    size = indexed_vertices.size();
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_colors.size() * sizeof(glm::vec3), &indexed_colors[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
}
void GeomVNC::unbake(){
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteBuffers(1, &normalbuffer);
}
void GeomTerrain::addReference(uint8_t a) {
    indexed_reference.push_back(a);
}
void GeomTerrain::bake(){
    size = indexed_vertices.size();
    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &referencebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, referencebuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_reference.size() * sizeof(uint8_t), &indexed_reference[0], GL_STATIC_DRAW);
}
void GeomTerrain::unbake(){
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &referencebuffer);
}
//void Geom::join(Geom* o) {
//    indexed_colors.insert( indexed_colors.end(), o->indexed_colors.begin(), o->indexed_colors.end() );
//    indexed_normals.insert( indexed_normals.end(), o->indexed_normals.begin(), o->indexed_normals.end() );
//    indexed_vertices.insert( indexed_vertices.end(), o->indexed_vertices.begin(), o->indexed_vertices.end() );
//}

//};

//class ShaderObject {
//private:
//    GLuint programID;
//    GLuint matrixID;
//    GLuint vertexPosition_modelspaceID;
//    GLuint vertexcolorID;
//    glm::mat4 cam;
//public:
void ShaderObject::mountshaders() {
    programID = LoadShaders( "shaders/vertshader.vert", "shaders/fragshader.frag" );
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
}
void ShaderObject::setcam(glm::mat4 newcam) {
    cam=newcam;
}
void ShaderObject::open() {
    glUseProgram(programID);
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
}
void ShaderObject::close() {
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
}
void ShaderVNC::mountshaders() {
    programID = LoadShaders( "shaders/vertshader.vert", "shaders/fragshader.frag" );
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
    vertexcolorID = glGetAttribLocation(programID, "vertexColor");
}

void ShaderVNC::open() {
    glUseProgram(programID);
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
    glEnableVertexAttribArray(vertexcolorID);
}
void ShaderVNC::close() {
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
    glDisableVertexAttribArray(vertexcolorID);
}
void ShaderVNC::draw(GeomVNC* geom) {
    if (geom->baked) {
        glm::mat4 composed_matrix;
        if (geom->matrix == NULL) {
            composed_matrix = cam*glm::mat4(1.0f);
        } else {
            composed_matrix = cam*(*geom->matrix);
            //            std::cout<<"yesmatrix\n";
        }
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->colorbuffer);
        glVertexAttribPointer(vertexcolorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, (int)geom->size);
    }
}
void ShaderVNC::draw(Geom* geom) {
    if (geom->baked) {
        glm::mat4 composed_matrix;
        if (geom->matrix == NULL) {
            composed_matrix = cam*glm::mat4(1.0f);
        } else {
            composed_matrix = cam*(*geom->matrix);
            //            std::cout<<"yesmatrix\n";
        }
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
//        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
//        glVertexAttribPointer(vertexcolorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, (int)geom->size);
    }
}
void ShaderObject::cleanup() {
    glDeleteProgram(programID);
}
//};
void ShaderTerrain::mountshaders() {
    programID = LoadShaders( "shaders/terrainshader.vert","shaders/terrainshader.geom", "shaders/terrainshader.frag" );
    
    matrixID = glGetUniformLocation(programID, "MVP");
    vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
    vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
    
    
    referenceID = glGetAttribLocation(programID, "reference");
    
    texture = loadBMP_custom("uvtemplate.bmp");
    textureID  = glGetUniformLocation(programID, "TextureAtlas");
}
void ShaderTerrain::open() {
    glUseProgram(programID);
    glEnableVertexAttribArray(vertexPosition_modelspaceID);
    glEnableVertexAttribArray(vertexNormal_modelspaceID);
    glEnableVertexAttribArray(referenceID);
}
void ShaderTerrain::close() {
    glDisableVertexAttribArray(vertexPosition_modelspaceID);
    glDisableVertexAttribArray(vertexNormal_modelspaceID);
    glDisableVertexAttribArray(referenceID);
}
void ShaderTerrain::draw(GeomTerrain* geom) {
    if (geom->baked) {
        glm::mat4 composed_matrix;
        composed_matrix = cam*(*geom->matrix);
        
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &(composed_matrix[0][0]));
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->vertexbuffer);
        glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->referencebuffer);
                glVertexAttribPointer(referenceID, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
//        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
//        glVertexAttribPointer(vertexcolorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glBindBuffer(GL_ARRAY_BUFFER, geom->normalbuffer);
        glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, (int)geom->size);
    }
}






