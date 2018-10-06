//
//  samplers.hpp
//  Total Control
//
//  Created by Parker on 3/30/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef samplers_hpp
#define samplers_hpp

#include <stdio.h>
#include <octree.h>
#include <vox.h>
#include "constants.h"
class Structure;




template <typename T> struct Triplet{
    T x,y,z;
    Triplet(T a,T b,T c):x(a),y(b),z(c){}
    Triplet(float f):x(f),y(f),z(f){}
};
Triplet<glm::vec2> tripletassemble(Triplet<float>,Triplet<float>);
Triplet<glm::vec3> tripletassemble(Triplet<float>,Triplet<float>,Triplet<float>);
Triplet<glm::vec4> tripletassemble(Triplet<float>,Triplet<float>,Triplet<float>,Triplet<float>);

float amtacc(int,const glm::vec2&);
float amtacc(int,const glm::vec3&);
float amtacc(int,const glm::vec4&);
Triplet<float> tripletacc(int,const Triplet<glm::vec2>&);
Triplet<float> tripletacc(int,const Triplet<glm::vec3>&);
Triplet<float> tripletacc(int,const Triplet<glm::vec4>&);

struct Gen {
    std::function<float(glm::vec3)> func;
    std::function<Triplet<float>(glm::vec3)> deriv;
    std::string mountable;
    Gen(std::function<float(glm::vec3)>,std::function<Triplet<float>(glm::vec3)>,std::string);
    explicit Gen(float);
};
struct Gen2 {
    std::function<glm::vec2(glm::vec3)> func;
    std::function<Triplet<glm::vec2>(glm::vec3)> deriv;
    std::string mountable;
    Gen2(std::function<glm::vec2(glm::vec3)>,std::function<Triplet<glm::vec2>(glm::vec3)>,std::string);
    Gen2(Gen,Gen);
    explicit Gen2(glm::vec2);
    Gen2(float,float);
    Gen x();Gen y();
};
struct Gen3 {
    std::function<glm::vec3(glm::vec3)> func;
    std::function<Triplet<glm::vec3>(glm::vec3)> deriv;
    std::string mountable;
    Gen3(std::function<glm::vec3(glm::vec3)>,std::function<Triplet<glm::vec3>(glm::vec3)>,std::string);
    Gen3(Gen,Gen,Gen);
    explicit Gen3(glm::vec3);
    Gen3(float,float,float);
    Gen x();Gen y();Gen z();
};
struct Gen4 {
    std::function<glm::vec4(glm::vec3)> func;
    std::function<Triplet<glm::vec4>(glm::vec3)> deriv;
    std::string mountable;
    Gen4(std::function<glm::vec4(glm::vec3)>,std::function<Triplet<glm::vec4>(glm::vec3)>,std::string);
    Gen4(Gen,Gen,Gen,Gen);
    explicit Gen4(glm::vec4);
    Gen4(float,float,float,float);
    Gen x();Gen y();Gen z();Gen w();
};
Gen sin(const Gen&);Gen cos(const Gen&);
Gen tan(const Gen&);Gen cot(const Gen&);
Gen csc(const Gen&);Gen sec(const Gen&);
Gen ln(const Gen&);
Gen pow(const Gen&);
Gen pow(const Gen&,float);
Gen pow(float,const Gen&);
Gen pow(const Gen&,const Gen&);
Gen floor(const Gen&);
Gen2 floor(const Gen2&);
Gen3 floor(const Gen3&);
Gen4 floor(const Gen4&);
Gen ceil(const Gen&);
Gen2 ceil(const Gen2&);
Gen3 ceil(const Gen3&);
Gen4 ceil(const Gen4&);
Gen fract(const Gen&);
Gen2 fract(const Gen2&);
Gen3 fract(const Gen3&);
Gen4 fract(const Gen4&);
Gen3 cross(const Gen3&,const Gen3&);
Gen magnitude(const Gen2&);
Gen magnitude(const Gen3&);
Gen magnitude(const Gen4&);
Gen squaredmagnitude(const Gen2&);
Gen squaredmagnitude(const Gen3&);
Gen squaredmagnitude(const Gen4&);
Gen2 normalize(const Gen2&);
Gen3 normalize(const Gen3&);
Gen4 normalize(const Gen4&);

Gen max(const Gen&,const Gen&);
Gen2 max(const Gen2&,const Gen2&);
Gen3 max(const Gen3&,const Gen3&);
Gen4 max(const Gen4&,const Gen4&);
Gen min(const Gen&,const Gen&);
Gen2 min(const Gen2&,const Gen2&);
Gen3 min(const Gen3&,const Gen3&);
Gen4 min(const Gen4&,const Gen4&);
Gen abs(const Gen&);
Gen2 abs(const Gen2&);
Gen3 abs(const Gen3&);
Gen4 abs(const Gen4&);

Gen  operator+(const Gen& , const Gen&);
Gen2 operator+(const Gen2&, const Gen2&);
Gen3 operator+(const Gen3&, const Gen3&);
Gen4 operator+(const Gen4&, const Gen4&);
Gen  operator+(const Gen& , float);
Gen2 operator+(const Gen2&, glm::vec2);
Gen3 operator+(const Gen3&, glm::vec3);
Gen4 operator+(const Gen4&, glm::vec4);
Gen  operator+(float     ,const Gen&);
Gen2 operator+(glm::vec2  ,const Gen2&);
Gen3 operator+(glm::vec3  ,const Gen3&);
Gen4 operator+(glm::vec4  ,const Gen4&);

