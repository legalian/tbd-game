//
//  main.cpp
//  Wasteland Kings
//
//  Created by Parker Lawrence on 8/4/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include </usr/local/include/GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


#include "environment.h"
#include "renderterrain.h"
#include "octree.h"


#include <pthread.h>





#define ASSERT(b) {int err=b;if (err) {std::cout<<"FATAL ERROR: "<<err<<"\n";throw;}}
#define ASSERT_ASSIGN(decl,exp) {int err=0;decl=exp;if (err or !decl) throw;}

cl_context gcl_context;
cl_command_queue gcl_commands;
cl_device_id gcl_device_id;

//#define PLOTTER_MODE

Sampler::Sampler() {
#ifndef PLOTTER_MODE
    std::ifstream file = std::ifstream("/Users/legalian/dev/wasteland_kings/Total Control/samplers/standard.comp");
    std::string KernelSource((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
#endif
#ifdef PLOTTER_MODE
//    gp.mount(magnitude(gp.get3("xyz"))-70);
    Gen x = gp.get("x");
    Gen y = gp.get("y");
    Gen z = gp.get("z");
    Gen3 xyz = gp.get3("xyz");
//    gp.mount(y-x);
//    gp.set("cube",50-max(max(abs(x),abs(y)),abs(z)));
//    gp.set("sphere",60-magnitude(xyz));
//    gp.mount(min(gp.get("cube"),0-gp.get("sphere")));
//    gp.mount(60-magnitude(xyz));
    gp.mount(x*x+y*y-z*z+x-80);
//    gp.mount()
    std::string KernelSource = gp.mounted.mountable;
#endif
    size_t start_pos = 0;
    while((start_pos = KernelSource.find("CHSIZE", start_pos)) != std::string::npos) {
        KernelSource.replace(start_pos,6,std::to_string(CHSIZE));
        start_pos += sizeof("CHSIZE")-std::to_string(CHSIZE).size();
    }
    const char* ajja = (const char*)KernelSource.c_str();
    ASSERT_ASSIGN(program,clCreateProgramWithSource(gcl_context,1,&ajja,NULL,&err));
    int gare = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (gare != CL_SUCCESS) {
        size_t len;
        char *buffer;
        clGetProgramBuildInfo(program,gcl_device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        buffer = new char[len];
        clGetProgramBuildInfo(program,gcl_device_id, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        printf("%s\n", buffer);
        exit(1);
    }
    ASSERT_ASSIGN(kernel,clCreateKernel(program, "square", &err));
    size_t maxWorkGroupSize;
    ASSERT(clGetKernelWorkGroupInfo(kernel,gcl_device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL));
//    cl_mem input  = clCreateBuffer(context, CL_MEM_READ_ONLY,  sizeof(float) * charn, NULL, NULL);
    ASSERT_ASSIGN(output,clCreateBuffer(gcl_context, CL_MEM_WRITE_ONLY, sizeof(float)*(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1), NULL, NULL));
}
#ifndef PLOTTER_MODE
glm::vec3 operator/(const glm::vec3& a,const float& b) {return a*(1/b);}
inline float clrand(float n) {float o=sin(n)*58.5453123;return o-floor(o);}//43758.5453123   437
inline float clrand3(float n,float m,float o) {return clrand(o*78.233+n*12.9898+m*67.9482);}
inline float clcmix(float a,float b,float c) {return a+(b-a)*sin(1.57079632679*c);}
inline float clgridnoise(glm::vec3 st) {
    glm::vec3 c = ceil(st);
    glm::vec3 f = floor(st);
    glm::vec3 fpos = st-f;
    return clcmix(
        clcmix(
            clcmix(clrand3(f.x,f.y,f.z),clrand3(c.x,f.y,f.z),fpos.x),
            clcmix(clrand3(f.x,c.y,f.z),clrand3(c.x,c.y,f.z),fpos.x),
            fpos.y),
        clcmix(
            clcmix(clrand3(f.x,f.y,c.z),clrand3(c.x,f.y,c.z),fpos.x),
            clcmix(clrand3(f.x,c.y,c.z),clrand3(c.x,c.y,c.z),fpos.x),
            fpos.y),
        fpos.z);
}

glm::vec3 Sampler::normal(float x,float y,float z) {
    return normalize(glm::vec3(sample(x-.001,y,z)-sample(x+.001,y,z),sample(x,y-.001,z)-sample(x,y+.001,z),sample(x,y,z-.001)-sample(x,y,z+.001)));
//    float o = abs(sample(x,y,z));
//    if ((o>0 and v<0) or (o<0 and v>0)) {
//        std::cout<<o<<","<<v<<"<aoidjfoaisjdf\n";
////        throw;
//    }
//    return glm::vec3((o-v)/v);
}
float Sampler::sample(float x,float y,float z) {
    glm::vec3 xyz = glm::vec3(x,y,z);
    float o = clgridnoise(xyz/40)*8+clgridnoise(xyz/20)*4+clgridnoise(xyz/10)*2+clgridnoise(xyz/5)+y/5;
    return o;
}
#endif
void Sampler::populate(int xc,int yc,int zc,Structure& world) {
    float* results = new float[(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1)];
    // Write our data set into the input array in device memory
//    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * charn, data, 0, NULL, NULL);
//    if (err != CL_SUCCESS) throw;//Error: Failed to write to source array!
    // Set the arguments to our compute kernel
    
//    unsigned int temp = charn;
//    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input)
//         | clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
//         | clSetKernelArg(kernel, 2, sizeof(unsigned int), &temp);
    int xcc=xc*CHSIZE,ycc=yc*CHSIZE,zcc=zc*CHSIZE;
    ASSERT(clSetKernelArg(kernel, 0, sizeof(cl_mem), &output));
    ASSERT(clSetKernelArg(kernel, 1, sizeof(cl_mem), &xcc));
    ASSERT(clSetKernelArg(kernel, 2, sizeof(cl_mem), &ycc));
    ASSERT(clSetKernelArg(kernel, 3, sizeof(cl_mem), &zcc));
    size_t glo[3]{128,128,256};
    size_t loc[3]{8,8,8};
//    clock_t begin = clock();
    ASSERT(clEnqueueNDRangeKernel(gcl_commands,kernel, 3, NULL, glo,loc, 0, NULL, NULL));
    uint8_t* realindecies = new uint8_t[(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1)];
    
    clFinish(gcl_commands);
//    clock_t end = clock();
//    std::cout<<end-begin<<"\n";
    ASSERT(clEnqueueReadBuffer(gcl_commands,output,CL_TRUE,0,sizeof(float)*(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1),results,0,NULL,NULL));
    
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=1;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                float o1 = sample(xcc+xi,ycc+yi,zcc+zi);
//                float o2 = results[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)];
//                if ((o1<0 and o2>0) or (o1<0 and o2>0)) {
//                    throw;
//                }
//            }
//        }
//    }
    
    
    for (int i=0;i<(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1);i++) {
        realindecies[i] = results[i]>0?2:results[i]>-2?3:1;
    }
//    for (int i=0;i<(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1);i++) {
//        realindecies[i] = results[i]>0?2:1;
//    }
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=1;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                if (realindecies[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]==1 and realindecies[xi+(yi-1)*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]==2) {
//                    realindecies[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]=3;
//                }
//            }
//        }
//    }
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int zi=0;zi<CHSIZE;zi++) {
//            if (realindecies[xi+zi*(CHSIZE+1)*(CHSIZE+1)]==1 and sample(xi+xcc,ycc-1,zi+zcc)>0) {
//                std::cout<<"ththth\n";
//                realindecies[xi+zi*(CHSIZE+1)*(CHSIZE+1)]=3;
//            }
//        }
//    }
    OctreeSegment* fi = world.loadportion(xc,yc,zc,realindecies);
//    for (int xi=0;xi<CHSIZE;xi++) {
//        for (int yi=0;yi<CHSIZE;yi++) {
//            for (int zi=0;zi<CHSIZE;zi++) {
//                if (fi->getser(xi,yi,zi)!=realindecies[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]) throw;
//            }
//        }
//    }
    for (int xi=0;xi<CHSIZE;xi++) {
        for (int yi=0;yi<CHSIZE;yi++) {
            for (int zi=0;zi<CHSIZE;zi++) {
                BlockId ma = realindecies[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)];
                float a = results[xi+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)];
                if (ma!=realindecies[(xi+1)+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]) {
                    float b = results[(xi+1)+yi*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)];
//                    std::cout<<"xdif\n";
                    #ifdef PLOTTER_MODE
                    fi->xcon(xi,yi,zi) = Edgedat(gp.samplenormal(xc*CHSIZE+xi+a/(a-b),yc*CHSIZE+yi,zc*CHSIZE+zi),a/(a-b));
                    #else
                    fi->xcon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi+a/(a-b),yc*CHSIZE+yi,zc*CHSIZE+zi),a/(a-b));
//                    fi->xcon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi,yc*CHSIZE+yi,zc*CHSIZE+zi,a),a/(a-b));
                    #endif
                }
                if (ma!=realindecies[xi+(yi+1)*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)]) {
                    float b = results[xi+(yi+1)*(CHSIZE+1)+zi*(CHSIZE+1)*(CHSIZE+1)];
//                    std::cout<<"ydif\n";
                    #ifdef PLOTTER_MODE
                    fi->ycon(xi,yi,zi) = Edgedat(gp.samplenormal(xc*CHSIZE+xi,yc*CHSIZE+yi+a/(a-b),zc*CHSIZE+zi),a/(a-b));
                    #else
                    fi->ycon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi,yc*CHSIZE+yi+a/(a-b),zc*CHSIZE+zi),a/(a-b));
//                    fi->ycon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi,yc*CHSIZE+yi,zc*CHSIZE+zi,a),a/(a-b));
                    #endif
                }
                if (ma!=realindecies[xi+yi*(CHSIZE+1)+(zi+1)*(CHSIZE+1)*(CHSIZE+1)]) {
                    float b = results[xi+yi*(CHSIZE+1)+(zi+1)*(CHSIZE+1)*(CHSIZE+1)];
//                    std::cout<<"zdif\n";
                    #ifdef PLOTTER_MODE
                    fi->zcon(xi,yi,zi) = Edgedat(gp.samplenormal(xc*CHSIZE+xi,yc*CHSIZE+yi,zc*CHSIZE+zi+a/(a-b)),a/(a-b));
                    #else
                    fi->zcon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi,yc*CHSIZE+yi,zc*CHSIZE+zi+a/(a-b)),a/(a-b));
