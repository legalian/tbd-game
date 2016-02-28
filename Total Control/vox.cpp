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


std::pair<Location,Location> OctreePortion::voxbounds() {
    return std::pair<Location,Location>(Location(0,0,0),(Location(CHSIZE-1,CHSIZE-1,CHSIZE-1)));
}

bool OctreePortion::tryvox() {
    return true;
}

unsigned int OctreeSegment::getser(const int& x,const int& y,const int& z) {
    if (isfilled) {
        return fillvalue;
    } else {
//        int andbit = CHSIZE/2;
//        int shiftbit = CHPOWER-1;
        return subdivisions[(x&1)][(y&1)][(z&1)]->getser(x>>1,y>>1,z>>1);
//        return subdivisions[(x&andbit)>>shiftbit][(y&andbit)>>shiftbit][(z&andbit)>>shiftbit]->getser(x<<1,y<<1,z<<1);
    }
}
bool OctreeSegment::uniqueat(const int& x,const int& y,const int& z,const int& recur) {
    if (isfilled) {
        return (x==((1<<recur)-1) or y==((1<<recur)-1) or z==((1<<recur)-1));
    } else {
//        int andbit = CHSIZE/2;
//        int shiftbit = CHPOWER-1;
        return subdivisions[(x&1)][(y&1)][(z&1)]->uniqueat(x>>1,y>>1,z>>1,recur-1);
    }
//    return true;
}
void OctreeSegment::savetofile(std::ostream &file) {
    file.write((char*)&isfilled,sizeof(bool));
    if (isfilled) {
        file.write((char*)&fillvalue,sizeof(unsigned int));
    } else {
        subdivisions[0][0][0]->savetofile(file);
        subdivisions[1][0][0]->savetofile(file);
        subdivisions[0][1][0]->savetofile(file);
        subdivisions[1][1][0]->savetofile(file);
        subdivisions[0][0][1]->savetofile(file);
        subdivisions[1][0][1]->savetofile(file);
        subdivisions[0][1][1]->savetofile(file);
        subdivisions[1][1][1]->savetofile(file);
    }
}
OctreeSegment::OctreeSegment(std::istream &file) {
    file.read((char*)&isfilled,sizeof(bool));
    if (isfilled) {
        file.read((char*)&fillvalue,sizeof(unsigned int));
    } else {
        subdivisions[0][0][0] = new OctreeSegment(file);
        subdivisions[1][0][0] = new OctreeSegment(file);
        subdivisions[0][1][0] = new OctreeSegment(file);
        subdivisions[1][1][0] = new OctreeSegment(file);
        subdivisions[0][0][1] = new OctreeSegment(file);
        subdivisions[1][0][1] = new OctreeSegment(file);
        subdivisions[0][1][1] = new OctreeSegment(file);
        subdivisions[1][1][1] = new OctreeSegment(file);
    }
}
OctreeSegment::OctreeSegment() {
    
}
void OctreePortion::save(std::string filename) {
    std::ofstream myFile(filename, std::ios::out | std::ios::binary);
    data.savetofile(myFile);
    myFile.close();
}
OctreePortion::OctreePortion(std::string filename) {
    std::ifstream myFile(filename, std::ios::in | std::ios::binary);
    data = OctreeSegment(myFile);
    myFile.close();
}


