#version 120

attribute vec3 vertexPosition_modelspace;
attribute vec3 vertexNormal_modelspace;
//attribute float reference;


//flat out Geomreference;

//varying VertexData
//{
//    vec3 Geomvertex;
//    vec3 Geomnormal;
//};
//out VertexData gl_out[3];
//varying VertexData
//{
//};
varying vec3 vertextoGeom;
varying vec3 normaltoGeom;
//varying float referencetoGeom;

uniform sampler2D textureAtlas;
uniform mat4 MVP;

void main(){
    
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    
    //    fragmentColor = vertexColor;
//    normalColor = (vertexNormal_modelspace+1)/2.0;
    vertextoGeom = vertexPosition_modelspace;
    normaltoGeom = vertexNormal_modelspace;
//    referencetoGeom = reference;
    //UV = vertexUV;
    
}
