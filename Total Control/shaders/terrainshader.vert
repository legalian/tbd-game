#version 120

attribute vec3 vertexPosition_modelspace;
attribute vec3 vertexNormal_modelspace;
//attribute uint reference;


varying vec3 Geomvertex;
varying vec3 Geomnormal;
//flat out Geomreference;

uniform sampler2D textureAtlas;

uniform mat4 MVP;

void main(){
    
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    
    //    fragmentColor = vertexColor;
//    normalColor = (vertexNormal_modelspace+1)/2.0;
    Geomvertex = vertexPosition_modelspace;
    Geomnormal = vertexNormal_modelspace;
    //UV = vertexUV;
    
}
