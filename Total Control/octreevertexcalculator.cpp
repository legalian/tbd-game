//
//  octreevertexcalculator.cpp
//  Total Control
//
//  Created by Parker on 12/20/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "octree.h"

Edgedat::Edgedat() {}
Edgedat::Edgedat(uint8_t tc,int8_t xc,int8_t yc,int8_t zc) : x(xc),y(yc),z(zc),t(tc) {}
Edgedat::Edgedat(glm::vec3 i,float ti) {
    x = (int8_t)128*i.x;
    y = (int8_t)128*i.y;
    z = (int8_t)128*i.z;
    t = (uint8_t)255*ti;
}
Edgedat::Edgedat(float xi,float yi,float zi,float ti) {
    x = (int8_t)128*xi;
    y = (int8_t)128*yi;
    z = (int8_t)128*zi;
    t = (uint8_t)255*ti;
}
MatrixCarriage Edgedat::calcxqef()  {return calcqef(1*(t/255.0)*(x/128.0),(t/255.0),0,0);}
MatrixCarriage Edgedat::calcyqef()  {return calcqef(1*(t/255.0)*(y/128.0),0,(t/255.0),0);}
MatrixCarriage Edgedat::calczqef()  {return calcqef(1*(t/255.0)*(z/128.0),0,0,(t/255.0));}
glm::vec3      Edgedat::getnorm()   {return glm::vec3(x/128.0,y/128.0,z/128.0);}
float          Edgedat::getoffset() {return t/255.0;}
MatrixCarriage Edgedat::calcqef(QefPerc df,QefPerc mx,QefPerc my,QefPerc mz) {
    QefPerc xf = x/128.0;
    QefPerc yf = y/128.0;
    QefPerc zf = z/128.0;
    return MatrixCarriage(xf,yf,zf,
                        df*xf,df*yf,df*zf,
                        xf*xf,yf*yf,zf*zf,
                        xf*yf,xf*zf,yf*zf,
                        mx,my,mz,1);
}
MatrixCarriage operator+(const MatrixCarriage &m1, const MatrixCarriage &m2){
    return MatrixCarriage(m1.Vx+m2.Vx,
                          m1.Vy+m2.Vy,
                          m1.Vz+m2.Vz,
                          m1.Abx+m2.Abx,
                          m1.Aby+m2.Aby,
                          m1.Abz+m2.Abz,
                          m1.AAxx+m2.AAxx,
                          m1.AAyy+m2.AAyy,
                          m1.AAzz+m2.AAzz,
                          m1.AAxy+m2.AAxy,
                          m1.AAxz+m2.AAxz,
                          m1.AAyz+m2.AAyz,
                          m1.Mx+m2.Mx,
                          m1.My+m2.My,
                          m1.Mz+m2.Mz,
                          m1.Mt+m2.Mt);
}
MatrixCarriage::MatrixCarriage() :
Vx(0),Vy(0),Vz(0),Abx(0), Aby(0), Abz(0), AAxx(0), AAyy(0), AAzz(0), AAxy(0), AAxz(0), AAyz(0),Mx(0),My(0),Mz(0),Mt(0) {};
MatrixCarriage::MatrixCarriage(QefPerc vx,QefPerc vy,QefPerc vz,QefPerc b,QefPerc c,QefPerc d,QefPerc e,QefPerc f,QefPerc g,QefPerc h,QefPerc i,QefPerc j,QefPerc x,QefPerc y,QefPerc z,int co) :
Vx(vx),Vy(vy),Vz(vz),Abx(b), Aby(c), Abz(d), AAxx(e), AAyy(f), AAzz(g), AAxy(h), AAxz(i), AAyz(j) ,Mx(x),My(y),Mz(z),Mt(co){};
MatrixCarriage MatrixCarriage::translate(QefPerc x,QefPerc y,QefPerc z) {
    return MatrixCarriage(Vx,Vy,Vz,
                          Abx+x*AAxx+y*AAxy+z*AAxz,
                          Aby+x*AAxy+y*AAyy+z*AAyz,
                          Abz+x*AAxz+y*AAyz+z*AAzz,
                          AAxx,
                          AAyy,
                          AAzz,
                          AAxy,
                          AAxz,
                          AAyz,
                          Mx+x*Mt,
                          My+y*Mt,
                          Mz+z*Mt,
                          Mt);
}
glm::vec3 MatrixCarriage::evaluate(int lod) {
    glm::vec3 x = glm::inverse(glm::dmat3(AAxx,AAxy,AAxz,AAxy,AAyy,AAyz,AAxz,AAyz,AAzz))*glm::dvec3(Abx,Aby,Abz);
    if (x.x<0 or x.y<0 or x.z<0 or x.x>1<<lod or x.y>1<<lod or x.z>1<<lod or isnan(x.x) or isnan(x.y) or isnan(x.z)) {
        return glm::dvec3(Mx,My,Mz)*(1.0/Mt);
    }
    else return x;
}
glm::vec3 MatrixCarriage::evaluatenormal() {
    float js = sqrt(Vx*Vx+Vy*Vy+Vz*Vz);
    return glm::vec3(Vx/js,Vy/js,Vz/js);
}

