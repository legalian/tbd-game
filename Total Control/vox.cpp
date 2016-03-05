//
//  vox.cpp
//  Total Control
//
//  Created by Parker Lawrence on 12/21/15.
//  Copyright (c) 2015 Parker Lawrence. All rights reserved.
//

#include "vox.h"
#include "lookuptables.cpp"

inline int MOD(int n,int m) {
    return ((n%m)+m)%m;
}
inline int TRUNC_DIV(int n, int m) {
    return (n-MOD(n,m))/m;
}
Location::Location(int qx, int qy, int qz) {
    x = qx;
    y = qy;
    z = qz;
}
Location::Location() {
    x = 0;
    y = 0;
    z = 0;
}
std::string Location::tostring() const {
    return "["+std::to_string(x)+","+std::to_string(y)+","+std::to_string(z)+"]";
}
bool operator< (const Location& l, const Location& r) {
    if (l.x<r.x) {return true;}
    if (l.x>r.x) {return false;}
    if (l.y<r.y) {return true;}
    if (l.y>r.y) {return false;}
    if (l.z<r.z) {return true;}
    if (l.z>r.z) {return false;}
    return false;
}
bool operator== (const Location& l, const Location& r) {
    if (l.x!=r.x) {return false;}
    if (l.y!=r.y) {return false;}
    if (l.z!=r.z) {return false;}
    return true;
}

//std::pair<Location,Location> OctreePortion::voxbounds() {
//    return std::pair<Location,Location>(Location(0,0,0),(Location(CHSIZE-1,CHSIZE-1,CHSIZE-1)));
//}

void OctreeSegment::setxcon(int x, int y, int z, const edgedat &edge) {throw;}
void OctreeSegment::setycon(int x, int y, int z, const edgedat &edge) {throw;}
void OctreeSegment::setzcon(int x, int y, int z, const edgedat &edge) {throw;}
edgedat OctreeSegment::getxcon(int x, int y, int z) {throw;}
edgedat OctreeSegment::getycon(int x, int y, int z) {throw;}
edgedat OctreeSegment::getzcon(int x, int y, int z) {throw;}
void OctreeSegment::savetofile(std::ostream &file) {throw;}
uint8_t OctreeSegment::getser(const int& x,const int& y,const int& z) {throw;}
bool OctreeSegment::uniqueat(const int& x,const int& y,const int& z,const int& recur) {throw;}

void OctreeBranch::setxcon(int x, int y, int z, const edgedat &edge) { subdivisions[x&1][y&1][z&1]->setxcon(x>>1,y>>1,z>>1,edge); }
void OctreeBranch::setycon(int x, int y, int z, const edgedat &edge) { subdivisions[x&1][y&1][z&1]->setycon(x>>1,y>>1,z>>1,edge); }
void OctreeBranch::setzcon(int x, int y, int z, const edgedat &edge) { subdivisions[x&1][y&1][z&1]->setzcon(x>>1,y>>1,z>>1,edge); }
edgedat OctreeBranch::getxcon(int x, int y, int z) { return subdivisions[x&1][y&1][z&1]->getxcon(x>>1,y>>1,z>>1); }
edgedat OctreeBranch::getycon(int x, int y, int z) { return subdivisions[x&1][y&1][z&1]->getycon(x>>1,y>>1,z>>1); }
edgedat OctreeBranch::getzcon(int x, int y, int z) { return subdivisions[x&1][y&1][z&1]->getzcon(x>>1,y>>1,z>>1); }
uint8_t OctreeBranch::getser(const int& x,const int& y,const int& z) { return subdivisions[x&1][y&1][z&1]->getser(x>>1,y>>1,z>>1); }
bool OctreeBranch::uniqueat(const int& x,const int& y,const int& z,const int& recur) { return subdivisions[x&1][y&1][z&1]->uniqueat(x>>1,y>>1,z>>1,recur-1); }



