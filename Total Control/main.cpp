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

#include </usr/include/GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


#include "environment.h"
#include "renderterrain.h"
#include "octree.h"
#include "geometryoctree.h"
#include "materials.h"

#include <pthread.h>


using namespace glm;

//change

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
    if( window == NULL ){
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
//    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    extern int numberofmaterials;
    extern ShaderTerrain* materials[];
    for (int k=0;k<numberofmaterials;k++) {
        if (materials[k]!=NULL) {
            materials[k]->mountshaders();
        }
    }
    
}

int main()
{
    initialize();
    
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
    
    
//    testgeom.bake();
    
//    for (int toload = 0;toload<200;toload++) {
//        structure.load(structure.queue[structure.queue.size()-toload]);
//    }
//    std::cout<<structure.geoms.size();
    
//    Environment envi;
    
    double x = 8;
    double y = 6;
    double z = -6;
    double theta = 0;
    
    float meteorangle = 0;
    
    
    
    double lastTime = glfwGetTime();
    double lastFrameTime = lastTime;
    int nbFrames = 0;
    
    Structure test;
    test.source = new BoxSample();
//    test.attain(Location(0,-1,0));
//    test.attain(Location(0,0,0));
    for (int f=0;f<2;f++) {
        test.updatequeue(3,3,3);
        std::cout<<test.queue.size()<<" is len\n";
        for (int index=0;index<test.queue.size();index++) {
            test.attain(test.queue[index]);
        }
        test.queue.clear();
        
    }
    
    extern glm::mat4 camera;
    
    do{
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        double currentTime = glfwGetTime();
        lastFrameTime = currentTime;
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){
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
        
        test.render();
        renderall();
//        std::cout<<x<<","<<y<<","<<z<<"\n";
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
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
            x += cos(theta)*0.3f;
            z += sin(theta)*0.3f;
        } else if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,0,-0.1f));
            //            View = move * View;
            x -= cos(theta)*0.3f;
            z -= sin(theta)*0.3f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,0.1f,0));
//            View = View * move;
            y += .5f;
        } else if (glfwGetKey(window, GLFW_KEY_E ) == GLFW_PRESS) {
//            glm::mat4 move = glm::translate(glm::mat4(), glm::vec3(0,-0.1f,0));
//            View = View * move;
            y -= .5f;
        }
        
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
//    shader.cleanup();
    //envi.cleanup();
    
    cleanup();
    
    
    glfwTerminate();
    return 0;
}















