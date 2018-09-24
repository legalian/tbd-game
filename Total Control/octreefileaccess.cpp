//
//  octreefileaccess.cpp
//  Total Control
//
//  Created by Parker on 12/19/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"


void OctreeSegment::filesave() {throw;}
void OctreeSegment::worldfilesave(int x,int y,int z) {throw;}



void OctreeFeature::filesave() {
    char towrite = 'f';
    s_file->write(&towrite,1);
    s_file->write((char*)&fillvalue, sizeof(BlockId));
    s_file->write((char*)&conflag, sizeof(uint8_t));
}

void OctreeLeaf::filesave() {
    char towrite = 'l';
    s_file->write(&towrite,1);
    s_file->write((char*)&fillvalue,sizeof(uint8_t));
    s_file->write((char*)&conflag, sizeof(uint8_t));
    s_file->write((char*)&xcondat.t,sizeof(uint8_t));
    s_file->write((char*)&xcondat.x,sizeof(int8_t));
    s_file->write((char*)&xcondat.y,sizeof(int8_t));
    s_file->write((char*)&xcondat.z,sizeof(int8_t));
    s_file->write((char*)&ycondat.t,sizeof(uint8_t));
    s_file->write((char*)&ycondat.x,sizeof(int8_t));
    s_file->write((char*)&ycondat.y,sizeof(int8_t));
    s_file->write((char*)&ycondat.z,sizeof(int8_t));
    s_file->write((char*)&zcondat.t,sizeof(uint8_t));
    s_file->write((char*)&zcondat.x,sizeof(int8_t));
    s_file->write((char*)&zcondat.y,sizeof(int8_t));
    s_file->write((char*)&zcondat.z,sizeof(int8_t));
}
void OctreeBud::filesave() {
    char towrite = 's';
    s_file->write(&towrite,1);
    s_file->write((char*)&fillvalue, sizeof(BlockId));
}
void OctreeBud::worldfilesave(int x,int y,int z) {
    char towrite = 's';
    s_file->write(&towrite,1);
    s_file->write((char*)&fillvalue, sizeof(BlockId));
}
void OctreeBranch::filesave() {
    char towrite = 'c';
    s_file->write(&towrite,1);
    subdivisions[0][0][0]->filesave();
    subdivisions[1][0][0]->filesave();
    subdivisions[0][1][0]->filesave();
    subdivisions[1][1][0]->filesave();
    subdivisions[0][0][1]->filesave();
    subdivisions[1][0][1]->filesave();
    subdivisions[0][1][1]->filesave();
    subdivisions[1][1][1]->filesave();
}
void OctreePortionAwareBranch::worldfilesave(int x,int y,int z) {
    g_vertecies = vertecies;
    g_normals = normals;
    OctreeBranch::worldfilesave(x,y,z);
}
void OctreeBranch::worldfilesave(int x,int y,int z) {
    int mask = 1<<depth;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    char towrite = 'c';
    s_file->write(&towrite,1);
    if (depth<MAX_WORLDFILE_GEOMSAVE) {
        uint8_t xpos,ypos,zpos;
        int8_t xnor,ynor,znor;
        if (point!=-1) {
            xpos = savevoxvert(x,g_vertecies[depth+1][point].x,depth+1);
            ypos = savevoxvert(y,g_vertecies[depth+1][point].y,depth+1);
            zpos = savevoxvert(z,g_vertecies[depth+1][point].z,depth+1);
            xnor = savevoxnorm(g_normals[depth+1][point].x);
            ynor = savevoxnorm(g_normals[depth+1][point].y);
            znor = savevoxnorm(g_normals[depth+1][point].z);
        }
        s_file->write((char*)&xpos, sizeof(uint8_t));
        s_file->write((char*)&ypos, sizeof(uint8_t));
        s_file->write((char*)&zpos, sizeof(uint8_t));
        s_file->write((char*)&xnor, sizeof(int8_t));
        s_file->write((char*)&ynor, sizeof(int8_t));
        s_file->write((char*)&znor, sizeof(int8_t));
        if (depth+1==MIN_WORLDFILE_GEOMSAVE) {
            uint8_t getmeh = getser(0,0,0);
            s_file->write((char*)&connections, sizeof(uint8_t));
            s_file->write((char*)&getmeh, sizeof(uint8_t));
        }
    }
    if (depth>=MIN_WORLDFILE_GEOMSAVE) {
        subdivisions[0][0][0]->worldfilesave(x     ,y     ,z     );
        subdivisions[1][0][0]->worldfilesave(x|mask,y     ,z     );
        subdivisions[0][1][0]->worldfilesave(x     ,y|mask,z     );
        subdivisions[1][1][0]->worldfilesave(x|mask,y|mask,z     );
        subdivisions[0][0][1]->worldfilesave(x     ,y     ,z|mask);
        subdivisions[1][0][1]->worldfilesave(x|mask,y     ,z|mask);
        subdivisions[0][1][1]->worldfilesave(x     ,y|mask,z|mask);
        subdivisions[1][1][1]->worldfilesave(x|mask,y|mask,z|mask);
    }
}


//bool Octree::dataexists(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
//    return boost::filesystem::exists(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)));
//}