//                    fi->zcon(xi,yi,zi) = Edgedat(normal(xc*CHSIZE+xi,yc*CHSIZE+yi,zc*CHSIZE+zi,a),a/(a-b));
                    #endif
                }
            }
        }
    }
    
    delete[] results;
    delete[] realindecies;
}




void UNITTEST() {
    // Global OpenCL init
    int gpu = 1;
    ASSERT(clGetDeviceIDs(NULL, gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &gcl_device_id, NULL));
    ASSERT_ASSIGN(gcl_context,clCreateContext(0, 1, &gcl_device_id, NULL, NULL, &err));
    ASSERT_ASSIGN(gcl_commands,clCreateCommandQueue(gcl_context,gcl_device_id, 0, &err));

}



void initialize(){
    // Initialise GLFW
    if(!glfwInit()) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        throw;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Playground", NULL, NULL);
    if( window == NULL ) {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        throw;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        throw;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT_AND_BACK);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glLineWidth(3.0);
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    extern int numberofmaterials;
    extern ShaderTerrain* materials[];
    for (int k=0;k<numberofmaterials;k++) {
        if (materials[k]!=NULL) {
            materials[k]->mountshaders();
        }
    }
//    extern ShaderTerrain* debugshader;
//    debugshader->mountshaders();
    
}
void getclippy(glm::mat4 m) {
    extern glm::vec3 clipplanes[];
    extern float clipdistances[];
    const int FrustumPlane_Right  = 0;
    const int FrustumPlane_Left   = 1;
    const int FrustumPlane_Top    = 2;
    const int FrustumPlane_Bottom = 3;
    const int FrustumPlane_Near   = 4;
    const int FrustumPlane_Far    = 5;
    
    clipplanes[FrustumPlane_Right].x  = m[0][3] + m[0][0];
    clipplanes[FrustumPlane_Right].y  = m[1][3] + m[1][0];
    clipplanes[FrustumPlane_Right].z  = m[2][3] + m[2][0];
    clipdistances[FrustumPlane_Right] = m[3][3] + m[3][0];

    clipplanes[FrustumPlane_Left].x  = m[0][3] - m[0][0];
    clipplanes[FrustumPlane_Left].y  = m[1][3] - m[1][0];
    clipplanes[FrustumPlane_Left].z  = m[2][3] - m[2][0];
    clipdistances[FrustumPlane_Left] = m[3][3] - m[3][0];

    clipplanes[FrustumPlane_Top].x  = m[0][3] - m[0][1];
    clipplanes[FrustumPlane_Top].y  = m[1][3] - m[1][1];
    clipplanes[FrustumPlane_Top].z  = m[2][3] - m[2][1];
    clipdistances[FrustumPlane_Top] = m[3][3] - m[3][1];

    clipplanes[FrustumPlane_Bottom].x  = m[0][3] + m[0][1];
    clipplanes[FrustumPlane_Bottom].y  = m[1][3] + m[1][1];
    clipplanes[FrustumPlane_Bottom].z  = m[2][3] + m[2][1];
    clipdistances[FrustumPlane_Bottom] = m[3][3] + m[3][1];

    clipplanes[FrustumPlane_Far].x  = m[0][2];
    clipplanes[FrustumPlane_Far].y  = m[1][2];
    clipplanes[FrustumPlane_Far].z  = m[2][2];
    clipdistances[FrustumPlane_Far] = m[3][2];

    clipplanes[FrustumPlane_Near].x  = m[0][3] - m[0][2];
    clipplanes[FrustumPlane_Near].y  = m[1][3] - m[1][2];
    clipplanes[FrustumPlane_Near].z  = m[2][3] - m[2][2];
    clipdistances[FrustumPlane_Near] = m[3][3] - m[3][2];
    
    for (int k=0;k<6;k++) {
        double dist = glm::length(clipplanes[k]);
        clipdistances[k] = clipdistances[k]/dist;
        clipplanes[k] = glm::normalize(clipplanes[k]);
    }
}

