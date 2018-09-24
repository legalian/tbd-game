//
//  shader.cpp
//  Total Control
//
//  Created by Parker Lawrence on 8/7/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "shader.h"
#include "texture.h"



using namespace std;

GLuint LoadComputeShader(const char * compute_file_path){
    
    GLuint ComputerShaderID = glCreateShader(GL_COMPUTE_SHADER);
    
    GLint Result = GL_FALSE;
    int InfoLogLength = 0;
    
    LoadSingleShader(ComputerShaderID, compute_file_path, Result, InfoLogLength);
    
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, ComputerShaderID);
    glLinkProgram(ProgramID);
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(ComputerShaderID);
    
    return ProgramID;
}
void LoadSingleShader(GLint shaderID,const char * file_path,GLint &Result, int &InfoLogLength) {
    std::string ShaderCode;
    std::ifstream ShaderStream(file_path, std::ios::in);
    if(ShaderStream.is_open()){
        std::string Line = "";
        while(getline(ShaderStream, Line))
            ShaderCode += "\n" + Line;
        ShaderStream.close();
    }else{
        printf("Impossible to open %s.\n", file_path);
        getchar();
        return;
    }
    
    printf("Compiling shader : %s\n", file_path);
    char const * SourcePointer = ShaderCode.c_str();
    glShaderSource(shaderID, 1, &SourcePointer , NULL);
    glCompileShader(shaderID);
    
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> shaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &shaderErrorMessage[0]);
        printf("%s\n", &shaderErrorMessage[0]);
    }
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLint Result = GL_FALSE;
    int InfoLogLength = 0;
    
    LoadSingleShader(VertexShaderID, vertex_file_path, Result, InfoLogLength);
    LoadSingleShader(FragmentShaderID, fragment_file_path, Result, InfoLogLength);
    
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}
GLuint LoadShaders(const char * vertex_file_path,const char * geometry_file_path,const char * fragment_file_path){
    
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    
    GLint Result = GL_FALSE;
    int InfoLogLength = 0;
    
    LoadSingleShader(VertexShaderID, vertex_file_path, Result, InfoLogLength);
    LoadSingleShader(FragmentShaderID, fragment_file_path, Result, InfoLogLength);
    LoadSingleShader(GeometryShaderID, geometry_file_path, Result, InfoLogLength);
    
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glAttachShader(ProgramID, GeometryShaderID);
    glLinkProgram(ProgramID);
    
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(GeometryShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}


