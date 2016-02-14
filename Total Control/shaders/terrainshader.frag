#version 120

// Interpolated values from the vertex shaders
//varying vec3 Geomvertex;

varying vec3 Fragvertex;
varying vec3 Fragnormal;
varying vec3 Fragbarysian;
// Values that stay constant for the whole mesh.
uniform sampler2D textureAtlas;



//varying vec3 fragmentColor;

void main()
{
//    gl_FragColor = vec4(texture2D( textureAtlas, Geomvertex.xz/2.0 ).rgb, 1);
    
    gl_FragColor = vec4(Fragbarysian.rgb, 1);
    
}