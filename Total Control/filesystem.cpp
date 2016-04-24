//////////
//////////  filesystem.cpp
//////////  Total Control
//////////
//////////  Created by Parker on 4/6/16.
//////////  Copyright © 2016 Parker Lawrence. All rights reserved.
//////////
////////
//#include "filesystem.h"
////////
////////
////////
//void Filesystem::linkenvtofile(Environment* world,std::string file) {
//    filename = file;
//    worldlink = world;
//}
//void Filesystem::createnewlink(Environment* world,std::string file) {
//    boost::filesystem::path dir("file");
//    if (!boost::filesystem::create_directory(dir)) throw;
//    filename = file;
//    worldlink = world;
//}
//void Filesystem::openstructureids() {
//    
//}
////
////void Filesystem::linktofile(std::string file) {
////    filename = file;
////    generatejumpmap();
////}
////void Filesystem::pulloctreelevel(std::ifstream& file,long begin,int recursleft,OctreeSegment*& target,BlockLoc x,BlockLoc y,BlockLoc z) {
////    file.seekg(begin*sizeof(int)*8);
////    int offsets[8];
////    file.read((char*) &offsets[0],sizeof(int));
////    if (offsets[0] == 0) {
////        uint8_t id;
////        file.read((char*) &id,sizeof(uint8_t));
////        target = new OctreeBud(id);
////    } else {
////        if (recursleft == CHPOWER-1) {
////            std::ifstream bfile = std::ifstream(filename+"_body",std::ios::in|std::ios::binary);
////            long offset;
////            file.read((char*) &offset,sizeof(long));
////            bfile.seekg(offset);
////            target = readoctree(bfile,CHPOWER-1);
////            bfile.close();
////        } else {
////            file.read((char*) &offsets[1],sizeof(int)*7);
////            long ko = offsets[ (((x&(1<<recursleft))>>recursleft)<<2)
////                              |(((y&(1<<recursleft))>>recursleft)<<1)
////                              | ((z&(1<<recursleft))>>recursleft) ];
////            pulloctreelevel(file,ko,recursleft-1,target,x,y,z);
////        }
////    }
////}
//////OctreeSegment* Filesystem::readoctree(std::ifstream& file,int dep) {
//////    char tester;
//////    file.read(&tester,1);
//////    if (tester == 'l') {
//////        uint8_t id;
//////        uint8_t ids[12];
//////        file.read((char*) &id,sizeof(uint8_t));
//////        file.read((char*) &ids,sizeof(uint8_t)*12);
//////        return new OctreeLeaf(id,ids);
//////    } else if (tester == 's') {
//////        uint8_t id;
//////        file.read((char*) &id,sizeof(uint8_t));
//////        return new OctreeBud(id);
//////    } else if (tester == 'c') {
//////        return new OctreeBranch(readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),
//////                            readoctree(file,dep-1),dep);
//////    } else {
//////        throw;
//////    }
//////}
//////void Filesystem::createnewlink(std::string) {
//////    
//////}
//////void Filesystem::pushoctreelevel(std::string structureid,OctreeSegment*& target,BlockLoc x,BlockLoc y,BlockLoc z) {
//////    std::fstream file = std::fstream(filename,std::ios::in|std::ios::out|std::ios::binary);
//////    if (jumpmap.count(structureid)==0) {
//////        file.seekg(0, std::ios::end);
//////        long newbegin = ((long)file.tellg())-headoffset;
//////        jumpmap[structureid] = std::pair<long,int>(newbegin,0);
//////        
//////        file.seekp(0);
//////        file.seekg(0);
//////    }
//////}
////void Filesystem::generatejumpmap() {
////    std::ifstream ifile = std::ifstream(filename+"_indx",std::ios::in|std::ios::binary);
////    ifile.seekg(0,std::ios::end);
////    indxmax = (int) ifile.tellg()/(sizeof(int)*8);
////    ifile.close();
////    std::ifstream file = std::ifstream(filename+"_head",std::ios::in|std::ios::binary);
////    file.read((char*) &jumpmaplen, sizeof(int));
////    
////    for (int st=0;st<jumpmaplen;st++) {
////        uint8_t idlength;
////        file.read((char*) &idlength, sizeof(uint8_t));
////        char name[idlength];
////        file.read(&name[0],idlength);
////        long offset;
////        int depth;
////        file.read((char*) &offset,sizeof(long));
////        file.read((char*) &depth,sizeof(int));
////        jumpmap[std::string(name)] = std::pair<long,int>(offset,depth);
////    }
////    file.close();
////}
////void Filesystem::recordjumpmap() {
////    std::ofstream file = std::ofstream(filename+"_head",std::ios::out|std::ios::binary|std::ios::trunc);
////    file.write((char*) &jumpmaplen, sizeof(int));
////    for(auto iterator = jumpmap.begin(); iterator != jumpmap.end(); iterator++) {
////        uint8_t size = (uint8_t) iterator->first.size();
////        file.write((char*) &size, sizeof(uint8_t));
////        const char* name;
////        name = iterator->first.c_str();
////        file.write((char*)name,size);
////        file.write((char*) &iterator->second.first,sizeof(long));
////        file.write((char*) &iterator->second.second,sizeof(int));
////    }
////    file.close();
////}
//void Filesystem::pulloctreelevel(std::string structureid,OctreeSegment*& target,BlockLoc x,BlockLoc y,BlockLoc z) {
//    std::ifstream file = std::ifstream(filename+"/"+structureid+"/"+Location(x,y,z).tostring(),std::ios::in|std::ios::binary);
////    pulloctreelevel(file structure, jumpmap[structureid].first, jumpmap[structureid].second, target,x,y,z);
//    
//    file.close();
//}
//void Filesystem::pushoctreelevel(std::string structureid,OctreeSegment *& target,BlockLoc x,BlockLoc y,BlockLoc z) {
//    std::ofstream file = std::ofstream(filename+"/"+structureid+"/"+Location(x,y,z).tostring(),std::ios::out|std::ios::binary|std::ios::trunc);
//    file.close();
//    
//}