void OctreeLeaf::setxcon(int x, int y, int z, const edgedat &edge) { xplane[y][z] = edge; }
void OctreeLeaf::setycon(int x, int y, int z, const edgedat &edge) { yplane[x][z] = edge; }
void OctreeLeaf::setzcon(int x, int y, int z, const edgedat &edge) { zplane[y][z] = edge; }
edgedat OctreeLeaf::getxcon(int x, int y, int z) { return xplane[y][z]; }
edgedat OctreeLeaf::getycon(int x, int y, int z) { return yplane[x][z]; }
edgedat OctreeLeaf::getzcon(int x, int y, int z) { return zplane[x][y]; }
uint8_t OctreeLeaf::getser(const int& x,const int& y,const int& z) { return fillvalue; }
bool OctreeLeaf::uniqueat(const int& x,const int& y,const int& z,const int& recur) {
    return (x==((1<<recur)-1) or y==((1<<recur)-1) or z==((1<<recur)-1));
}


OctreeLeaf::OctreeLeaf(int recur) {
    xplane = new edgedat*[recur];
    yplane = new edgedat*[recur];
    zplane = new edgedat*[recur];
    for (int i = 0;i<recur;i++) {
        xplane[i] = new edgedat[recur];
        yplane[i] = new edgedat[recur];
        zplane[i] = new edgedat[recur];
    }
}



void OctreeLeaf::savetofile(std::ostream &file) {
    bool aug = true;
    file.write((char*)&aug,sizeof(bool));
    file.write((char*)&fillvalue,sizeof(uint8_t));
}
void OctreeBranch::savetofile(std::ostream &file) {
    bool aug = false;
    file.write((char*)&aug,sizeof(bool));
    subdivisions[0][0][0]->savetofile(file);
    subdivisions[1][0][0]->savetofile(file);
    subdivisions[0][1][0]->savetofile(file);
    subdivisions[1][1][0]->savetofile(file);
    subdivisions[0][0][1]->savetofile(file);
    subdivisions[1][0][1]->savetofile(file);
    subdivisions[0][1][1]->savetofile(file);
    subdivisions[1][1][1]->savetofile(file);
}

OctreeSegment* OctreeSegment::makeOctree(std::istream &file,int recur) {
    bool isfilled;
    file.read((char*)&isfilled,sizeof(bool));
    if (isfilled) {
        OctreeLeaf* node = new OctreeLeaf(recur);
        file.read((char*)&(node->fillvalue),sizeof(uint8_t));
        return node;
    } else {
        OctreeBranch* node = new OctreeBranch();
        node->subdivisions[0][0][0] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[1][0][0] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[0][1][0] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[1][1][0] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[0][0][1] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[1][0][1] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[0][1][1] = OctreeSegment::makeOctree(file,recur-1);
        node->subdivisions[1][1][1] = OctreeSegment::makeOctree(file,recur-1);
        return node;
    }
}
void OctreePortion::save(std::string filename) {
    std::ofstream myFile(filename, std::ios::out | std::ios::binary);
    data->savetofile(myFile);
    myFile.close();
}
OctreePortion::OctreePortion(std::string filename) {
    std::ifstream myFile(filename, std::ios::in | std::ios::binary);
    data = OctreeSegment::makeOctree(myFile,CHPOWER);
    myFile.close();
}


OctreePortion::OctreePortion(uint8_t (*dat)[CHSIZE][CHSIZE]) :
    data(OctreeSegment::makeOctree(dat,CHPOWER,0,0,0)) {}

