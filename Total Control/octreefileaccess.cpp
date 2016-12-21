//
//  octreefileaccess.cpp
//  Total Control
//
//  Created by Parker on 12/19/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"


void OctreeSegment::filesave(std::ostream& file) {throw;}
void OctreeSegment::worldfilesave(std::ostream& file,int) {throw;}



void OctreeFeature::filesave(std::ostream& file) {
    char towrite = 'f';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
    file.write((char*)&conflag, sizeof(uint8_t));
}

void OctreeLeaf::filesave(std::ostream& file) {
    char towrite = 'l';
    file.write(&towrite,1);
    file.write((char*)&fillvalue,sizeof(uint8_t));
    file.write((char*)&conflag, sizeof(uint8_t));
    file.write((char*)&xcondat.t,sizeof(uint8_t));
    file.write((char*)&xcondat.x,sizeof(int8_t));
    file.write((char*)&xcondat.y,sizeof(int8_t));
    file.write((char*)&xcondat.z,sizeof(int8_t));
    file.write((char*)&ycondat.t,sizeof(uint8_t));
    file.write((char*)&ycondat.x,sizeof(int8_t));
    file.write((char*)&ycondat.y,sizeof(int8_t));
    file.write((char*)&ycondat.z,sizeof(int8_t));
    file.write((char*)&zcondat.t,sizeof(uint8_t));
    file.write((char*)&zcondat.x,sizeof(int8_t));
    file.write((char*)&zcondat.y,sizeof(int8_t));
    file.write((char*)&zcondat.z,sizeof(int8_t));
}

void OctreeBud::filesave(std::ostream& file) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}
void OctreeBud::worldfilesave(std::ostream& file,int scale) {
    char towrite = 's';
    file.write(&towrite,1);
    file.write((char*)&fillvalue, sizeof(BlockId));
}

void OctreeBranch::filesave(std::ostream& file) {
    char towrite = 'c';
    file.write(&towrite,1);
    subdivisions[0][0][0]->filesave(file);
    subdivisions[1][0][0]->filesave(file);
    subdivisions[0][1][0]->filesave(file);
    subdivisions[1][1][0]->filesave(file);
    subdivisions[0][0][1]->filesave(file);
    subdivisions[1][0][1]->filesave(file);
    subdivisions[0][1][1]->filesave(file);
    subdivisions[1][1][1]->filesave(file);
}
void OctreeBranch::worldfilesave(std::ostream& file,int scale) {
    char towrite = 'c';
    file.write(&towrite,1);
    if (scale<=MAX_WORLDFILE_GEOMSAVE) {
        file.write((char*)&point.x, sizeof(uint8_t));
        file.write((char*)&point.y, sizeof(uint8_t));
        file.write((char*)&point.z, sizeof(uint8_t));
        if (scale==MIN_WORLDFILE_GEOMSAVE) {
            uint8_t getmeh = getser(0,0,0);
            file.write((char*)&connections, sizeof(uint8_t));
            file.write((char*)&getmeh, sizeof(uint8_t));
        }
    }
    if (scale>MIN_WORLDFILE_GEOMSAVE) {
        subdivisions[0][0][0]->worldfilesave(file,scale-1);
        subdivisions[1][0][0]->worldfilesave(file,scale-1);
        subdivisions[0][1][0]->worldfilesave(file,scale-1);
        subdivisions[1][1][0]->worldfilesave(file,scale-1);
        subdivisions[0][0][1]->worldfilesave(file,scale-1);
        subdivisions[1][0][1]->worldfilesave(file,scale-1);
        subdivisions[0][1][1]->worldfilesave(file,scale-1);
        subdivisions[1][1][1]->worldfilesave(file,scale-1);
    }
}

void Octree::filepushportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    std::ofstream file = std::ofstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::out|std::ios::binary|std::ios::trunc);
    OctreePortionAwareBranch* look = data->getvoxunit(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z));
    if (look!=NULL) {
        look->filesave(file);
    } else {
        OctreeBud(data->getser(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z))).filesave(file);
    }
    file.close();
}
void Octree::filepullportion(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    //    file =
    std::ifstream file = std::ifstream(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)),std::ios::in|std::ios::binary);
    std::cout<<filebase<<" was opened. \n";
    expandchunk(x,y,z);
    data->insertinto(ASCHUNKLOC(x),ASCHUNKLOC(y),ASCHUNKLOC(z),CHPOWER,depth,makeOctree(file,CHPOWER),data);
    file.close();
}
bool Octree::dataexists(std::string filebase,BlockLoc x,BlockLoc y,BlockLoc z) {
    return boost::filesystem::exists(filebase+"/"+(std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)));
}

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
        if (recur == 6) {
            return new OctreePortionAwareBranch(makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),
                                                makeOctree(file,recur),recur,true);
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

OctreeSegment* loadWorldFile(std::ifstream& file,int recur) {
    char tester;
    file.read(&tester,1);
    if (tester == 's') {
        uint8_t id;
        file.read((char*) &id,sizeof(uint8_t));
        return new OctreeBud(id);
    } else if (tester == 'c') {
        recur--;
        if (recur<=MAX_WORLDFILE_GEOMSAVE-1) {
            uint8_t x;
            uint8_t y;
            uint8_t z;
            file.read((char*) &x,sizeof(uint8_t));
            file.read((char*) &y,sizeof(uint8_t));
            file.read((char*) &z,sizeof(uint8_t));
            if (recur > MIN_WORLDFILE_GEOMSAVE-1) {
                if (recur == CHPOWER-1) {
                    return new OctreePortionAwareBranch(loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),
                                                        loadWorldFile(file,recur),recur,x,y,x);
                } else {
                    return new OctreeBranch(loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),
                                            loadWorldFile(file,recur),recur,x,y,x);
                }
            } else {
                uint8_t conn;
                uint8_t id;
                file.read((char*) &id,sizeof(uint8_t));
                file.read((char*) &conn,sizeof(uint8_t));
                if (recur == CHPOWER-1) {
                    return new OctreePortionAwareBranch(new OctreeBud(id),recur,x,y,x,conn);
                } else {
                    return new OctreeBranch(new OctreeBud(id),recur,x,y,x,conn);
                }
            }
        } else {
            if (recur == CHPOWER-1) {
                return new OctreePortionAwareBranch(loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),
                                                    loadWorldFile(file,recur),recur,false);
            } else {
                return new OctreeBranch(loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),
                                        loadWorldFile(file,recur),recur);
            }
        }
    } else {
        std::cout<<tester<<"\n";
        throw;
    }
}