void OctreeSegment::prepare(int x,int y,int z) {}
void OctreeBranch::prepare(int x,int y,int z) {
    int mask = 1<<depth;
    x&=~mask;
    y&=~mask;
    z&=~mask;
    subdivisions[0][0][0]->prepare(x     ,y     ,z     );
    subdivisions[1][0][0]->prepare(x|mask,y     ,z     );
    subdivisions[0][1][0]->prepare(x     ,y|mask,z     );
    subdivisions[1][1][0]->prepare(x|mask,y|mask,z     );
    subdivisions[0][0][1]->prepare(x     ,y     ,z|mask);
    subdivisions[1][0][1]->prepare(x|mask,y     ,z|mask);
    subdivisions[0][1][1]->prepare(x     ,y|mask,z|mask);
    subdivisions[1][1][1]->prepare(x|mask,y|mask,z|mask);
}
void OctreePortionAwareBranch::prepare(int x,int y,int z) {
    if (!prepared and hardload) {
        if (g_world->serloadedat(x+CHSIZE,y,z) and
            g_world->serloadedat(x,y+CHSIZE,z) and
            g_world->serloadedat(x,y,z+CHSIZE) and
            g_world->serloadedat(x,y+CHSIZE,z+CHSIZE) and
            g_world->serloadedat(x+CHSIZE,y,z+CHSIZE) and
            g_world->serloadedat(x+CHSIZE,y+CHSIZE,z)) {
//            hermitify(x,y,z);
            std::cout<<"AJAJAJAJAJ\n";
            for (int g=0;g<=MAX_WORLDFILE_GEOMSAVE;g++) {
                vertecies[g].clear();
                normals[g].clear();
            }
            g_vertecies = vertecies;
            g_normals = normals;
            
//            std::cout<<Location(x,y,z).tostring()<<"\n";
            
//                std::cout<<Location((x-ASBLOCKLOC(0))/CHSIZE,(y-ASBLOCKLOC(0))/CHSIZE,(z-ASBLOCKLOC(0))/CHSIZE).tostring()<<"\n";
            vertify(x,y,z);
//            testcoherence();
            baked = false;
            prepared=true;
        }
    }
    if (!psuedovoxed) {
        g_geometry = &geometry;
        for (g_lod=MIN_WORLDFILE_GEOMSAVE;g_lod<=MAX_WORLDFILE_GEOMSAVE;g_lod++) {
            manifestgeom(x,y,z);
            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                iterator->second.factor(g_lod);
            }
        }
        psuedovoxed=true;
    }
    if (prepared and !voxed) {
        g_geometry = &geometry;
        for (g_lod=0;g_lod<=MAX_WORLDFILE_GEOMSAVE;g_lod++) {
            manifestgeom(x,y,z);
            for(auto iterator = geometry.begin(); iterator != geometry.end(); iterator++) {
                iterator->second.factor(g_lod);
            }
        }
        voxed=true;
    }
}
void OctreeSegment::testcoherence() {}
void OctreeBranch::testcoherence() {
    if (depth<5 and point==-1) throw;
    subdivisions[0][0][0]->testcoherence();
    subdivisions[1][0][0]->testcoherence();
    subdivisions[0][1][0]->testcoherence();
    subdivisions[1][1][0]->testcoherence();
    subdivisions[0][0][1]->testcoherence();
    subdivisions[1][0][1]->testcoherence();
    subdivisions[0][1][1]->testcoherence();
    subdivisions[1][1][1]->testcoherence();
}
MatrixCarriage OctreeSegment::vertify(int x,int y,int z) {return MatrixCarriage();}
MatrixCarriage OctreeFeature::vertify(int x,int y,int z) {
    MatrixCarriage qef;
    OctreeSegment* buf[2][2][2];
//    buf[0][0][0] = g_world->indivref(x+xl,y+yl,z+zl);
    buf[1][0][0] = g_world->indivref(x+1,y  ,z  );
    buf[0][1][0] = g_world->indivref(x  ,y+1,z  );
    buf[1][1][0] = g_world->indivref(x+1,y+1,z  );
    buf[0][0][1] = g_world->indivref(x,  y  ,z+1);
    buf[1][0][1] = g_world->indivref(x+1,y  ,z+1);
    buf[0][1][1] = g_world->indivref(x  ,y+1,z+1);
    buf[1][1][1] = g_world->indivref(x+1,y+1,z+1);
    if (fillvalue!=buf[0][0][1]->getser(0,0,0)) qef = qef+zcon(0,0,0).calczqef();
    if (buf[1][0][0]->getser(0,0,0)!=buf[1][0][1]->getser(0,0,0)) qef = qef+buf[1][0][0]->zcon(0,0,0).calczqef().translate(1,0,0);
    if (buf[0][1][0]->getser(0,0,0)!=buf[0][1][1]->getser(0,0,0)) qef = qef+buf[0][1][0]->zcon(0,0,0).calczqef().translate(0,1,0);
    if (buf[1][1][0]->getser(0,0,0)!=buf[1][1][1]->getser(0,0,0)) qef = qef+buf[1][1][0]->zcon(0,0,0).calczqef().translate(1,1,0);
    if (fillvalue!=buf[0][1][0]->getser(0,0,0)) qef = qef+ycon(0,0,0).calcyqef();
    if (buf[1][0][0]->getser(0,0,0)!=buf[1][1][0]->getser(0,0,0)) qef = qef+buf[1][0][0]->ycon(0,0,0).calcyqef().translate(1,0,0);
    if (buf[0][0][1]->getser(0,0,0)!=buf[0][1][1]->getser(0,0,0)) qef = qef+buf[0][0][1]->ycon(0,0,0).calcyqef().translate(0,0,1);
    if (buf[1][0][1]->getser(0,0,0)!=buf[1][1][1]->getser(0,0,0)) qef = qef+buf[1][0][1]->ycon(0,0,0).calcyqef().translate(1,0,1);
    if (fillvalue!=buf[1][0][0]->getser(0,0,0)) qef = qef+xcon(0,0,0).calcxqef();
    if (buf[0][1][0]->getser(0,0,0)!=buf[1][1][0]->getser(0,0,0)) qef = qef+buf[0][1][0]->xcon(0,0,0).calcxqef().translate(0,1,0);
    if (buf[0][0][1]->getser(0,0,0)!=buf[1][0][1]->getser(0,0,0)) qef = qef+buf[0][0][1]->xcon(0,0,0).calcxqef().translate(0,0,1);
    if (buf[0][1][1]->getser(0,0,0)!=buf[1][1][1]->getser(0,0,0)) qef = qef+buf[0][1][1]->xcon(0,0,0).calcxqef().translate(0,1,1);

    point = (int)g_vertecies->size();
//    g_vertecies->push_back(glm::vec3((x-ASBLOCKLOC(0))+0.5,(y-ASBLOCKLOC(0))+0.5,(z-ASBLOCKLOC(0))+0.5));
    g_vertecies->push_back(qef.evaluate(0)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0)));
    g_normals->push_back(qef.evaluatenormal());