OctreeSegment* OctreeSegment::makeOctree(uint8_t (*dat)[CHSIZE][CHSIZE],int recur,int xoff,int yoff, int zoff) {
    uint8_t fillvalue = dat[xoff][yoff][zoff];
    if (recur==0) {
        OctreeLeaf* node = new OctreeLeaf(recur);
        node->fillvalue = fillvalue;
        return node;
    }
    for (int x=xoff;x<xoff+(1<<recur);x++) {
        for (int y=yoff;y<yoff+(1<<recur);y++) {
            for (int z=zoff;z<zoff+(1<<recur);z++) {
                if (dat[x][y][z] != fillvalue) {
                    OctreeBranch* node = new OctreeBranch();
                    int nrc = 1<<(recur-1);
                    node->subdivisions[0][0][0] = OctreeSegment::makeOctree(dat,recur-1,xoff,yoff,zoff);
                    node->subdivisions[1][0][0] = OctreeSegment::makeOctree(dat,recur-1,xoff+nrc,yoff,zoff);
                    node->subdivisions[0][1][0] = OctreeSegment::makeOctree(dat,recur-1,xoff,yoff+nrc,zoff);
                    node->subdivisions[1][1][0] = OctreeSegment::makeOctree(dat,recur-1,xoff+nrc,yoff+nrc,zoff);
                    node->subdivisions[0][0][1] = OctreeSegment::makeOctree(dat,recur-1,xoff,yoff,zoff+nrc);
                    node->subdivisions[1][0][1] = OctreeSegment::makeOctree(dat,recur-1,xoff+nrc,yoff,zoff+nrc);
                    node->subdivisions[0][1][1] = OctreeSegment::makeOctree(dat,recur-1,xoff,yoff+nrc,zoff+nrc);
                    node->subdivisions[1][1][1] = OctreeSegment::makeOctree(dat,recur-1,xoff+nrc,yoff+nrc,zoff+nrc);
                    return node;
                }
            }
        }
    }
    OctreeLeaf* node = new OctreeLeaf(recur);
    node->fillvalue = fillvalue;
    return node;
}
uint8_t OctreePortion::getAt(int x, int y, int z) {
    return data->getser(flipbits(x),flipbits(y),flipbits(z));
//    return data.getser(x,y,z);
}
bool OctreePortion::uniqueat(int x, int y, int z) {
//    std::cout<<
    return data->uniqueat(flipbits(x),flipbits(y),flipbits(z),CHPOWER);
}
int OctreePortion::flipbits(int f) {
    int k = 0;
    k|=(f&1)<<6;
    k|=(f&2)<<4;
    k|=(f&4)<<2;
    k|=(f&8);
    k|=(f&16)>>2;
    k|=(f&32)>>4;
    k|=(f&64)>>6;
    if (f&128) {throw;}
//    k|=(f&1)<<7;
//    k|=(f&2)<<5;
//    k|=(f&4)<<3;
//    k|=(f&8)<<1;
//    k|=(f&16)>>1;
//    k|=(f&32)>>3;
//    k|=(f&64)>>5;
//    k|=(f&128)>>7;
//    if (f&256) {throw;}
    return k;
}


//unsigned int* Portion::solidface(int face) {
//    return NULL;
//}
//unsigned int* ComplexPortion::solidface(int face) {
//    if (face<3) {
//        if (depths[face] == 0) {
//            return NULL;
//        } else {
//            return &(defaults[face]);
//        }
//    } else {
//        if (depths[face] == CHSIZE-1) {
//            return NULL;
//        } else {
//            return &(defaults[face]);
//        }
//    }
//}
//unsigned int* SolidPortion::solidface(int face) {
//    return &fillId;
//}


void Structure::settransform(glm::mat4 trans) {
    transform = trans;
//    for (igeom=0;igeom<geom)
//    for (std::map<Location,Geom>::iterator it = geoms.begin();it!=geoms.end();it++) {
//        it->second.;
//    }
}
bool Structure::existsAt(int x, int y, int z) {
    if (portions.count(Location(x,y,z)) == 1) {
        return true;
    }
    return false;
}
GeomTerrain* Structure::createOrGet(Location po) {
    if (geoms.count(po) == 0) {
        GeomTerrain geom;
        geom.matrix = &transform;
        geoms.insert ( std::pair<Location,GeomTerrain>(po,geom) );
    }
    return &geoms[po];
}
uint8_t Structure::getAt(int x,int y,int z) {
    OctreePortion* sampler = portions[Location(TRUNC_DIV(x,CHSIZE),TRUNC_DIV(y,CHSIZE),TRUNC_DIV(z,CHSIZE))];
    return sampler->getAt((x&(CHSIZE-1)),(y&(CHSIZE-1)),z&(CHSIZE-1));
}
int Structure::getLodAt(int x, int y, int z) {
    OctreePortion* yupperoo = portions[Location(x,y,z)];
    return yupperoo->lod;
}

