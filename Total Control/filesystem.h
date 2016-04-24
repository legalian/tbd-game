//////
//////  filesystem.hpp
//////  Total Control
//////
//////  Created by Parker on 4/6/16.
//////  Copyright © 2016 Parker Lawrence. All rights reserved.
//////
////
//#ifndef filesystem_hpp
//#define filesystem_hpp
////
////#include <stdio.h>
////#include <fstream>
////#include <map>
//#include "constants.h"
////
//#include "octree.h"
//#include "environment.h"
//#include <boost/filesystem.hpp>
////
////class Filesystem {
////private:
////    static std::string filename;
////public:
////    static void linktofile(std::string);
////    static void createnewlink(std::string);
////    static void pushoctreelevel(std::string,OctreeSegment*&,BlockLoc,BlockLoc,BlockLoc);
////    static void pulloctreelevel(std::string,OctreeSegment*&,BlockLoc,BlockLoc,BlockLoc);
////};
//
//class Filesystem {
//private:
//    static std::string filename;
//    static Environment* worldlink;
////    static std::map<std::string,std::pair<long,int>> jumpmap;
////    static int jumpmaplen;
////    static int indxmax;
////    
////    static int grabempty(std::fstream&);
////    static void registerempty(std::fstream&,int);
////    
////    static void generatejumpmap();
////    static void recordjumpmap();
////    
////    static void pulloctreelevel(std::ifstream&,long,int,OctreeSegment*&,BlockLoc,BlockLoc,BlockLoc);
////    static OctreeSegment* readoctree(std::ifstream&,int);
//    
//public:
//    static void linkenvtofile(Environment*,std::string);
//    static void createnewlink(Environment*,std::string);
//    
//    static void openstructureids();
//    
//    static void pushoctreelevel(std::string,OctreeSegment*&,BlockLoc,BlockLoc,BlockLoc);
//    static void pulloctreelevel(std::string,OctreeSegment*&,BlockLoc,BlockLoc,BlockLoc);
//};
////
////
//#endif /* filesystem_hpp */