//    point = ;
    return qef;
}
MatrixCarriage OctreeBranch::vertify(int x,int y,int z) {
    int mask = 1<<depth;
    if (depth<MAX_WORLDFILE_GEOMSAVE) {
        MatrixCarriage q =
           subdivisions[0][0][0]->vertify(x     ,y     ,z     )+
           subdivisions[1][0][0]->vertify(x|mask,y     ,z     ).translate(1<<(depth),0         ,0)+
           subdivisions[0][1][0]->vertify(x     ,y|mask,z     ).translate(0         ,1<<(depth),0)+
           subdivisions[1][1][0]->vertify(x|mask,y|mask,z     ).translate(1<<(depth),1<<(depth),0)+
           subdivisions[0][0][1]->vertify(x     ,y     ,z|mask).translate(0         ,0         ,1<<(depth))+
           subdivisions[1][0][1]->vertify(x|mask,y     ,z|mask).translate(1<<(depth),0         ,1<<(depth))+
           subdivisions[0][1][1]->vertify(x     ,y|mask,z|mask).translate(0         ,1<<(depth),1<<(depth))+
           subdivisions[1][1][1]->vertify(x|mask,y|mask,z|mask).translate(1<<(depth),1<<(depth),1<<(depth));
        
        point = (int)g_vertecies[depth+1].size();
//        g_vertecies[depth+1].push_back(glm::vec3((x-ASBLOCKLOC(0))+mask,(y-ASBLOCKLOC(0))+mask,(z-ASBLOCKLOC(0))+mask));
        g_vertecies[depth+1].push_back(q.evaluate(depth+1)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0)));
        g_normals[depth+1].push_back(q.evaluatenormal());
        return q;
    } else {
        subdivisions[0][0][0]->vertify(x     ,y     ,z     );
        subdivisions[1][0][0]->vertify(x|mask,y     ,z     );
        subdivisions[0][1][0]->vertify(x     ,y|mask,z     );
        subdivisions[1][1][0]->vertify(x|mask,y|mask,z     );
        subdivisions[0][0][1]->vertify(x     ,y     ,z|mask);
        subdivisions[1][0][1]->vertify(x|mask,y     ,z|mask);
        subdivisions[0][1][1]->vertify(x     ,y|mask,z|mask);
        subdivisions[1][1][1]->vertify(x|mask,y|mask,z|mask);
        return MatrixCarriage();
    }
    