void Structure::refreshqueue() {
    queue.clear();
    while (queue.size() == 0 and loadstage<6) {
        int neg =-1*(loadstage);
        int pos = (loadstage);
        bool dotry = !bounded;
        for (int ix=neg;ix<=pos;ix++) {
            if ((lowestbounds.x<=epicenter.x+ix and epicenter.x+ix<=upperbounds.x) or dotry) {
                for (int iy=neg;iy<=pos;iy++) {
                    if ((lowestbounds.y<=epicenter.y+iy and epicenter.y+iy<=upperbounds.y) or dotry) {
                        for (int iz=neg;iz<=pos;iz++) {
                            if ((lowestbounds.z<=epicenter.z+iz and epicenter.z+iz<=upperbounds.z) or dotry) {
                                if (!existsAt(epicenter.x+ix,epicenter.y+iy,epicenter.z+iz)) {
                                    queue.push_back(Location(epicenter.x+ix,epicenter.y+iy,epicenter.z+iz));
                                }
                            }
                        }
                    }
                }
            }
        }
        loadstage++;
    }
}

void Structure::load(Location po,Generator* resource) {
    struct stat buffer;
    std::string filename = "structures/"+structureid+po.tostring()+".str";
    if (stat (filename.c_str(), &buffer) == 0) {
        OctreePortion* newportion = new OctreePortion(filename);
        portions.insert ( std::pair<Location,OctreePortion*>(po,newportion) );
    } else {
        OctreePortion* newportion = resource->terrain_update(this,po);
        newportion->save(filename);
        portions.insert ( std::pair<Location,OctreePortion*>(po,newportion) );
                
    }
    //    delete myportion;
    voxPortion(po);
//    voxSnippets(po);
}
void Structure::voxSnippets(Location po) {
//    std::cout<<portion.x<<","<<portion.y<<","<<portion.z<<"\n";
//    return;
    if (existsAt(po.x-1, po.y, po.z)) {
        voxXskirt(po.x-1, po.y, po.z);
    }
    if (existsAt(po.x+1, po.y, po.z)) {
        voxXskirt(po.x, po.y, po.z);
    }
    if (existsAt(po.x, po.y-1, po.z)) {
        voxYskirt(po.x, po.y-1, po.z);
    }
    if (existsAt(po.x, po.y+1, po.z)) {
        voxYskirt(po.x, po.y, po.z);
    }
    if (existsAt(po.x, po.y, po.z-1)) {
        voxZskirt(po.x, po.y, po.z-1);
    }
    if (existsAt(po.x, po.y, po.z+1)) {
        voxZskirt(po.x, po.y, po.z);
    }
    for (int yo1=-1;yo1<=0;yo1++) {
        for (int zo1=-1;zo1<=0;zo1++) {
            bool count = true;
            for (int yo2=0;yo2<=1;yo2++) {
                for (int zo2=0;zo2<=1;zo2++) {
                    if (!existsAt(po.x,po.y+yo1+yo2,po.z+zo1+zo2)) {
                        count = false;
                    }
                }
            }
            if (count) {
                voxXrow(po.x,po.y+yo1,po.z+zo1);
            }
        }
    }
    for (int xo1=-1;xo1<=0;xo1++) {
        for (int zo1=-1;zo1<=0;zo1++) {
            bool count = true;
            for (int xo2=0;xo2<=1;xo2++) {
                for (int zo2=0;zo2<=1;zo2++) {
                    if (!existsAt(po.x+xo1+xo2,po.y,po.z+zo1+zo2)) {
                        count = false;
                    }
                }
            }
            if (count) {
                voxYrow(po.x+xo1,po.y,po.z+zo1);
            }
        }
    }
    for (int xo1=-1;xo1<=0;xo1++) {
        for (int yo1=-1;yo1<=0;yo1++) {
            bool count = true;
            for (int xo2=0;xo2<=1;xo2++) {
                for (int yo2=0;yo2<=1;yo2++) {
                    if (!existsAt(po.x+xo1+xo2,po.y+yo1+yo2,po.z)) {
                        count = false;
                    }
                }
            }
            if (count) {
                voxZrow(po.x+xo1,po.y+yo1,po.z);
            }
        }
    }
    for (int xo1=-1;xo1<=0;xo1++) {
        for (int yo1=-1;yo1<=0;yo1++) {
            for (int zo1=-1;zo1<=0;zo1++) {
                bool count = true;
                for (int xo2=0;xo2<=1;xo2++) {
                    for (int yo2=0;yo2<=1;yo2++) {
                        for (int zo2=0;zo2<=1;zo2++) {
                            if (!existsAt(po.x+xo1+xo2,po.y+yo1+yo2,po.z+zo1+zo2)) {
                                count = false;
                            }
                        }
                    }
                }
                if (count) {
                    voxXYZcorner(po.x+xo1,po.y+yo1,po.z+zo1);
                }
            }
        }
    }
}

