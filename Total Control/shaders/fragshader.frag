#version 120

// Interpolated values from the vertex shaders
//varying vec2 UV;

// Values that stay constant for the whole mesh.
//uniform sampler2D myTextureSampler;

//varying vec3 fragmentColor;
varying vec3 normalColor;


void main()
{
//    gl_FragColor = vec4(1,1,1,1);
    gl_FragColor = vec4(normalColor.r,normalColor.g,normalColor.b,1);//vec4(fragmentColor*texture2D( myTextureSampler, UV ).rgb, 1);
//    gl_FragColor = vec4(fragmentColor.r,fragmentColor.g,fragmentColor.b,1);//vec4(fragmentColor, 1);
    
}