//    point = q.evaluate(depth+1)+glm::vec3(x-ASBLOCKLOC(0),y-ASBLOCKLOC(0),z-ASBLOCKLOC(0));
//    point = glm::vec3((x-ASBLOCKLOC(0))+mask,(y-ASBLOCKLOC(0))+mask,(z-ASBLOCKLOC(0))+mask);
//    return q;
}

//MatrixCarriage OctreeSegment::getqef() {return MatrixCarriage();}
//MatrixCarriage OctreeFeature::getqef() {return qef;}
//MatrixCarriage OctreeBranch::getqef() {
//    return subdivisions[0][0][0]->getqef()+
//           subdivisions[1][0][0]->getqef().translate(1<<(depth),0         ,0)+
//           subdivisions[0][1][0]->getqef().translate(0         ,1<<(depth),0)+
//           subdivisions[1][1][0]->getqef().translate(1<<(depth),1<<(depth),0)+
//           subdivisions[0][0][1]->getqef().translate(0         ,0         ,1<<(depth))+
//           subdivisions[1][0][1]->getqef().translate(1<<(depth),0         ,1<<(depth))+
//           subdivisions[0][1][1]->getqef().translate(0         ,1<<(depth),1<<(depth))+
//           subdivisions[1][1][1]->getqef().translate(1<<(depth),1<<(depth),1<<(depth));
//}




