/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include<stdio.h>
#include<queue>
#include"../namespace/namespace.hpp"

#ifndef __PIXEL
#define __PIXEL
typedef struct pxl{
	uint8_t level;
	unsigned char value;
	pxl(){
		level = 0;
		value = 0;
	}
}pxl;
#endif

extern queue<signed char*> video_codec;
extern queue<unsigned char*> predictive_video_codec;