void Structure::voxPortion(Location portion) {
//    if (!portions[portion]->tryvox()) {return;}
    GeomTerrain geom;
    GLfloat afCubeValue[8];
    bool scCubeValue[8];
    //    std::vector<glm::vec3> triangles;
    OctreePortion* samddpler = portions[portion];
//    std::pair<Location,Location> bounds = portions[portion]->voxbounds();
    for (int xi=0;xi<CHSIZE-1;xi++) {
        for (int yi=0;yi<CHSIZE-1;yi++) {
            for (int zi=0;zi<CHSIZE-1;zi++) {
                int xt = portion.x*CHSIZE+xi;
                int yt = portion.y*CHSIZE+yi;
                int zt = portion.z*CHSIZE+zi;
                if (samddpler->uniqueat(xi,yi,zi)) {
                    std::vector<glm::vec3> triangles = std::vector<glm::vec3>();
                    for(int iVertex = 0; iVertex < 8; iVertex++) {
                        uint8_t code = samddpler->getAt(xi+a2fVertexOffset[iVertex][0],yi+a2fVertexOffset[iVertex][1],zi+a2fVertexOffset[iVertex][2]);
                        afCubeValue[iVertex] = 0;
                        scCubeValue[iVertex] = false;
                        if (code == 4) {
                            scCubeValue[iVertex] = true;
                        } else if (code == 3) {
                            afCubeValue[iVertex] = 1.0;
//                            scCubeValue[iVertex] = true;
                        } else if (code == 2) {
                            afCubeValue[iVertex] = 2.0/3.0;
//                            scCubeValue[iVertex] = true;
                        } else if (code == 1) {
                            afCubeValue[iVertex] = 1.0/3.0;
//                            scCubeValue[iVertex] = true;
                        }
                    }
                    MarchCube((portion.x*CHSIZE)+xi, (portion.y*CHSIZE)+yi, (portion.z*CHSIZE)+zi, afCubeValue,triangles);
                    SolidCube((portion.x*CHSIZE)+xi, (portion.y*CHSIZE)+yi, (portion.z*CHSIZE)+zi, scCubeValue,triangles);
                    
                    
                    for (int ivert = 0;ivert<triangles.size();ivert++) {
    //                    std::cout<<"output";
                        geom.addVert(triangles[ivert]);
                    }
                    geom.assumeNormals();
    //                addmarch(xt, yt, zt, &geom);
                }
            }
        }
    }
    geom.changed = true;//bake();
    //geom.
    geom.matrix = &transform;
    geoms.insert ( std::pair<Location,GeomTerrain>(portion,geom) );
}
void Structure::voxXskirt(int x, int y, int z) {
    //    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    if (getLodAt(x,y,z)>getLodAt(x+1,y,z)) {
        
    } else if (getLodAt(x,y,z)<getLodAt(x+1,y,z)) {
        
    } else {
        for (int yi=0;yi<CHSIZE-1;yi++) {
            for (int zi=0;zi<CHSIZE-1;zi++) {
                addmarch(x*CHSIZE+CHSIZE-1,y*CHSIZE+yi,z*CHSIZE+zi, geom);
            }
        }
    }
    geom->changed = true;//rebake();
}
void Structure::voxYskirt(int x, int y, int z) {
    //    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    if (getLodAt(x,y,z)>getLodAt(x,y+1,z)) {
        
    } else if (getLodAt(x,y,z)<getLodAt(x,y+1,z)) {
        
    } else {
        for (int xi=0;xi<CHSIZE-1;xi++) {
            for (int zi=0;zi<CHSIZE-1;zi++) {
                addmarch(x*CHSIZE+xi,y*CHSIZE+CHSIZE-1,z*CHSIZE+zi, geom);
            }
        }
    }
    geom->changed = true;//rebake();
}
void Structure::voxZskirt(int x, int y, int z) {
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    if (getLodAt(x,y,z)>getLodAt(x,y,z+1)) {
        
    } else if (getLodAt(x,y,z)<getLodAt(x,y,z+1)) {
        
    } else {
        for (int xi=0;xi<CHSIZE-1;xi++) {
            for (int yi=0;yi<CHSIZE-1;yi++) {
                addmarch(x*CHSIZE+xi,y*CHSIZE+yi,z*CHSIZE+CHSIZE-1, geom);
            }
        }
    }
    geom->changed = true;//rebake();
}
void Structure::voxXrow(int x, int y, int z) {
//    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    for (int ix=0;ix<CHSIZE-1;ix++) {
        addmarch(x*CHSIZE+ix,y*CHSIZE+CHSIZE-1,z*CHSIZE+CHSIZE-1, geom);
    }
    geom->changed = true;//rebake();
}
void Structure::voxYrow(int x, int y, int z) {
    //    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    for (int iy=0;iy<CHSIZE-1;iy++) {
        addmarch(x*CHSIZE+CHSIZE-1,y*CHSIZE+iy,z*CHSIZE+CHSIZE-1, geom);
    }
    geom->changed = true;//rebake();
}
void Structure::voxZrow(int x, int y, int z) {
    //    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    for (int iz=0;iz<CHSIZE-1;iz++) {
        addmarch(x*CHSIZE+CHSIZE-1,y*CHSIZE+CHSIZE-1,z*CHSIZE+iz, geom);
    }
    geom->changed = true;//rebake();
}
void Structure::voxXYZcorner(int x, int y, int z) {
    //    Geom* geom = &geoms[Location(x,y,z)];
    GeomTerrain* geom = createOrGet(Location(x,y,z));
    addmarch(x*CHSIZE+CHSIZE-1,y*CHSIZE+CHSIZE-1,z*CHSIZE+CHSIZE-1, geom);
    geom->changed = true;//rebake();
}
void Structure::addmarch(int x, int y, int z,GeomTerrain* addto) {
    GLfloat afCubeValue[8];
    bool scCubeValue[8];
    std::vector<glm::vec3> triangles = std::vector<glm::vec3>();
    
    for(int iVertex = 0; iVertex < 8; iVertex++) {
        uint8_t code = getAt(x+a2fVertexOffset[iVertex][0],y+a2fVertexOffset[iVertex][1],z+a2fVertexOffset[iVertex][2]);
        afCubeValue[iVertex] = 0;
        scCubeValue[iVertex] = false;
        if (code == 4) {
            scCubeValue[iVertex] = true;
        } else if (code == 3) {
            afCubeValue[iVertex] = 1.0;
//            scCubeValue[iVertex] = true;
        } else if (code == 2) {
            afCubeValue[iVertex] = 2.0/3.0;
//            scCubeValue[iVertex] = true;
        } else if (code == 1) {
            afCubeValue[iVertex] = 1.0/3.0;
//            scCubeValue[iVertex] = true;
        }
    }
    MarchCube(x,y,z, afCubeValue,triangles);
    SolidCube(x,y,z, scCubeValue,triangles);
    for (int ivert = 0;ivert<triangles.size();ivert++) {
        addto->addVert(triangles[ivert]);
    }
    addto->assumeNormals();
}
//void voxYskirt(int x, int y, int z,Structure sampler) {
//
//}
//void voxZskirt(int x, int y, int z,Structure sampler) {
//    
//}



