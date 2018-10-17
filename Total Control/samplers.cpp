//
//  samplers.cpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#include "samplers.h"

#ifdef PLOTTER_MODE
using std::function;
using glm::vec4;
using glm::vec3;
using glm::vec2;
using std::string;
using std::to_string;
using glm::length;
using glm::dot;
using glm::cross;
using glm::normalize;
using glm::max;
using glm::min;
using glm::abs;
#define Gen1 Gen
#define STYPE1 float
#define STYPE2 vec2
#define STYPE3 vec3
#define STYPE4 vec4
#define FUNC(k) [=](vec3 e)throw()->STYPE##k
#define DERIV(k) [=](vec3 e)throw()->Triplet<STYPE##k >
#define GEN(k,A,B,C) {return Gen##k (FUNC(k){A},DERIV(k){B},C);}
#define ASSEMBLE(k) return Triplet<STYPE##k>(D1,D2,D3)

#define GENMAKE0(k,O,M)GEN(k,return O;,return 0;,M)
#define GENMAKE1(k,O,D,M)GEN(k,auto a=ai.func(e);return O;,\
auto a=ai.func(e);auto dda=ai.deriv(e);auto da=dda.x;auto D1=D;da=dda.y;auto D2=D;da=dda.z;auto D3=D;ASSEMBLE(k);,M)
#define GENMAKE2(k,O,D,M) GEN(k,auto a=ai.func(e);auto b=bi.func(e);return O;,\
auto a=ai.func(e);auto b=bi.func(e);auto dda=ai.deriv(e);auto ddb=bi.deriv(e);\
auto da=dda.x;auto db=ddb.x;auto D1=D;da=dda.y;db=ddb.y;auto D2=D;da=dda.z;db=ddb.z;auto D3=D;ASSEMBLE(k);,M)

#define CONSTMAKE1(k,O,M) GEN(k,auto a=ai.func(e);return O;,return 0;,M)
#define CONSTMAKE2(k,O,M) GEN(k,auto a=ai.func(e);auto b=bi.func(e);return O;,return 0;,M)

#define LINEARMAKE1(k,O,D,M)GEN(k,auto a=ai.func(e);return O;,\
auto dda=ai.deriv(e);auto da=dda.x;auto D1=D;da=dda.y;auto D2=D;da=dda.z;auto D3=D;ASSEMBLE(k);,M)
#define LINEARMAKE2(k,O,D,M)GEN(k,auto a=ai.func(e);auto b=bi.func(e);return O;,auto dda=ai.deriv(e);auto ddb=bi.deriv(e);\
auto da=dda.x;auto db=ddb.x;auto D1=D;da=dda.y;db=ddb.y;auto D2=D;da=dda.z;db=ddb.z;auto D3=D;ASSEMBLE(k);,M)

#define MEMBERACC(k,A,M) GEN(k,return func(e).A;,auto a = deriv(e);return Triplet<STYPE##k>(a.x.A,a.y.A,a.z.A);,M);

string tostring(float a) {return to_string(a);}
string tostring(float a,float b) {return "(float2)("+to_string(a)+","+to_string(b)+")";}
string tostring(float a,float b,float c) {return "(float3)("+to_string(a)+","+to_string(b)+","+to_string(c)+")";}
string tostring(float a,float b,float c,float d) {return "(float4)("+to_string(a)+","+to_string(b)+","+to_string(c)+","+to_string(d)+")";}
string tostring(vec2 ab) {return tostring(ab.x,ab.y);}
string tostring(vec3 abc) {return tostring(abc.x,abc.y,abc.z);}
string tostring(vec4 abcd) {return tostring(abcd.x,abcd.y,abcd.z,abcd.w);}

Gen::Gen(function<float(vec3)>f,function<Triplet<float>(vec3)>d,string m):func(f),deriv(d),mountable(m){}
Gen::Gen(float a):func(FUNC(1){return a;}),deriv(DERIV(1){return 0;}),mountable(tostring(a)) {}
Gen2::Gen2(function<vec2(vec3)>f,function<Triplet<vec2>(vec3)>d,string m):func(f),deriv(d),mountable(m){}
Gen2::Gen2(float a,float b):func(FUNC(2){return vec2(a,b);}),deriv(DERIV(2){return 0;}),mountable(tostring(a,b)) {}
Gen2::Gen2(vec2 ab):func(FUNC(2){return ab;}),deriv(DERIV(2){return 0;}),mountable(tostring(ab)) {}
Gen2::Gen2(Gen a,Gen b):
    func(FUNC(2){return vec2(a.func(e),b.func(e));}),
    deriv(DERIV(2){return tripletassemble(a.deriv(e),b.deriv(e));}),
    mountable("vec2("+a.mountable+","+b.mountable+")") {}
Gen Gen2::x() MEMBERACC(1,x,mountable+".x")
Gen Gen2::y() MEMBERACC(1,y,mountable+".y")
Gen3::Gen3(function<vec3(vec3)>f,function<Triplet<vec3>(vec3)>d,string m):func(f),deriv(d),mountable(m){}
Gen3::Gen3(float a,float b,float c):func(FUNC(3){return vec3(a,b,c);}),deriv(DERIV(3){return 0;}),mountable(tostring(a,b,c)) {}
Gen3::Gen3(vec3 abc):func(FUNC(3){return abc;}),deriv(DERIV(3){return 0;}),mountable(tostring(abc)) {}
Gen3::Gen3(Gen a,Gen b,Gen c):
    func(FUNC(3){return vec3(a.func(e),b.func(e),c.func(e));}),
    deriv(DERIV(3){return tripletassemble(a.deriv(e),b.deriv(e),c.deriv(e));}),
    mountable("vec3("+a.mountable+","+b.mountable+","+c.mountable+")") {}
