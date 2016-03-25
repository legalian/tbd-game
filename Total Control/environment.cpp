//
//  environment.cpp
//  Total Control
//
//  Created by Parker Lawrence on 1/9/16.
//  Copyright (c) 2016 Parker Lawrence. All rights reserved.
//

#include "environment.h"


//
//Environment::Environment() {
////    Structure k;
////    structures.push_back(k);
//    gen.pool = &structures;
//    gen.firstdump();
//    pthread_create(&loadingthread , NULL, loaderthread, (void*)this);
////    pthread_t loadingthread2;
////    pthread_create(&loadingthread2 , NULL, loaderthread, (void*)this);
//}
//Structure* Environment::getStruct(std::string targetid) {
//    for (int istr = 0;istr<structures.size();istr++) {
//        if (structures[istr].structureid == targetid) {
//            return &structures[istr];
//        }
//    }
//    throw;
//}
//
//void Environment::loadnextchunk() {
////    if (testerbool) {
//    if (structures.size()>0) {
//        
//    //    Location* next = loadqueue.back().first;
//    //    Structure* dest = loadqueue.back().second;
//    //    loadqueue.pop_back();
//    //    if (!dest->existsAt(next->x,next->y,next->z)) {
//    //        dest->load(*next);
//    //    }
//        
//        
//        
//        for (int k=0;k<structures.size();k++) {
//            glm::vec4 test = glm::inverse(structures[k].transform)*view;
//            int test_x = TRUNC_DIV((int)(floor(test.x)),CHSIZE);
//            int test_y = TRUNC_DIV((int)(floor(test.y)),CHSIZE);
//            int test_z = TRUNC_DIV((int)(floor(test.z)),CHSIZE);
//            if (!(structures[k].epicenter == Location(test_x,test_y,test_z))) {
////                std::cout<<Location(test_x,test_y,test_z).tostring()<<"chosen for: "<<structures[k].structureid<<" and "<<Location(test_x,test_y,test_z).tostring()<<"\n";
//                structures[k].epicenter = Location(test_x,test_y,test_z);
//                structures[k].loadstage = 0;
//                structures[k].refreshqueue();
//            }
//            else if (structures[k].queue.size() == 0) {
//                structures[k].refreshqueue();
//            }
//        }
////        std::cout<<structures[0].loadstage<<"\n";
////        std::cout<<structures[0].queue.back().x;
//        Structure* min = &(structures[0]);
//        for (int k=1;k<structures.size();k++) {
//            if (structures[k].loadstage<(min->loadstage)) {
//                min = &(structures[k]);
//            }
//            if (structures[k].loadstage==(min->loadstage) && structures[k].queue.size()>(min->queue.size())) {
//                min = &(structures[k]);
//            }
//        }
////        min->sortqueue(view/((float)CHSIZE));
//        glm::vec4 thiscoor = (glm::inverse(min->transform)*view)/((float)CHSIZE);
////        std::cout<<min->structureid<<" summoned: "<<thiscoor.x<<","<<thiscoor.y<<","<<thiscoor.z<<"\n";
////        std::cout<<glm::to_string(view)<<"\n";
//        int loadind = 0;
//        double lastcomp = 9000000;
//        for (int k=0;k<min->queue.size();k++) {
//            double xdif = ((min->queue[k].x+.5)-thiscoor.x);
//            double ydif = ((min->queue[k].y+.5)-thiscoor.y);
//            double zdif = ((min->queue[k].z+.5)-thiscoor.z);
//            double thiscomp = xdif*xdif+ydif*ydif+zdif*zdif;
//            if (thiscomp<lastcomp) {
//                loadind = k;
//                lastcomp = thiscomp;
//            }
//        }
//        
//        
//        
////        Location toload = min->queue.back();
//        min->load(min->queue[loadind],&gen);
////        std::cout<<"loaded "<<min->structureid<<": "<<min->queue[loadind].tostring()<<"\n";
//        min->queue.erase(min->queue.begin() + loadind);
//        //gen.terrain_update(min,toload);
////        min->queue.pop_back();
//    }
////        testerbool = false;
////    }
//}
//
//
//void Environment::draw(ShaderVNC* shader) {
//    glm::vec4 thisvert;
////    bool hitplanes[6] = {false,false,false,false,false,false};
//    for (int k=0;k<structures.size();k++) {
//        for (std::map<Location,GeomTerrain>::iterator it = structures[k].geoms.begin();it!=structures[k].geoms.end();it++) {
////            std::cout<<"trying";
//            bool hitplanes[6] = {false,false,false,false,false,false};
//            for (int iv=0;iv<8;iv++) {
//                thisvert = glm::vec4((it->first.x+(iv&1))*CHSIZE,
//                (it->first.y+((iv&2)>>1))*CHSIZE,
//                (it->first.z+((iv&4)>>2))*CHSIZE,1);
//                thisvert = (shader->cam)*(*(it->second.matrix))*thisvert;
//                
//                if (thisvert.x/thisvert.w<1) hitplanes[0]=true;
//                if (thisvert.x/thisvert.w>-1) hitplanes[1]=true;
//                if (thisvert.y/thisvert.w<1) hitplanes[2]=true;
//                if (thisvert.y/thisvert.w>-1) hitplanes[3]=true;
//                if (thisvert.z/thisvert.w<1) hitplanes[4]=true;
//                if (thisvert.z/thisvert.w>-1) hitplanes[5]=true;
//            }
//            
//            
////            (shader->cam)*(*it->second.matrix)*
//            
//            
//            //it->first.x*CHSIZE,(it->first.x+1)*CHSIZE
//            //it->first.y*CHSIZE,(it->first.y+1)*CHSIZE
//            //it->first.z*CHSIZE,(it->first.z+1)*CHSIZE
//            if (hitplanes[0] and hitplanes[1] and hitplanes[2] and hitplanes[3] and hitplanes[4] and hitplanes[5]) {
//                shader->draw(&(it->second));
//            }
//        }
//    }
////    std::cout<<"\n";
//}
//
//void Environment::cluein(double vx,double vy, double vz) {
//    view = glm::vec4(vx,vy,vz,1);
//}
//void Environment::checkup() {
////    if (!testerbool) {
//    for (int k=0;k<structures.size();k++) {
//        for (std::map<Location,GeomTerrain>::iterator it = structures[k].geoms.begin();it!=structures[k].geoms.end();it++) {
//            if (it->second.changed) {
//                it->second.changed = false;
//                if (it->second.baked) {
//                    it->second.unbake();
//                }
//                it->second.bake();
//                it->second.baked = true;
//            }
//        }
//    }
////        testerbool = true;
////    }
//}
//void Environment::cleanup() {
//    keepexecution = false;
//    pthread_join(loadingthread,NULL);
//}
//
//void* loaderthread(void* me) {
//    Environment* dest = (Environment*)me;
//    while (dest->keepexecution) {
//        dest->loadnextchunk();
////        break;
//    }
//    return NULL;
//}
//






