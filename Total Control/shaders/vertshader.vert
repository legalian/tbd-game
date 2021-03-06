#version 120
// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
//attribute vec2 vertexUV;

//attribute vec3 vertexColor;
attribute vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
//varying vec3 fragmentColor;
varying vec3 normalColor;
//varying vec2 UV;

uniform mat4 MVP;

void main(){
    
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    
//    fragmentColor = vertexColor;
    normalColor = (vertexNormal_modelspace+1)/2.0;
    //UV = vertexUV;
    
}