Gen Gen3::x() MEMBERACC(1,x,mountable+".x")
Gen Gen3::y() MEMBERACC(1,y,mountable+".y")
Gen Gen3::z() MEMBERACC(1,z,mountable+".z")
Gen4::Gen4(function<vec4(vec3)>f,function<Triplet<vec4>(vec3)>d,string m):func(f),deriv(d),mountable(m){}
Gen4::Gen4(float a,float b,float c,float d):func(FUNC(4){return vec4(a,b,c,d);}),deriv(DERIV(4){return 0;}),mountable(tostring(a,b,c,d)) {}
Gen4::Gen4(vec4 abcd):func(FUNC(4){return abcd;}),deriv(DERIV(4){return 0;}),mountable(tostring(abcd)) {}
Gen4::Gen4(Gen a,Gen b,Gen c,Gen d):
    func([=](vec3 e)throw()->vec4{return vec4(a.func(e),b.func(e),c.func(e),d.func(e));}),
    deriv(DERIV(4){return tripletassemble(a.deriv(e),b.deriv(e),c.deriv(e),d.deriv(e));}),
    mountable("vec4("+a.mountable+","+b.mountable+","+c.mountable+","+d.mountable+")") {}
Gen Gen4::x() MEMBERACC(1,x,mountable+".x")
Gen Gen4::y() MEMBERACC(1,y,mountable+".y")
Gen Gen4::z() MEMBERACC(1,z,mountable+".z")
Gen Gen4::w() MEMBERACC(1,w,mountable+".w")

Gen sin(const Gen& ai) GENMAKE1(1,sin(a)  ,          da*cos(a),"sin((float)"+ai.mountable+")")
Gen cos(const Gen& ai) GENMAKE1(1,cos(a)  ,         -da*sin(a),"cos((float)"+ai.mountable+")")
Gen tan(const Gen& ai) GENMAKE1(1,tan(a)  , da/(cos(a)*cos(a)),"tan((float)"+ai.mountable+")")
Gen cot(const Gen& ai) GENMAKE1(1,1/tan(a),-da/(sin(a)*sin(a)),"cot((float)"+ai.mountable+")")
Gen sec(const Gen& ai) GENMAKE1(1,1/cos(a),   da*tan(a)/cos(a),"sec((float)"+ai.mountable+")")
Gen csc(const Gen& ai) GENMAKE1(1,1/sin(a),-da/(sin(a)*tan(a)),"csc((float)"+ai.mountable+")")
Gen ln(const Gen& ai)  GENMAKE1(1,log(a),da/a,"ln((float)"+ai.mountable+")")
Gen pow(const Gen& ai) GENMAKE1(1,exp(a),da*exp(a),"exp("+ai.mountable+")")
Gen pow(const Gen& ai,float b) GENMAKE1(1,pow(a,b),da*(b-1)*pow(a,b-1),"pow("+ai.mountable+","+to_string(b)+")")
Gen pow(float b,const Gen& ai) GENMAKE1(1,pow(b,a),da*pow(b,a)*log(b),"pow("+ai.mountable+","+to_string(b)+")")
Gen pow(const Gen& ai,const Gen& bi) GENMAKE2(1,pow(a,b),da*(b-1)*pow(a,b-1)+db*pow(a,b)*log(a),"pow("+ai.mountable+","+bi.mountable+")")

Gen floor(const Gen& ai)   CONSTMAKE1(1,floor(a),"floor("+ai.mountable+")")
Gen2 floor(const Gen2& ai) CONSTMAKE1(2,floor(a),"floor("+ai.mountable+")")
Gen3 floor(const Gen3& ai) CONSTMAKE1(3,floor(a),"floor("+ai.mountable+")")
Gen4 floor(const Gen4& ai) CONSTMAKE1(4,floor(a),"floor("+ai.mountable+")")
Gen ceil(const Gen& ai)    CONSTMAKE1(1,ceil(a),"ceil("+ai.mountable+")")
Gen2 ceil(const Gen2& ai)  CONSTMAKE1(2,ceil(a),"ceil("+ai.mountable+")")
Gen3 ceil(const Gen3& ai)  CONSTMAKE1(3,ceil(a),"ceil("+ai.mountable+")")
Gen4 ceil(const Gen4& ai)  CONSTMAKE1(4,ceil(a),"ceil("+ai.mountable+")")
Gen fract(const Gen& ai)   LINEARMAKE1(1,a-floor(a),da,"fract("+ai.mountable+")")
Gen2 fract(const Gen2& ai) LINEARMAKE1(2,a-floor(a),da,"fract("+ai.mountable+")")
Gen3 fract(const Gen3& ai) LINEARMAKE1(3,a-floor(a),da,"fract("+ai.mountable+")")
Gen4 fract(const Gen4& ai) LINEARMAKE1(4,a-floor(a),da,"fract("+ai.mountable+")")
Gen magnitude(const Gen2& ai) GENMAKE1(1,length(a),dot(da,a)/length(a),"fast_length("+ai.mountable+")")
Gen magnitude(const Gen3& ai) GENMAKE1(1,length(a),dot(da,a)/length(a),"fast_length("+ai.mountable+")")
Gen magnitude(const Gen4& ai) GENMAKE1(1,length(a),dot(da,a)/length(a),"fast_length("+ai.mountable+")")
Gen squaredmagnitude(const Gen2& ai) GENMAKE1(1,dot(a,a),2*dot(da,a),"dot("+ai.mountable+","+ai.mountable+")")
Gen squaredmagnitude(const Gen3& ai) GENMAKE1(1,dot(a,a),2*dot(da,a),"dot("+ai.mountable+","+ai.mountable+")")
Gen squaredmagnitude(const Gen4& ai) GENMAKE1(1,dot(a,a),2*dot(da,a),"dot("+ai.mountable+","+ai.mountable+")")
Gen2 normalize(const Gen2& ai) GENMAKE1(2,normalize(a),(a*dot(da,a)+vec2(1,1))/length(a),"fast_normalize("+ai.mountable+","+ai.mountable+")")
Gen3 normalize(const Gen3& ai) GENMAKE1(3,normalize(a),(a*dot(da,a)+vec3(1,1,1))/length(a),"fast_normalize("+ai.mountable+","+ai.mountable+")")
Gen4 normalize(const Gen4& ai) GENMAKE1(4,normalize(a),(a*dot(da,a)+vec4(1,1,1,1))/length(a),"fast_normalize("+ai.mountable+","+ai.mountable+")")
Gen3 cross(const Gen3& ai,const Gen3& bi) GENMAKE2(3,cross(a,b),cross(da,b)-cross(db,a),"cross("+ai.mountable+","+bi.mountable+")")