OctreePortion::OctreePortion(int (*dat)[CHSIZE][CHSIZE]) : data(OctreeSegment(dat,CHPOWER,0,0,0)) {
    
}
OctreeSegment::OctreeSegment(int (*dat)[CHSIZE][CHSIZE],int recur,int xoff,int yoff, int zoff) {
    fillvalue = dat[xoff][yoff][zoff];
    if (recur==0) {
        isfilled = true;
        return;
    }
    for (int x=xoff;x<xoff+(1<<recur);x++) {
        for (int y=yoff;y<yoff+(1<<recur);y++) {
            for (int z=zoff;z<zoff+(1<<recur);z++) {
                if (dat[x][y][z] != fillvalue) {
                    isfilled = false;
                    int nrc = recur-1;
                    subdivisions[0][0][0] = new OctreeSegment(dat,nrc,xoff,yoff,zoff);
                    subdivisions[1][0][0] = new OctreeSegment(dat,nrc,xoff+(1<<nrc),yoff,zoff);
                    subdivisions[0][1][0] = new OctreeSegment(dat,nrc,xoff,yoff+(1<<nrc),zoff);
                    subdivisions[1][1][0] = new OctreeSegment(dat,nrc,xoff+(1<<nrc),yoff+(1<<nrc),zoff);
                    subdivisions[0][0][1] = new OctreeSegment(dat,nrc,xoff,yoff,zoff+(1<<nrc));
                    subdivisions[1][0][1] = new OctreeSegment(dat,nrc,xoff+(1<<nrc),yoff,zoff+(1<<nrc));
                    subdivisions[0][1][1] = new OctreeSegment(dat,nrc,xoff,yoff+(1<<nrc),zoff+(1<<nrc));
                    subdivisions[1][1][1] = new OctreeSegment(dat,nrc,xoff+(1<<nrc),yoff+(1<<nrc),zoff+(1<<nrc));
                    return;
                }
            }
        }
    }
    isfilled = true;
}
unsigned int OctreePortion::getAt(int x, int y, int z) {
    return data.getser(flipbits(x),flipbits(y),flipbits(z));
//    return data.getser(x,y,z);
}
bool OctreePortion::uniqueat(int x, int y, int z) {
    return data.uniqueat(flipbits(x),flipbits(y),flipbits(z),CHPOWER);
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
unsigned int Structure::getAt(int x,int y,int z) {
    OctreePortion* sampler = portions[Location(TRUNC_DIV(x,CHSIZE),TRUNC_DIV(y,CHSIZE),TRUNC_DIV(z,CHSIZE))];
    return sampler->getAt((x&(CHSIZE-1)),(y&(CHSIZE-1)),z&(CHSIZE-1));
}
unsigned int Structure::getLodAt(int x, int y, int z) {
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
    voxSnippets(po);
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
    if (!portions[portion]->tryvox()) {return;}
    GeomTerrain geom;
    GLfloat afCubeValue[8];
    GLfloat scCubeValue[8];
    //    std::vector<glm::vec3> triangles;
    OctreePortion* samddpler = portions[portion];
    std::pair<Location,Location> bounds = portions[portion]->voxbounds();
    for (int xi=bounds.first.x;xi<bounds.second.x;xi++) {
        for (int yi=bounds.first.y;yi<bounds.second.y;yi++) {
            for (int zi=bounds.first.z;zi<bounds.second.z;zi++) {
                int xt = portion.x*CHSIZE+xi;
                int yt = portion.y*CHSIZE+yi;
                int zt = portion.z*CHSIZE+zi;
                if (samddpler->uniqueat(xi,yi,zi)) {
                    std::vector<glm::vec3> triangles = std::vector<glm::vec3>();
                    for(int iVertex = 0; iVertex < 8; iVertex++) {
                        unsigned int code = samddpler->getAt(xi+a2fVertexOffset[iVertex][0],yi+a2fVertexOffset[iVertex][1],zi+a2fVertexOffset[iVertex][2]);
                        afCubeValue[iVertex] = 0;
                        scCubeValue[iVertex] = 0;
                        if (code == 4) {
                            scCubeValue[iVertex] = .25;
                        } else if (code == 5) {
                            scCubeValue[iVertex] = .15;
                        } else if (code == 3) {
                            afCubeValue[iVertex] = 1.0;
                        } else if (code == 2) {
                            afCubeValue[iVertex] = 2.0/3.0;
                        } else if (code == 1) {
                            afCubeValue[iVertex] = 1.0/3.0;
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
    GLfloat scCubeValue[8];
    std::vector<glm::vec3> triangles = std::vector<glm::vec3>();
    
    for(int iVertex = 0; iVertex < 8; iVertex++) {
        unsigned int code = getAt(x+a2fVertexOffset[iVertex][0],y+a2fVertexOffset[iVertex][1],z+a2fVertexOffset[iVertex][2]);
        afCubeValue[iVertex] = 0;
        scCubeValue[iVertex] = 0;
        if (code == 4) {
            scCubeValue[iVertex] = .25;
        } else if (code == 5) {
            scCubeValue[iVertex] = .15;
        } else if (code == 3) {
            afCubeValue[iVertex] = 1.0;
        } else if (code == 2) {
            afCubeValue[iVertex] = 2.0/3.0;
        } else if (code == 1) {
            afCubeValue[iVertex] = 1.0/3.0;
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
            asEdgeVertex[iEdge].x = fX + a2fVertexOffset[e1][0] + a2fEdgeDirection[iEdge][0]*offset;
            asEdgeVertex[iEdge].y = fY + a2fVertexOffset[e1][1] + a2fEdgeDirection[iEdge][1]*offset;
            asEdgeVertex[iEdge].z = fZ + a2fVertexOffset[e1][2] + a2fEdgeDirection[iEdge][2]*offset;
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
void SolidCube(int fX, int fY, int fZ,GLfloat* scCubeValue,std::vector<glm::vec3> &triangles) {
    if (scCubeValue[0]==0 and scCubeValue[1]==0 and scCubeValue[2]==0 and scCubeValue[3]==0 and
        scCubeValue[4]==0 and scCubeValue[5]==0 and scCubeValue[6]==0 and scCubeValue[7]==0) {
        return;
    }
    for (int iface=0;iface<3;iface++) {
        if (scCubeValue[0]>scCubeValue[axistoa2f[iface]]) {
            for (int i=0;i<6;i++) {
                glm::vec3 t;
                t.x = fX+scVertexOffset[scFaceIndexes[iface][i]][0]*scCubeValue[0];
                t.y = fY+scVertexOffset[scFaceIndexes[iface][i]][1]*scCubeValue[0];
                t.z = fZ+scVertexOffset[scFaceIndexes[iface][i]][2]*scCubeValue[0];
                triangles.push_back(t);
            }
        } else if (scCubeValue[0]<scCubeValue[axistoa2f[iface]]) {
            for (int i=0;i<6;i++) {
                glm::vec3 t;
                t.x = fX+a2fVertexOffset[axistoa2f[iface]][0]+scVertexOffset[scFaceIndexes[iface+3][i]][0]*scCubeValue[axistoa2f[iface]];
                t.y = fY+a2fVertexOffset[axistoa2f[iface]][1]+scVertexOffset[scFaceIndexes[iface+3][i]][1]*scCubeValue[axistoa2f[iface]];
                t.z = fZ+a2fVertexOffset[axistoa2f[iface]][2]+scVertexOffset[scFaceIndexes[iface+3][i]][2]*scCubeValue[axistoa2f[iface]];
                triangles.push_back(t);
            }
        }
    }
    double x0min = std::min(std::min(scCubeValue[scplanes[0][0][0]], scCubeValue[scplanes[0][0][1]]),std::min( scCubeValue[scplanes[0][0][2]], scCubeValue[scplanes[0][0][3]]));
    double x1min = std::min(std::min(scCubeValue[scplanes[0][1][0]], scCubeValue[scplanes[0][1][1]]),std::min( scCubeValue[scplanes[0][1][2]], scCubeValue[scplanes[0][1][3]]));
    double y0min = std::min(std::min(scCubeValue[scplanes[1][0][0]], scCubeValue[scplanes[1][0][1]]),std::min( scCubeValue[scplanes[1][0][2]], scCubeValue[scplanes[1][0][3]]));
    double y1min = std::min(std::min(scCubeValue[scplanes[1][1][0]], scCubeValue[scplanes[1][1][1]]),std::min( scCubeValue[scplanes[1][1][2]], scCubeValue[scplanes[1][1][3]]));
    double z0min = std::min(std::min(scCubeValue[scplanes[2][0][0]], scCubeValue[scplanes[2][0][1]]),std::min( scCubeValue[scplanes[2][0][2]], scCubeValue[scplanes[2][0][3]]));
    double z1min = std::min(std::min(scCubeValue[scplanes[2][1][0]], scCubeValue[scplanes[2][1][1]]),std::min( scCubeValue[scplanes[2][1][2]], scCubeValue[scplanes[2][1][3]]));
    if (x0min!=0){
        if (x1min==0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+x0min,fY+x0min,fZ+x0min),
                glm::vec3(fX+x0min,fY+1-x0min,fZ+x0min),
                glm::vec3(fX+x0min,fY+x0min,fZ+1-x0min),
                glm::vec3(fX+x0min,fY+1-x0min,fZ+1-x0min)
            };
            int find[] = {0,1,3,0,3,2};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    } else {
        if (x1min!=0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+1-x1min,fY+x1min,fZ+x1min),
                glm::vec3(fX+1-x1min,fY+1-x1min,fZ+x1min),
                glm::vec3(fX+1-x1min,fY+x1min,fZ+1-x1min),
                glm::vec3(fX+1-x1min,fY+1-x1min,fZ+1-x1min)
            };
            int find[] = {0,3,1,0,2,3};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    }
    if (y0min!=0){
        if (y1min==0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+y0min,fY+y0min,fZ+y0min),
                glm::vec3(fX+1-y0min,fY+y0min,fZ+y0min),
                glm::vec3(fX+y0min,fY+y0min,fZ+1-y0min),
                glm::vec3(fX+1-y0min,fY+y0min,fZ+1-y0min)
            };
            int find[] = {0,3,1,0,2,3};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    } else {
        if (y1min!=0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+y1min,fY+1-y1min,fZ+y1min),
                glm::vec3(fX+1-y1min,fY+1-y1min,fZ+y1min),
                glm::vec3(fX+y1min,fY+1-y1min,fZ+1-y1min),
                glm::vec3(fX+1-y1min,fY+1-y1min,fZ+1-y1min)
            };
            int find[] = {0,1,3,0,3,2};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    }
    if (z0min!=0){
        if (z1min==0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+z0min,fY+z0min,fZ+z0min),
                glm::vec3(fX+1-z0min,fY+z0min,fZ+z0min),
                glm::vec3(fX+z0min,fY+1-z0min,fZ+z0min),
                glm::vec3(fX+1-z0min,fY+1-z0min,fZ+z0min)
            };
            int find[] = {0,1,3,0,3,2};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    } else {
        if (z1min!=0) {
            glm::vec3 face[4] = {
                glm::vec3(fX+z1min,fY+z1min,fZ+1-z1min),
                glm::vec3(fX+1-z1min,fY+z1min,fZ+1-z1min),
                glm::vec3(fX+z1min,fY+1-z1min,fZ+1-z1min),
                glm::vec3(fX+1-z1min,fY+1-z1min,fZ+1-z1min)
            };
            int find[] = {0,3,1,0,2,3};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
    }
    
    for (int iplane=0;iplane<6;iplane++) {
        
        double e0 = std::min(scCubeValue[scedgepairs[iplane][0][0]],scCubeValue[scedgepairs[iplane][0][1]]);
        double e1 = std::min(scCubeValue[scedgepairs[iplane][1][0]],scCubeValue[scedgepairs[iplane][1][1]]);
        if (e0>e1) {
            glm::vec3 face[4] = {
                glm::vec3(
                    fX+a2fVertexOffset[scedgepairs[iplane][0][0]][0]-scVertexOffset[scedgepairs[iplane][0][0]][0]*e0,
                    fY+a2fVertexOffset[scedgepairs[iplane][0][0]][1]-scVertexOffset[scedgepairs[iplane][0][0]][1]*e0,
                    fZ+a2fVertexOffset[scedgepairs[iplane][0][0]][2]-scVertexOffset[scedgepairs[iplane][0][0]][2]*e0),
                glm::vec3(
                    fX+a2fVertexOffset[scedgepairs[iplane][0][1]][0]-scVertexOffset[scedgepairs[iplane][0][1]][0]*e0,
                    fY+a2fVertexOffset[scedgepairs[iplane][0][1]][1]-scVertexOffset[scedgepairs[iplane][0][1]][1]*e0,
                    fZ+a2fVertexOffset[scedgepairs[iplane][0][1]][2]-scVertexOffset[scedgepairs[iplane][0][1]][2]*e0),
                glm::vec3(
                    fX+a2fVertexOffset[scedgepairs[iplane][0][0]][0]+scVertexOffset[axistoa2f[iplane/2]][0]*scVertexOffset[scedgepairs[iplane][0][0]][0]*e0,
                    fY+a2fVertexOffset[scedgepairs[iplane][0][0]][1]+scVertexOffset[axistoa2f[iplane/2]][1]*scVertexOffset[scedgepairs[iplane][0][0]][1]*e0,
                    fZ+a2fVertexOffset[scedgepairs[iplane][0][0]][2]+scVertexOffset[axistoa2f[iplane/2]][2]*scVertexOffset[scedgepairs[iplane][0][0]][2]*e0),
                glm::vec3(
                    fX+a2fVertexOffset[scedgepairs[iplane][0][1]][0]+scVertexOffset[axistoa2f[iplane/2]][0]*scVertexOffset[scedgepairs[iplane][0][1]][0]*e0,
                    fY+a2fVertexOffset[scedgepairs[iplane][0][1]][1]+scVertexOffset[axistoa2f[iplane/2]][1]*scVertexOffset[scedgepairs[iplane][0][1]][1]*e0,
                    fZ+a2fVertexOffset[scedgepairs[iplane][0][1]][2]+scVertexOffset[axistoa2f[iplane/2]][2]*scVertexOffset[scedgepairs[iplane][0][1]][2]*e0)
            };
            int find[] = {0,1,3,0,3,2};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        } else if (e0<e1) {
            glm::vec3 face[4] = {
                glm::vec3(
                          fX+a2fVertexOffset[scedgepairs[iplane][1][0]][0]-scVertexOffset[scedgepairs[iplane][1][0]][0]*e1,
                          fY+a2fVertexOffset[scedgepairs[iplane][1][0]][1]-scVertexOffset[scedgepairs[iplane][1][0]][1]*e1,
                          fZ+a2fVertexOffset[scedgepairs[iplane][1][0]][2]-scVertexOffset[scedgepairs[iplane][1][0]][2]*e1),
                glm::vec3(
                          fX+a2fVertexOffset[scedgepairs[iplane][1][1]][0]-scVertexOffset[scedgepairs[iplane][1][1]][0]*e1,
                          fY+a2fVertexOffset[scedgepairs[iplane][1][1]][1]-scVertexOffset[scedgepairs[iplane][1][1]][1]*e1,
                          fZ+a2fVertexOffset[scedgepairs[iplane][1][1]][2]-scVertexOffset[scedgepairs[iplane][1][1]][2]*e1),
                glm::vec3(
                          fX+a2fVertexOffset[scedgepairs[iplane][1][0]][0]+scVertexOffset[axistoa2f[iplane/2]][0]*scVertexOffset[scedgepairs[iplane][1][0]][0]*e1,
                          fY+a2fVertexOffset[scedgepairs[iplane][1][0]][1]+scVertexOffset[axistoa2f[iplane/2]][1]*scVertexOffset[scedgepairs[iplane][1][0]][1]*e1,
                          fZ+a2fVertexOffset[scedgepairs[iplane][1][0]][2]+scVertexOffset[axistoa2f[iplane/2]][2]*scVertexOffset[scedgepairs[iplane][1][0]][2]*e1),
                glm::vec3(
                          fX+a2fVertexOffset[scedgepairs[iplane][1][1]][0]+scVertexOffset[axistoa2f[iplane/2]][0]*scVertexOffset[scedgepairs[iplane][1][1]][0]*e1,
                          fY+a2fVertexOffset[scedgepairs[iplane][1][1]][1]+scVertexOffset[axistoa2f[iplane/2]][1]*scVertexOffset[scedgepairs[iplane][1][1]][1]*e1,
                          fZ+a2fVertexOffset[scedgepairs[iplane][1][1]][2]+scVertexOffset[axistoa2f[iplane/2]][2]*scVertexOffset[scedgepairs[iplane][1][1]][2]*e1)
            };
            int find[] = {0,3,1,0,2,3};
            for (int i=0;i<6;i++) {
                triangles.push_back(face[find[i]]);
            }
        }
        
    }
    
}


























