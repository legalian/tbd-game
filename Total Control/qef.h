//
//  qef.hpp
//  Total Control
//
//  Created by Parker on 5/7/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef qef_hpp
#define qef_hpp

#include <stdio.h>
//int calculate_qef();

#include <math.h>
#include <glm/glm.hpp>
#include "constants.h"
//#include <glm/gtc/swizzle.hpp>
using namespace glm;

//#define abs fabs
//#define sqrt sqrtf
//#define max(x,y) (((x)>(y))?(x):(y))
#define IN(t,x) t x
#define OUT(t, x) t &x
#define INOUT(t, x) t &x

#define SWIZZLE_XYZ(v) vec3(v.x,v.y,v.z)


dvec3 svd_vmul_sym(IN(dmat3,a), IN(dvec3,v));
void svd_solve_ATA_ATb(
                       IN(dmat3,ATA), IN(dvec3,ATb), OUT(dvec3,x)
                       );
int test_qef();



#endif /* qef_hpp */