Gen  max(const Gen&  ai,const Gen& bi)  GENMAKE2(1,max(a,b),a>b?da:db,"fmax("+ai.mountable+","+bi.mountable+")")
Gen2 max(const Gen2& ai,const Gen2& bi) GENMAKE2(2,max(a,b),vec2(a.x>b.x?da.x:db.x,a.y>b.y?da.y:db.y),"fmax("+ai.mountable+","+bi.mountable+")")
Gen3 max(const Gen3& ai,const Gen3& bi) GENMAKE2(3,max(a,b),
vec3(a.x>b.x?da.x:db.x,a.y>b.y?da.y:db.y,a.z>b.z?da.z:db.z),"fmax("+ai.mountable+","+bi.mountable+")")
Gen4 max(const Gen4& ai,const Gen4& bi) GENMAKE2(4,max(a,b),
vec4(a.x>b.x?da.x:db.x,a.y>b.y?da.y:db.y,a.z>b.z?da.z:db.z,a.w>b.w?da.w:db.w),"fmax("+ai.mountable+","+bi.mountable+")")
Gen  min(const Gen&  ai,const Gen& bi)  GENMAKE2(1,min(a,b),a<b?da:db,"fmin("+ai.mountable+","+bi.mountable+")")
Gen2 min(const Gen2& ai,const Gen2& bi) GENMAKE2(2,min(a,b),vec2(a.x<b.x?da.x:db.x,a.y<b.y?da.y:db.y),"fmin("+ai.mountable+","+bi.mountable+")")
Gen3 min(const Gen3& ai,const Gen3& bi) GENMAKE2(3,min(a,b),
vec3(a.x<b.x?da.x:db.x,a.y<b.y?da.y:db.y,a.z<b.z?da.z:db.z),"fmin("+ai.mountable+","+bi.mountable+")")
Gen4 min(const Gen4& ai,const Gen4& bi) GENMAKE2(4,min(a,b),
vec4(a.x<b.x?da.x:db.x,a.y<b.y?da.y:db.y,a.z<b.z?da.z:db.z,a.w<b.w?da.w:db.w),"fmin("+ai.mountable+","+bi.mountable+")")
Gen  abs(const Gen&  ai) GENMAKE1(1,abs(a),a<0?-da:da,"fabs("+ai.mountable+")")
Gen2 abs(const Gen2& ai) GENMAKE1(2,abs(a),vec2(a.x<0?-da.x:da.x,a.y<0?-da.y:da.y),"fmin("+ai.mountable+")")
Gen3 abs(const Gen3& ai) GENMAKE1(3,abs(a),
vec3(a.x<0?-da.x:da.x,a.y<0?-da.y:da.y,a.z<0?-da.z:da.z),"fmin("+ai.mountable+")")
Gen4 abs(const Gen4& ai) GENMAKE1(4,abs(a),
vec4(a.x<0?-da.x:da.x,a.y<0?-da.y:da.y,a.z<0?-da.z:da.z,a.w<0?-da.w:da.w),"fmin("+ai.mountable+")")


