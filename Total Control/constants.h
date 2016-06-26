//
//  constants.h
//  Total Control
//
//  Created by Parker on 3/8/16.
//  Copyright © 2016 Parker Lawrence. All rights reserved.
//

#ifndef constants_h
#define constants_h

#define RANDPOWER 6 //size of sampling arrays used by generator
#define RANDSIZE (1<<RANDPOWER)

#define CHPOWER  7 //size of portions loaded into voxel memory
#define CHSIZE (1<<CHPOWER)

#define GEOPOWER 7 //size of geometry batches
#define GEOSIZE (1<<GEOPOWER)

#define SCALE 1 //distance between voxels

#define SVD_NUM_SWEEPS 5

#include <stdint.h>
#include <climits>

typedef uint8_t BlockId;
typedef int BlockLoc;
typedef float QefPerc;

#define BLOCKLOCFULLMASK INT_MAX
#define ALTERNATOR (((BLOCKLOCFULLMASK/3)<<1)|1)
#define ALTALTERNATOR (ALTERNATOR<<1)


//leave this alone
#define ASBLOCKLOC(f) f+(ALTERNATOR-(ALTERNATOR&(CHSIZE-1)))

#if CHPOWER%2 == 0
#define ASCHUNKLOC(f) ((f+ALTERNATOR)<<CHPOWER)
#else
#define ASCHUNKLOC(f) ((f+ALTALTERNATOR)<<CHPOWER)
#endif


inline int MOD(int n,int m) {
    return ((n%m)+m)%m;
}
inline int TRUNC_DIV(int n, int m) {
    return (n-MOD(n,m))/m;
}
#define WIREFRAMEDEBUG


#endif /* constants_h */