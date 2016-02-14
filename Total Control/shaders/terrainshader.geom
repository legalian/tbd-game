
#version 120
//#extension GL_ARB_geometry_shader4 : enable

//layout (triangles) in;
////layout (triangles) out;
//layout (triangles, max_vertices = 3) out;

varying in vec3 Geomvertex[3];
varying in vec3 Geomnormal[3];
//flat in uint Geomreference[3];

varying out vec3 Fragvertex;
varying out vec3 Fragnormal;
varying out vec3 Fragbarysian;
//flat out vec3 Fragreference;

void main()
{
    int i;
    
    for(i = 0; i < gl_VerticesIn; i++)
    {
        gl_Position = gl_PositionIn[i];
        Fragvertex = Geomvertex[i];
        Fragnormal = Geomnormal[i];
//        Fragreference = vec3(Geomreference[0],Geomreference[1],Geomreference[2]);
        Fragbarysian = vec3(int(i==0),int(i==1),int(i==2));
        
        EmitVertex();
    }
    EndPrimitive();
}