Gen  operator+(const Gen&  ai,const Gen&  bi) LINEARMAKE2(1,a+b,da+db,"("+ai.mountable+"+"+bi.mountable+")")
Gen2 operator+(const Gen2& ai,const Gen2& bi) LINEARMAKE2(2,a+b,da+db,"("+ai.mountable+"+"+bi.mountable+")")
Gen3 operator+(const Gen3& ai,const Gen3& bi) LINEARMAKE2(3,a+b,da+db,"("+ai.mountable+"+"+bi.mountable+")")
Gen4 operator+(const Gen4& ai,const Gen4& bi) LINEARMAKE2(4,a+b,da+db,"("+ai.mountable+"+"+bi.mountable+")")
Gen  operator+(const Gen&  ai,float b) LINEARMAKE1(1,a+b,da,"("+ai.mountable+"+"+tostring(b)+")")
Gen2 operator+(const Gen2& ai,vec2 b ) LINEARMAKE1(2,a+b,da,"("+ai.mountable+"+"+tostring(b)+")")
Gen3 operator+(const Gen3& ai,vec3 b ) LINEARMAKE1(3,a+b,da,"("+ai.mountable+"+"+tostring(b)+")")
Gen4 operator+(const Gen4& ai,vec4 b ) LINEARMAKE1(4,a+b,da,"("+ai.mountable+"+"+tostring(b)+")")
Gen  operator+(float ai,const Gen&  bi) {return bi+ai;}
Gen2 operator+(vec2  ai,const Gen2& bi) {return bi+ai;}
Gen3 operator+(vec3  ai,const Gen3& bi) {return bi+ai;}
Gen4 operator+(vec4  ai,const Gen4& bi) {return bi+ai;}
Gen  operator-(const Gen&  ai,const Gen&  bi) LINEARMAKE2(1,a-b,da-db,"("+ai.mountable+"-"+bi.mountable+")")
Gen2 operator-(const Gen2& ai,const Gen2& bi) LINEARMAKE2(2,a-b,da-db,"("+ai.mountable+"-"+bi.mountable+")")
Gen3 operator-(const Gen3& ai,const Gen3& bi) LINEARMAKE2(3,a-b,da-db,"("+ai.mountable+"-"+bi.mountable+")")
Gen4 operator-(const Gen4& ai,const Gen4& bi) LINEARMAKE2(4,a-b,da-db,"("+ai.mountable+"-"+bi.mountable+")")
Gen  operator-(const Gen&  ai,float b) {return ai+(-b);}
Gen2 operator-(const Gen2& ai,vec2 b ) {return ai+(-b);}
Gen3 operator-(const Gen3& ai,vec3 b ) {return ai+(-b);}
Gen4 operator-(const Gen4& ai,vec4 b ) {return ai+(-b);}
Gen  operator-(float b,const Gen&  ai) LINEARMAKE1(1,b-a,-da,"("+tostring(b)+"-"+ai.mountable+")")
Gen2 operator-(vec2  b,const Gen2& ai) LINEARMAKE1(2,b-a,-da,"("+tostring(b)+"-"+ai.mountable+")")
Gen3 operator-(vec3  b,const Gen3& ai) LINEARMAKE1(3,b-a,-da,"("+tostring(b)+"-"+ai.mountable+")")
Gen4 operator-(vec4  b,const Gen4& ai) LINEARMAKE1(4,b-a,-da,"("+tostring(b)+"-"+ai.mountable+")")
Gen  operator*(const Gen& ai,const Gen& bi)  GENMAKE2(1,a*b,a*db+b*da,"("+ai.mountable+"*"+bi.mountable+")")
Gen2 operator*(const Gen& ai,const Gen2& bi) GENMAKE2(2,a*b,a*db+b*da,"("+ai.mountable+"*"+bi.mountable+")")
Gen3 operator*(const Gen& ai,const Gen3& bi) GENMAKE2(3,a*b,a*db+b*da,"("+ai.mountable+"*"+bi.mountable+")")
Gen4 operator*(const Gen& ai,const Gen4& bi) GENMAKE2(4,a*b,a*db+b*da,"("+ai.mountable+"*"+bi.mountable+")")
Gen  operator*(const Gen& ai,float b)   LINEARMAKE1(1,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen  operator*(float b,const Gen& ai) {return ai*b;}
Gen2 operator*(const Gen& ai,vec2 b) LINEARMAKE1(2,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen3 operator*(const Gen& ai,vec3 b) LINEARMAKE1(3,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen4 operator*(const Gen& ai,vec4 b) LINEARMAKE1(4,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen2 operator*(const Gen2& ai,float b) LINEARMAKE1(2,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen3 operator*(const Gen3& ai,float b) LINEARMAKE1(3,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen4 operator*(const Gen4& ai,float b) LINEARMAKE1(4,a*b,b*da,"("+ai.mountable+"*"+tostring(b)+")")
Gen2 operator*(const Gen2& ai,const Gen& bi) {return bi*ai;}
Gen3 operator*(const Gen3& ai,const Gen& bi) {return bi*ai;}
Gen4 operator*(const Gen4& ai,const Gen& bi) {return bi*ai;}
Gen2 operator*(vec2 b,const Gen& ai) {return ai*b;}
Gen3 operator*(vec3 b,const Gen& ai) {return ai*b;}
Gen4 operator*(vec4 b,const Gen& ai) {return ai*b;}
Gen2 operator*(float b,const Gen2& ai) {return ai*b;}
Gen3 operator*(float b,const Gen3& ai) {return ai*b;}
Gen4 operator*(float b,const Gen4& ai) {return ai*b;}
Gen  operator/(const Gen&  ai,const Gen& bi) GENMAKE2(1,a/b,(da*b-db*a)/(b*b),"("+ai.mountable+"/"+bi.mountable+")")
Gen2 operator/(const Gen2& ai,const Gen& bi) GENMAKE2(2,a/b,(da*b-db*a)/(b*b),"("+ai.mountable+"/"+bi.mountable+")")
Gen3 operator/(const Gen3& ai,const Gen& bi) GENMAKE2(3,a/b,(da*b-db*a)/(b*b),"("+ai.mountable+"/"+bi.mountable+")")
Gen4 operator/(const Gen4& ai,const Gen& bi) GENMAKE2(4,a/b,(da*b-db*a)/(b*b),"("+ai.mountable+"/"+bi.mountable+")")
Gen  operator/(float b,const Gen& ai) GENMAKE1(1,a/b,-da*b/(a*a),"("+tostring(b)+"/"+ai.mountable+")")
Gen2 operator/(vec2  b,const Gen& ai) GENMAKE1(2,a/b,-da*b/(a*a),"("+tostring(b)+"/"+ai.mountable+")")
Gen3 operator/(vec3  b,const Gen& ai) GENMAKE1(3,a/b,-da*b/(a*a),"("+tostring(b)+"/"+ai.mountable+")")
Gen4 operator/(vec4  b,const Gen& ai) GENMAKE1(4,a/b,-da*b/(a*a),"("+tostring(b)+"/"+ai.mountable+")")
Gen  operator/(const Gen&  ai,float b) {return ai*(1/b);}
Gen2 operator/(const Gen2& ai,float b) {return ai*(1/b);}
Gen3 operator/(const Gen3& ai,float b) {return ai*(1/b);}
Gen4 operator/(const Gen4& ai,float b) {return ai*(1/b);}
Gen operator*(const Gen2& ai,const Gen2& bi) GENMAKE2(1,dot(a,b),dot(a,db)+dot(b,da),"dot("+ai.mountable+","+bi.mountable+")")
Gen operator*(const Gen3& ai,const Gen3& bi) GENMAKE2(1,dot(a,b),dot(a,db)+dot(b,da),"dot("+ai.mountable+","+bi.mountable+")")
Gen operator*(const Gen4& ai,const Gen4& bi) GENMAKE2(1,dot(a,b),dot(a,db)+dot(b,da),"dot("+ai.mountable+","+bi.mountable+")")
Gen operator*(const Gen2& ai,vec2 b) LINEARMAKE1(1,dot(a,b),dot(b,da),"dot("+ai.mountable+","+tostring(b)+")")
Gen operator*(const Gen3& ai,vec3 b) LINEARMAKE1(1,dot(a,b),dot(b,da),"dot("+ai.mountable+","+tostring(b)+")")
Gen operator*(const Gen4& ai,vec4 b) LINEARMAKE1(1,dot(a,b),dot(b,da),"dot("+ai.mountable+","+tostring(b)+")")
Gen operator*(vec2 b,const Gen2& ai) {return ai*b;}
Gen operator*(vec3 b,const Gen3& ai) {return ai*b;}
Gen operator*(vec4 b,const Gen4& ai) {return ai*b;}

void GenProgram::set(string id,const Gen& g) {
    id1.push_back(id);
    int z = (int)shrak1.size();
    shrak1.push_back({0,0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak1[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak1[z] = {g.func(e),g.deriv(e)};};
    premount = premount+"float "+id+" = "+g.mountable+";\n";
}
void GenProgram::set2(string id,const Gen2& g) {
    id2.push_back(id);
    int z = (int)shrak2.size();
    shrak2.push_back({vec2(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak2[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak2[z] = {g.func(e),g.deriv(e)};};
    premount = premount+"float2 "+id+" = "+g.mountable+";\n";
}
void GenProgram::set3(string id,const Gen3& g) {
    id3.push_back(id);
    int z = (int)shrak3.size();
    shrak3.push_back({vec3(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak3[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak3[z] = {g.func(e),g.deriv(e)};};
    premount = premount+"float3 "+id+" = "+g.mountable+";\n";
}
void GenProgram::set4(string id,const Gen4& g) {
    id4.push_back(id);
    int z = (int)shrak4.size();
    shrak4.push_back({vec4(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak4[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak4[z] = {g.func(e),g.deriv(e)};};
    premount = premount+"float4 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setconst(string id,const Gen& g) {
    id1.push_back(id);
    int z = (int)shrak1.size();
    shrak1.push_back({0,0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak1[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak1[z] = {g.func(e),0};};
    premount = premount+"float "+id+" = "+g.mountable+";\n";
}
void GenProgram::setconst2(string id,const Gen2& g) {
    id2.push_back(id);
    int z = (int)shrak2.size();
    shrak2.push_back({vec2(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak2[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak2[z] = {g.func(e),0};};
    premount = premount+"float2 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setconst3(string id,const Gen3& g) {
    id3.push_back(id);
    int z = (int)shrak3.size();
    shrak3.push_back({vec3(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak3[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak3[z] = {g.func(e),0};};
    premount = premount+"float3 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setconst4(string id,const Gen4& g) {
    id4.push_back(id);
    int z = (int)shrak4.size();
    shrak4.push_back({vec4(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak4[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak4[z] = {g.func(e),0};};
    premount = premount+"float4 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setlinear(string id,const Gen& g) {
    id1.push_back(id);
    int z = (int)shrak1.size();
    shrak1.push_back({0,0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak1[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak1[z] = {0,g.deriv(e)};};
    premount = premount+"float "+id+" = "+g.mountable+";\n";
}
void GenProgram::setlinear2(string id,const Gen2& g) {
    id2.push_back(id);
    int z = (int)shrak2.size();
    shrak2.push_back({vec2(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak2[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak2[z] = {vec2(0),g.deriv(e)};};
    premount = premount+"float2 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setlinear3(string id,const Gen3& g) {
    id3.push_back(id);
    int z = (int)shrak3.size();
    shrak3.push_back({vec3(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak3[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak3[z] = {vec3(0),g.deriv(e)};};
    premount = premount+"float3 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setlinear4(string id,const Gen4& g) {
    id4.push_back(id);
    int z = (int)shrak4.size();
    shrak4.push_back({vec4(0),0});
    function<void(vec3)> pf = prefunc;
    function<void(vec3)> pd = prederiv;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak4[z] = {g.func(e),0};};
    prederiv = [=](vec3 e)throw()->void{pd(e);shrak4[z] = {vec4(0),g.deriv(e)};};
    premount = premount+"float4 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setfunconly(string id,const Gen& g) {
    id1.push_back(id);
    int z = (int)shrak1.size();
    shrak1.push_back({0,0});
    function<void(vec3)> pf = prefunc;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak1[z] = {g.func(e),0};};
    premount = premount+"float "+id+" = "+g.mountable+";\n";
}
void GenProgram::setfunconly2(string id,const Gen2& g) {
    id2.push_back(id);
    int z = (int)shrak2.size();
    shrak2.push_back({vec2(0),0});
    function<void(vec3)> pf = prefunc;
    prefunc  = [=](vec3 e)throw()->void{pf(e); shrak2[z] = {g.func(e),0};};
    premount = premount+"float2 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setfunconly3(string id,const Gen3& g) {
    id3.push_back(id);
    int z = (int)shrak3.size();
    shrak3.push_back({vec3(0),0});
    function<void(vec3)> pf = prefunc;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak3[z] = {g.func(e),0};};
    premount = premount+"float3 "+id+" = "+g.mountable+";\n";
}
void GenProgram::setfunconly4(string id,const Gen4& g) {
    id4.push_back(id);
    int z = (int)shrak4.size();
    shrak4.push_back({vec4(0),0});
    function<void(vec3)> pf = prefunc;
    prefunc  = [=](vec3 e)throw()->void{pf(e);shrak4[z] = {g.func(e),0};};
    premount = premount+"float4 "+id+" = "+g.mountable+";\n";
}
Gen GenProgram::get(string id) {
    for (int i=(int)id1.size()-1;i>=0;i--) if (id1[i]==id) {
        return Gen(FUNC(1){return shrak1[i].first;},DERIV(1){return shrak1[i].second;},id);
    }
    if (id=="x") {return Gen(FUNC(1){return e.x;},DERIV(1){return Triplet<float>(1,0,0);},"xyz.x");}
    if (id=="y") {return Gen(FUNC(1){return e.y;},DERIV(1){return Triplet<float>(0,1,0);},"xyz.y");}
    if (id=="z") {return Gen(FUNC(1){return e.z;},DERIV(1){return Triplet<float>(0,0,1);},"xyz.z");}
    throw;
}
Gen2 GenProgram::get2(string id) {
    for (int i=(int)id2.size()-1;i>=0;i--) if (id2[i]==id) {
        return Gen2(FUNC(2){return shrak2[i].first;},DERIV(2){return shrak2[i].second;},id);
    }
    throw;
}
Gen3 GenProgram::get3(string id) {

    for (int i=(int)id3.size()-1;i>=0;i--) if (id3[i]==id) {
        return Gen3(FUNC(3){return shrak3[i].first;},DERIV(3){return shrak3[i].second;},id);
    }
    if (id=="xyz") {return Gen3(FUNC(3){return e;},DERIV(3){return Triplet<vec3>(vec3(1,0,0),vec3(0,1,0),vec3(0,0,1));},"xyz");}
    throw;
}
Gen4 GenProgram::get4(string id) {
    for (int i=(int)id4.size()-1;i>=0;i--) if (id4[i]==id) {
        return Gen4(FUNC(4){return shrak4[i].first;},DERIV(4){return shrak4[i].second;},id);
    }
    throw;
}


Gen GenProgram::get(int rz,string id,char x) {
    if (x=='x') x=0; if (x=='y') x=1; if (x=='z') x=2; if (x=='w') x=3;
    if (x=='X') x=0; if (x=='Y') x=1; if (x=='Z') x=2; if (x=='W') x=3;
    std::string postfix=".";
    if (x==0) postfix+="x";if (x==1) postfix+="y";if (x==2) postfix+="z";if (x==3) postfix+="w";
    switch (rz) {
        case 1: for (int i=(int)id1.size()-1;i>=0;i--) if (id1[i]==id) {
            return Gen(FUNC(1){return shrak1[i].first;},DERIV(1){return shrak1[i].second;},id+postfix);
        } break;
        case 2: for (int i=(int)id2.size()-1;i>=0;i--) if (id2[i]==id) {
            return Gen(FUNC(1){return amtacc(x,shrak2[i].first);},DERIV(1){return tripletacc(x,shrak2[i].second);},id+postfix);
        } break;
        case 3: for (int i=(int)id3.size()-1;i>=0;i--) if (id3[i]==id) {
            return Gen(FUNC(1){return amtacc(x,shrak3[i].first);},DERIV(1){return tripletacc(x,shrak3[i].second);},id+postfix);
        } break;
        case 4: for (int i=(int)id4.size()-1;i>=0;i--) if (id4[i]==id) {
            return Gen(FUNC(1){return amtacc(x,shrak4[i].first);},DERIV(1){return tripletacc(x,shrak4[i].second);},id+postfix);
        } break;
    }
    throw;
}
Gen2 GenProgram::get2(int rz,string id,char x,char y)  {
    if (x=='x') x=0; if (x=='y') x=1; if (x=='z') x=2; if (x=='w') x=3;
    if (x=='X') x=0; if (x=='Y') x=1; if (x=='Z') x=2; if (x=='W') x=3;
    if (y=='x') y=0; if (y=='y') y=1; if (y=='z') y=2; if (y=='w') y=3;
    if (y=='X') y=0; if (y=='Y') y=1; if (y=='Z') y=2; if (y=='W') y=3;
    std::string postfix=".";
    if (x==0) postfix+="x";if (x==1) postfix+="y";if (x==2) postfix+="z";if (x==3) postfix+="w";
    if (y==0) postfix+="x";if (y==1) postfix+="y";if (y==2) postfix+="z";if (y==3) postfix+="w";
    switch (rz) {
        case 1: for (int i=(int)id1.size()-1;i>=0;i--) if (id1[i]==id) {
            return Gen2(FUNC(2){return vec2(shrak1[i].first);},DERIV(2){return tripletassemble(shrak1[i].second,shrak1[i].second);},id+postfix);
        } break;
        case 2: for (int i=(int)id2.size()-1;i>=0;i--) if (id2[i]==id) {
            return Gen2(FUNC(2){return vec2(amtacc(x,shrak2[i].first),amtacc(y,shrak2[i].first));},DERIV(2){return tripletassemble(tripletacc(x,shrak2[i].second),tripletacc(y,shrak2[i].second));},id+postfix);
        } break;
        case 3: for (int i=(int)id3.size()-1;i>=0;i--) if (id3[i]==id) {
            return Gen2(FUNC(2){return vec2(amtacc(x,shrak3[i].first),amtacc(y,shrak3[i].first));},DERIV(2){return tripletassemble(tripletacc(x,shrak3[i].second),tripletacc(y,shrak3[i].second));},id+postfix);
        } break;
        case 4: for (int i=(int)id4.size()-1;i>=0;i--) if (id4[i]==id) {
            return Gen2(FUNC(2){return vec2(amtacc(x,shrak4[i].first),amtacc(y,shrak4[i].first));},DERIV(2){return tripletassemble(tripletacc(x,shrak4[i].second),tripletacc(y,shrak4[i].second));},id+postfix);
        } break;
    }
    throw;
}
Gen3 GenProgram::get3(int rz,string id,char x,char y,char z)  {
    if (x=='x') x=0; if (x=='y') x=1; if (x=='z') x=2; if (x=='w') x=3;
    if (x=='X') x=0; if (x=='Y') x=1; if (x=='Z') x=2; if (x=='W') x=3;
    if (y=='x') y=0; if (y=='y') y=1; if (y=='z') y=2; if (y=='w') y=3;
    if (y=='X') y=0; if (y=='Y') y=1; if (y=='Z') y=2; if (y=='W') y=3;
    if (z=='x') z=0; if (z=='y') z=1; if (z=='z') z=2; if (z=='w') z=3;
    if (z=='X') z=0; if (z=='Y') z=1; if (z=='Z') z=2; if (z=='W') z=3;
    std::string postfix=".";
    if (x==0) postfix+="x";if (x==1) postfix+="y";if (x==2) postfix+="z";if (x==3) postfix+="w";
    if (y==0) postfix+="x";if (y==1) postfix+="y";if (y==2) postfix+="z";if (y==3) postfix+="w";
    if (z==0) postfix+="x";if (z==1) postfix+="y";if (z==2) postfix+="z";if (z==3) postfix+="w";
    switch (rz) {
        case 1: for (int i=(int)id1.size()-1;i>=0;i--) if (id1[i]==id) {
            return Gen3(FUNC(3){return vec3(shrak1[i].first);},DERIV(3){return tripletassemble(shrak1[i].second,shrak1[i].second,shrak1[i].second);},id+postfix);
        } break;
        case 2: for (int i=(int)id2.size()-1;i>=0;i--) if (id2[i]==id) {
            return Gen3(FUNC(3){return vec3(amtacc(x,shrak2[i].first),amtacc(y,shrak2[i].first),amtacc(z,shrak2[i].first));},DERIV(3){return tripletassemble(tripletacc(x,shrak2[i].second),tripletacc(y,shrak2[i].second),tripletacc(z,shrak2[i].second));},id+postfix);
        } break;
        case 3: for (int i=(int)id3.size()-1;i>=0;i--) if (id3[i]==id) {
            return Gen3(FUNC(3){return vec3(amtacc(x,shrak3[i].first),amtacc(y,shrak3[i].first),amtacc(z,shrak3[i].first));},DERIV(3){return tripletassemble(tripletacc(x,shrak3[i].second),tripletacc(y,shrak3[i].second),tripletacc(z,shrak3[i].second));},id+postfix);
        } break;
        case 4: for (int i=(int)id4.size()-1;i>=0;i--) if (id4[i]==id) {
            return Gen3(FUNC(3){return vec3(amtacc(x,shrak3[i].first),amtacc(y,shrak3[i].first),amtacc(z,shrak3[i].first));},DERIV(3){return tripletassemble(tripletacc(x,shrak3[i].second),tripletacc(y,shrak3[i].second),tripletacc(z,shrak3[i].second));},id+postfix);
        } break;
    }
    throw;
}
Gen4 GenProgram::get4(int rz,string id,char x,char y,char z,char w)  {
    if (x=='x') x=0; if (x=='y') x=1; if (x=='z') x=2; if (x=='w') x=3;
    if (x=='X') x=0; if (x=='Y') x=1; if (x=='Z') x=2; if (x=='W') x=3;
    if (y=='x') y=0; if (y=='y') y=1; if (y=='z') y=2; if (y=='w') y=3;
    if (y=='X') y=0; if (y=='Y') y=1; if (y=='Z') y=2; if (y=='W') y=3;
    if (z=='x') z=0; if (z=='y') z=1; if (z=='z') z=2; if (z=='w') z=3;
    if (z=='X') z=0; if (z=='Y') z=1; if (z=='Z') z=2; if (z=='W') z=3;
    if (w=='x') w=0; if (w=='y') w=1; if (w=='z') w=2; if (w=='w') w=3;
    if (w=='X') w=0; if (w=='Y') w=1; if (w=='Z') w=2; if (w=='W') w=3;
    std::string postfix=".";
    if (x==0) postfix+="x";if (x==1) postfix+="y";if (x==2) postfix+="z";if (x==3) postfix+="w";
    if (y==0) postfix+="x";if (y==1) postfix+="y";if (y==2) postfix+="z";if (y==3) postfix+="w";
    if (z==0) postfix+="x";if (z==1) postfix+="y";if (z==2) postfix+="z";if (z==3) postfix+="w";
    if (w==0) postfix+="x";if (w==1) postfix+="y";if (w==2) postfix+="z";if (w==3) postfix+="w";
    switch (rz) {
        case 1: for (int i=(int)id1.size()-1;i>=0;i--) if (id1[i]==id) {
            return Gen4(FUNC(4){return vec4(shrak1[i].first);},DERIV(4){return tripletassemble(shrak1[i].second,shrak1[i].second,shrak1[i].second,shrak1[i].second);},id+postfix);
        } break;
        case 2: for (int i=(int)id2.size()-1;i>=0;i--) if (id2[i]==id) {
            return Gen4(FUNC(4){return vec4(amtacc(x,shrak2[i].first),amtacc(y,shrak2[i].first),amtacc(z,shrak2[i].first),amtacc(w,shrak2[i].first));},DERIV(4){return tripletassemble(tripletacc(x,shrak2[i].second),tripletacc(y,shrak2[i].second),tripletacc(z,shrak2[i].second),tripletacc(w,shrak2[i].second));},id+postfix);
        } break;
        case 3: for (int i=(int)id3.size()-1;i>=0;i--) if (id3[i]==id) {
            return Gen4(FUNC(4){return vec4(amtacc(x,shrak3[i].first),amtacc(y,shrak3[i].first),amtacc(z,shrak3[i].first),amtacc(w,shrak3[i].first));},DERIV(4){return tripletassemble(tripletacc(x,shrak3[i].second),tripletacc(y,shrak3[i].second),tripletacc(z,shrak3[i].second),tripletacc(w,shrak3[i].second));},id+postfix);
        } break;
        case 4: for (int i=(int)id4.size()-1;i>=0;i--) if (id4[i]==id) {
            return Gen4(FUNC(4){return vec4(amtacc(x,shrak4[i].first),amtacc(y,shrak4[i].first),amtacc(z,shrak4[i].first),amtacc(w,shrak4[i].first));},DERIV(4){return tripletassemble(tripletacc(x,shrak4[i].second),tripletacc(y,shrak4[i].second),tripletacc(z,shrak4[i].second),tripletacc(w,shrak4[i].second));},id+postfix);
        } break;
    }
    throw;
}
void GenProgram::mount(const Gen& g) {
    string prefix =
    "__kernel void square(\n"
    "__global float* output,\n"
    "const int xc,\n"
    "const int yc,\n"
    "const int zc){\n"
    "long combined = get_global_id(0)+get_global_id(1)*CHSIZE+get_global_id(2)*CHSIZE*CHSIZE;\n"
    "int xwi = combined%(CHSIZE+1);\n"
    "int ywi = (combined/(CHSIZE+1))%(CHSIZE+1);\n"
    "int zwi = combined/((CHSIZE+1)*(CHSIZE+1));\n"
    "if (combined<(CHSIZE+1)*(CHSIZE+1)*(CHSIZE+1))\n{"
    "float3 xyz = (float3)(xc+xwi,yc+ywi,zc+zwi);\n";
    string postfix = "}}";
    mounted = Gen(FUNC(1){prefunc(e);return g.func(e);},DERIV(1){prederiv(e);return g.deriv(e);},
    prefix+premount+"output[combined] = "+g.mountable+";\n"+postfix);
}
vec3 GenProgram::samplenormal(float x,float y,float z) {
    Triplet<float> ag = mounted.deriv(glm::vec3(x,y,z));
    return normalize(vec3(ag.x,ag.y,ag.z));
}
float GenProgram::sample(float x,float y,float z) {
    return mounted.func(glm::vec3(x,y,z));
}

Triplet<float> tripletacc(int c,const Triplet<vec2>& r) {
    if (c==0) return Triplet<float>(r.x.x,r.y.x,r.z.x);
    if (c==1) return Triplet<float>(r.x.y,r.y.y,r.z.y);
    throw;
}
Triplet<float> tripletacc(int c,const Triplet<vec3>& r) {
    if (c==0) return Triplet<float>(r.x.x,r.y.x,r.z.x);
    if (c==1) return Triplet<float>(r.x.y,r.y.y,r.z.y);
    if (c==2) return Triplet<float>(r.x.z,r.y.z,r.z.z);
    throw;
}
Triplet<float> tripletacc(int c,const Triplet<vec4>& r) {
    if (c==0) return Triplet<float>(r.x.x,r.y.x,r.z.x);
    if (c==1) return Triplet<float>(r.x.y,r.y.y,r.z.y);
    if (c==2) return Triplet<float>(r.x.z,r.y.z,r.z.z);
    if (c==3) return Triplet<float>(r.x.w,r.y.w,r.z.w);
    throw;
}
float amtacc(int c,const vec2& r) {
    if (c==0) return r.x;
    if (c==1) return r.y;
    throw;
}
float amtacc(int c,const vec3& r) {
    if (c==0) return r.x;
    if (c==1) return r.y;
    if (c==2) return r.z;
    throw;
}
float amtacc(int c,const vec4& r) {
    if (c==0) return r.x;
    if (c==1) return r.y;
    if (c==2) return r.z;
    if (c==3) return r.w;
    throw;
}

Triplet<vec2> tripletassemble(Triplet<float> x,Triplet<float> y) {
    return Triplet<vec2>(vec2(x.x,y.x),vec2(x.y,y.y),vec2(x.z,y.z));
}
Triplet<vec3> tripletassemble(Triplet<float> x,Triplet<float> y,Triplet<float> z) {
    return Triplet<vec3>(vec3(x.x,y.x,z.x),vec3(x.y,y.y,z.y),vec3(x.z,y.z,z.z));
}
Triplet<vec4> tripletassemble(Triplet<float> x,Triplet<float> y,Triplet<float> z,Triplet<float> w) {
    return Triplet<vec4>(vec4(x.x,y.x,z.x,w.x),vec4(x.y,y.y,z.y,w.y),vec4(x.z,y.z,z.z,w.z));
}

GenProgram::GenProgram() : mounted(0), prederiv([=](vec3 e)throw()->void{}), prefunc([=](vec3 e)throw()->void{}) {}

#define ASSERT(b) {int err=b;if (err) {std::cout<<"FATAL ERROR: "<<err<<"\n";throw;}}
#define ASSERT_ASSIGN(decl,exp) {int err=0;decl=exp;if (err or !decl) throw;}


#endif


Sampler::~Sampler() {
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
}