void MarchCube(int fX, int fY, int fZ, GLfloat* afCubeValue,std::vector<glm::vec3> &triangles)
{
    glm::vec3 asEdgeVertex[12];
//    std::vector<glm::vec3> triangles;
    
    int iFlagIndex = 0;
    for(int iVertexTest = 0; iVertexTest < 8; iVertexTest++)
    {
        if(afCubeValue[iVertexTest] == 0) {
            iFlagIndex |= 1<<iVertexTest;
        }
    }
    
    int iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];
    if(iEdgeFlags == 0)
    {
        return;
    }
    for(int iEdge = 0; iEdge < 12; iEdge++)
    {
        if(iEdgeFlags & (1<<iEdge))
        {
            int e1 = a2iEdgeConnection[iEdge][0];
            int e2 = a2iEdgeConnection[iEdge][1];
            GLfloat offset;
            if (afCubeValue[e1]>afCubeValue[e2]) {
                offset = afCubeValue[e1];
            } else {
                offset = 1-afCubeValue[e2];
            }
            asEdgeVertex[iEdge].x = (fX + a2fVertexOffset[e1][0] + a2fEdgeDirection[iEdge][0]*offset)*SCALE;
            asEdgeVertex[iEdge].y = (fY + a2fVertexOffset[e1][1] + a2fEdgeDirection[iEdge][1]*offset)*SCALE;
            asEdgeVertex[iEdge].z = (fZ + a2fVertexOffset[e1][2] + a2fEdgeDirection[iEdge][2]*offset)*SCALE;
        }
    }
    
    for (int iTriangle = 0;;iTriangle+=3) {
        if (a2iTriangleConnectionTable[iFlagIndex][iTriangle] == -1) {
            break;
        }
        triangles.push_back(asEdgeVertex[a2iTriangleConnectionTable[iFlagIndex][iTriangle]]);
        triangles.push_back(asEdgeVertex[a2iTriangleConnectionTable[iFlagIndex][iTriangle+1]]);
        triangles.push_back(asEdgeVertex[a2iTriangleConnectionTable[iFlagIndex][iTriangle+2]]);
    };
}
void SolidCube(int fX, int fY, int fZ,bool* scCubeValue,std::vector<glm::vec3> &triangles) {
    for (int iface=0;iface<3;iface++) {
        if (scCubeValue[0] and not scCubeValue[axistoa2f[iface]]) {
            for (int i=0;i<6;i++) {
                glm::vec3 t;
                t.x = (fX+scVertexOffset[scFaceIndexes[iface][i]][0]*.5)*SCALE;
                t.y = (fY+scVertexOffset[scFaceIndexes[iface][i]][1]*.5)*SCALE;
                t.z = (fZ+scVertexOffset[scFaceIndexes[iface][i]][2]*.5)*SCALE;
                triangles.push_back(t);
            }
        } else if (scCubeValue[axistoa2f[iface]] and not scCubeValue[0]) {
            for (int i=0;i<6;i++) {
                glm::vec3 t;
                t.x = (fX+a2fVertexOffset[axistoa2f[iface]][0]+scVertexOffset[scFaceIndexes[iface+3][i]][0]*.5)*SCALE;
                t.y = (fY+a2fVertexOffset[axistoa2f[iface]][1]+scVertexOffset[scFaceIndexes[iface+3][i]][1]*.5)*SCALE;
                t.z = (fZ+a2fVertexOffset[axistoa2f[iface]][2]+scVertexOffset[scFaceIndexes[iface+3][i]][2]*.5)*SCALE;
                triangles.push_back(t);
            }
        }
    }
    
}


























