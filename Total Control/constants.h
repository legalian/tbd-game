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

#endif /* constants_h */