OctreeSegment* makeOctree(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 'l') {
        uint8_t id;
        uint8_t conflag;
        int8_t ids[12];
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &conflag,sizeof(uint8_t));
        file.read((char*) &ids,sizeof(int8_t)*12);
        return new OctreeLeaf(id,conflag,ids);
    } else if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'f') {
        uint8_t id;
        uint8_t conflag;
        file.read((char*) &id,sizeof(uint8_t));
        file.read((char*) &conflag,sizeof(uint8_t));
        return new OctreeFeature(id,conflag);
    } else if (tester == 'c') {
        recur--;
        if (recur == CHPOWER-1) {
            g_vertecies = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
            g_normals = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
            OctreePortionAwareBranch* opab =
                new OctreePortionAwareBranch(makeOctree(file,recur),//IT"S THE THREAD
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),
                                             makeOctree(file,recur),true);
            opab->vertecies = g_vertecies;
            opab->normals = g_normals;
            return opab;
//            return new OctreePortionAwareBranch(makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),
//                                             makeOctree(file,recur),true);
        } else {
            return new OctreeBranch(makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),
                                    makeOctree(file,recur),recur);
        }
    } else {
        throw;
    }
}

OctreeSegment* loadWorldFile(std::ifstream& file,int x,int y,int z,int recur) {
    int mask = 1<<recur;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    char tester;
    file.read(&tester,1);
    if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'c') {
        recur--;
        if (recur<MAX_WORLDFILE_GEOMSAVE) {
            uint8_t xu,yu,zu;
            int8_t xv,yv,zv;
            file.read((char*) &xu,sizeof(uint8_t));
            file.read((char*) &yu,sizeof(uint8_t));
            file.read((char*) &zu,sizeof(uint8_t));
            file.read((char*) &xv,sizeof(int8_t));
            file.read((char*) &yv,sizeof(int8_t));
            file.read((char*) &zv,sizeof(int8_t));
            if (recur > MIN_WORLDFILE_GEOMSAVE-1) {
                if (recur == CHPOWER-1) {
                    g_vertecies = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                    g_normals = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                    OctreePortionAwareBranch* opab =
                        new OctreePortionAwareBranch(loadWorldFile(file,x     ,y     ,z     ,recur),
                                                     loadWorldFile(file,x|mask,y     ,z     ,recur),
                                                     loadWorldFile(file,x     ,y|mask,z     ,recur),
                                                     loadWorldFile(file,x|mask,y|mask,z     ,recur),
                                                     loadWorldFile(file,x     ,y     ,z|mask,recur),
                                                     loadWorldFile(file,x|mask,y     ,z|mask,recur),
                                                     loadWorldFile(file,x     ,y|mask,z|mask,recur),
                                                     loadWorldFile(file,x|mask,y|mask,z|mask,recur),false,readvoxvert(x,y,z,xu,yu,zu,CHPOWER),readvoxnorm(xv,yv,zv));
                    opab->vertecies = g_vertecies;
                    opab->normals = g_normals;
                    return opab;
                } else {
                    return new OctreeBranch(loadWorldFile(file,x     ,y     ,z     ,recur),
                                            loadWorldFile(file,x|mask,y     ,z     ,recur),
                                            loadWorldFile(file,x     ,y|mask,z     ,recur),
                                            loadWorldFile(file,x|mask,y|mask,z     ,recur),
                                            loadWorldFile(file,x     ,y     ,z|mask,recur),
                                            loadWorldFile(file,x|mask,y     ,z|mask,recur),
                                            loadWorldFile(file,x     ,y|mask,z|mask,recur),
                                            loadWorldFile(file,x|mask,y|mask,z|mask,recur),recur,readvoxvert(x,y,z,xu,yu,zu,recur+1),readvoxnorm(xv,yv,zv));
                }
            } else {
                uint8_t conn;
                uint8_t id;
                file.read((char*) &conn,sizeof(uint8_t));
                file.read((char*) &id,sizeof(uint8_t));
                if (recur == CHPOWER-1) {
                    return new OctreePortionAwareBranch(id,readvoxvert(x,y,z,xu,yu,zu,CHPOWER),readvoxnorm(xv,yv,zv),conn);
                } else {
                    return new OctreeBranch(id,recur,readvoxvert(x,y,z,xu,yu,zu,recur+1),readvoxnorm(xv,yv,zv),conn);
                }
            }
        } else {
            if (recur == CHPOWER-1) {
                g_vertecies = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                g_normals = new std::vector<glm::vec3>[MAX_WORLDFILE_GEOMSAVE+1];
                OctreePortionAwareBranch* opab =
                    new OctreePortionAwareBranch(loadWorldFile(file,x     ,y     ,z     ,recur),
                                                 loadWorldFile(file,x|mask,y     ,z     ,recur),
                                                 loadWorldFile(file,x     ,y|mask,z     ,recur),
                                                 loadWorldFile(file,x|mask,y|mask,z     ,recur),
                                                 loadWorldFile(file,x     ,y     ,z|mask,recur),
                                                 loadWorldFile(file,x|mask,y     ,z|mask,recur),
                                                 loadWorldFile(file,x     ,y|mask,z|mask,recur),
                                                 loadWorldFile(file,x|mask,y|mask,z|mask,recur),false);
                opab->vertecies = g_vertecies;
                opab->normals = g_normals;
                return opab;
            } else {
                return new OctreeBranch(loadWorldFile(file,x     ,y     ,z     ,recur),
                                        loadWorldFile(file,x|mask,y     ,z     ,recur),
                                        loadWorldFile(file,x     ,y|mask,z     ,recur),
                                        loadWorldFile(file,x|mask,y|mask,z     ,recur),
                                        loadWorldFile(file,x     ,y     ,z|mask,recur),
                                        loadWorldFile(file,x|mask,y     ,z|mask,recur),
                                        loadWorldFile(file,x     ,y|mask,z|mask,recur),
                                        loadWorldFile(file,x|mask,y|mask,z|mask,recur),recur);
            }
        }
    } else {
        std::cout<<tester<<"\n";
        throw;
    }
}