Gen  operator-(const Gen& , const Gen&);
Gen2 operator-(const Gen2&, const Gen2&);
Gen3 operator-(const Gen3&, const Gen3&);
Gen4 operator-(const Gen4&, const Gen4&);
Gen  operator-(const Gen& , float);
Gen2 operator-(const Gen2&, glm::vec2);
Gen3 operator-(const Gen3&, glm::vec3);
Gen4 operator-(const Gen4&, glm::vec4);
Gen  operator-(float     ,const Gen&);
Gen2 operator-(glm::vec2  ,const Gen2&);
Gen3 operator-(glm::vec3  ,const Gen3&);
Gen4 operator-(glm::vec4  ,const Gen4&);

Gen operator*(const Gen&,const Gen&);
Gen operator*(float,const Gen&);
Gen operator*(const Gen&,float);
Gen operator/(const Gen&,const Gen&);
Gen operator/(float,const Gen&);
Gen operator/(const Gen&,float);

Gen2 operator*(const Gen&,const Gen2&);
Gen3 operator*(const Gen&,const Gen3&);
Gen4 operator*(const Gen&,const Gen4&);
Gen2 operator*(const Gen&,glm::vec2);
Gen3 operator*(const Gen&,glm::vec3);
Gen4 operator*(const Gen&,glm::vec4);
Gen2 operator*(float,const Gen2&);
Gen3 operator*(float,const Gen3&);
Gen4 operator*(float,const Gen4&);
Gen2 operator*(const Gen2&,const Gen&);
Gen3 operator*(const Gen3&,const Gen&);
Gen4 operator*(const Gen4&,const Gen&);
Gen2 operator*(glm::vec2,const Gen&);
Gen3 operator*(glm::vec3,const Gen&);
Gen4 operator*(glm::vec4,const Gen&);
Gen2 operator*(const Gen2&,float);
Gen3 operator*(const Gen3&,float);
Gen4 operator*(const Gen4&,float);
Gen2 operator/(const Gen2&,const Gen&);
Gen3 operator/(const Gen3&,const Gen&);
Gen4 operator/(const Gen4&,const Gen&);
Gen2 operator/(const Gen2&,float);
Gen3 operator/(const Gen3&,float);
Gen4 operator/(const Gen4&,float);
Gen2 operator/(glm::vec2,const Gen&);
Gen3 operator/(glm::vec3,const Gen&);
Gen4 operator/(glm::vec4,const Gen&);

Gen operator*(const Gen2&,const Gen2&);
Gen operator*(const Gen3&,const Gen3&);
Gen operator*(const Gen4&,const Gen4&);
Gen operator*(const Gen2&,glm::vec2);
Gen operator*(const Gen3&,glm::vec3);
Gen operator*(const Gen4&,glm::vec4);
Gen operator*(glm::vec2,const Gen2&);
Gen operator*(glm::vec3,const Gen3&);
Gen operator*(glm::vec4,const Gen4&);


struct GenProgram{
    Gen mounted;
    GenProgram();
    std::function<void(glm::vec3)> prefunc;
    std::function<void(glm::vec3)> prederiv;
    std::string premount;
    std::vector<std::string> id1;
    std::vector<std::string> id2;
    std::vector<std::string> id3;
    std::vector<std::string> id4;
    std::vector<std::pair<float,Triplet<float>>>         shrak1;
    std::vector<std::pair<glm::vec2,Triplet<glm::vec2>>> shrak2;
    std::vector<std::pair<glm::vec3,Triplet<glm::vec3>>> shrak3;
    std::vector<std::pair<glm::vec4,Triplet<glm::vec4>>> shrak4;
    void set( std::string,const Gen& );
    void set2(std::string,const Gen2&);
    void set3(std::string,const Gen3&);
    void set4(std::string,const Gen4&);
    void setlinear( std::string,const Gen& );
    void setlinear2(std::string,const Gen2&);
    void setlinear3(std::string,const Gen3&);
    void setlinear4(std::string,const Gen4&);
    void setconst( std::string,const Gen& );
    void setconst2(std::string,const Gen2&);
    void setconst3(std::string,const Gen3&);
    void setconst4(std::string,const Gen4&);
    void setfunconly( std::string,const Gen& );
    void setfunconly2(std::string,const Gen2&);
    void setfunconly3(std::string,const Gen3&);
    void setfunconly4(std::string,const Gen4&);
    Gen  get(std::string);
    Gen2 get2(std::string);
    Gen3 get3(std::string);
    Gen4 get4(std::string);
    Gen  get(int,std::string,char);
    Gen2 get2(int,std::string,char,char);
    Gen3 get3(int,std::string,char,char,char);
    Gen4 get4(int,std::string,char,char,char,char);
    void mount(const Gen&);
    glm::vec3 samplenormal(float,float,float);
    float sample(float,float,float);
};



struct Sampler {
    GenProgram gp;
    cl_program program;
    cl_kernel kernel;
    cl_mem output;
    Sampler();
//    glm::vec3 normal(double,double,double,Structure&);
    void populate(int,int,int,Structure&);
    ~Sampler();
};

#endif /* samplers_hpp */
