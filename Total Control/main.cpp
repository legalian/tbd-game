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
//#include "qef.h"

//#include <btBulletDynamicsCommon.h>


//template<int& T>
//int grab() {
//    return T;
//}
//int locloc = 453;
//
//
////change
//void UNITTEST() {
//    std::cout<<grab<locloc>()<<"\n";
//}



void initialize(){
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        throw;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    
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
    glEnable(GL_CULL_FACE);
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

int main()
{
//    UNITTEST();
//    return 0;
    initialize();
//    test_qef();
    
//    ShaderVNC shader;
//    shader.mountshaders();
    
    
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
//    std::cout<<structure.geoms.size();
    
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
    temppoint->source = new SimpleTerrainSample();
    world.addstructure(temppoint);
    world.opensavedirectory();
    world.beginthread();
    
//    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    
    
//    Structure test = Structure("test");
//    test.source = new SimpleTerrainSample();
//    test.source = new SimpleTerrainSample();
//    test.attain(Location(0,-1,0));
//    test.attain(Location(0,0,0));
//    for (int f=0;f<2;f++) {
//        test.updatequeue(3,3,3);
//        std::cout<<test.queue.size()<<" is len\n";
//        for (int index=0;index<test.queue.size();index++) {
//            test.attain(test.queue[index]);
//        }
//        test.queue.clear();
//        
//    }
//    test_qef();
    
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
                glPolygonMode( GL_FRONT, GL_FILL );
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
    return 0;
}