int main() {
    UNITTEST();
//    return 0;
    initialize();
    
    
    
    glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);

//    glm::mat4 ViewRot       = glm::lookAt(
//                                       glm::vec3(0,0,0), // Camera is at (4,3,3), in World Space
//                                       glm::vec3(-4,-3,-3), // and looks at the origin
//                                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
//                                       );
//    glm::mat4 ViewTranslate = glm::translate(glm::mat4(), glm::vec3(4,3,3));
    
    glm::mat4 View       = glm::lookAt(
                                          glm::vec3(8,6,-6), // Camera is at (4,3,3), in World Space
                                          glm::vec3(0,0,0), // and looks at the origin
                                          glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                          );
    
//    getclippy(Projection);
    
//    testgeom.bake();
    
//    for (int toload = 0;toload<200;toload++) {
//        structure.load(structure.queue[structure.queue.size()-toload]);
//    }
    
//    Environment envi;
    
    double x = 8;
    double y = 6;
    double z = 6;
    double theta = 0;
    
//    float meteorangle = 0;
    
    
    
    double lastTime = glfwGetTime();
    double lastFrameTime = lastTime;
    int nbFrames = 0;
    
    
    Environment world;
    Structure* temppoint = new Structure("newformat",world,true);
    temppoint->source = new Sampler();//new SimpleTerrainSample();
    world.addstructure(temppoint);
    world.opensavedirectory();
    world.beginthread();
    
    

    
    extern glm::mat4 camera;
    extern glm::mat4 clipcamera;
    
    bool sharp = false;
    
    int multip = 1;
    do{
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        double currentTime = glfwGetTime();
        lastFrameTime = currentTime;
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ) {
            std::cout<<nbFrames<<" frames per second\n";
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        View       = glm::lookAt(
                                   glm::vec3(x,y,z), // Camera is at (4,3,3), in World Space
                                   glm::vec3(x+cos(theta)*3,y-1,z+sin(theta)*3), // and looks at the origin
                                   glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
        
        camera = Projection*View;
        getclippy(camera);
//        clipcamera = View;
        
        
        
        world.checkup(x,y,z);
        
        world.draw();
        
        
        
        
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        if (glfwGetKey(window,GLFW_KEY_P) == GLFW_PRESS) {
            //            temppoint->world.realworld.data->debugprinttrace(0);
//            overover = !overover;
//            extern bool quaqua;
//            quaqua=false;
            if (!sharp) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            }
            sharp = true;
        } else {
            if (sharp) {
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            }
            sharp = false;
//            quaqua=true;
//            glPolygonMode( GL_FRONT, GL_FILL );
        }
        
        
        if (glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS) {
            //            temppoint->world.realworld.data->debugprinttrace(0);
            multip = 3;
        } else {
            multip = 1;
        }
        
        if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS) {
//            glm::mat4 move = glm::rotate(glm::mat4(1.0f), .075f, glm::vec3(0.0f, 1.0f, 0.0f));
//            View = move * View;
            theta -= 0.05f;
        } else if (glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS) {
//            glm::mat4 move = glm::rotate(glm::mat4(1.0f), -.075f, glm::vec3(0.0f, 1.0f, 0.0f));
//            View = move * View;
            theta += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,0,0.1f));
//            View = move * View;
            x += cos(theta)*0.3f*multip;
            z += sin(theta)*0.3f*multip;
        } else if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,0,-0.1f));
            //            View = move * View;
            x -= cos(theta)*0.3f*multip;
            z -= sin(theta)*0.3f*multip;
        }
        if (glfwGetKey(window, GLFW_KEY_Q ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,0.1f,0));
//            View = View * move;
            y += .5f*multip;
        } else if (glfwGetKey(window, GLFW_KEY_E ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,-0.1f,0));
//            View = View * move;
            y -= .5f*multip;
        }
        
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
//    shader.cleanup();
    //envi.cleanup();
    
    cleanup();
    
    
    glfwTerminate();
//    clFinish();
    clFinish(gcl_commands);
    clReleaseCommandQueue(gcl_commands);
    clReleaseContext(gcl_context);
    return 0;
}















