/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include <stdio.h>
#include "decode.hpp"
#define BASE_CHANNEL_INDEX i*cols*channels+j*channels

void recover_frame(Mat &max, Mat &min, Mat& frame, signed char *c, int rows, int cols, int channels){
	int i=0,j=0;
	frame.create(rows, cols, CV_8UC3);
	uint8_t *max_data = (uint8_t*)max.data;
	uint8_t *min_data = (uint8_t*)min.data;
	#pragma parallel for
	for(i=0; i<rows; i++){
		#pragma parallel for
		for(j=0; j<cols; j++){
			if(c[BASE_CHANNEL_INDEX]<0){
				if(c[BASE_CHANNEL_INDEX] == -128){
					frame.data[BASE_CHANNEL_INDEX] = max_data[BASE_CHANNEL_INDEX];
				}
				else{
					frame.data[BASE_CHANNEL_INDEX] = max_data[BASE_CHANNEL_INDEX] + c[BASE_CHANNEL_INDEX];
				}
			}
			else{
				frame.data[BASE_CHANNEL_INDEX] = min_data[BASE_CHANNEL_INDEX] + c[BASE_CHANNEL_INDEX];
			}
			if(c[BASE_CHANNEL_INDEX+1]<0){
				if(c[BASE_CHANNEL_INDEX+1] == -128){
					frame.data[BASE_CHANNEL_INDEX+1] = max_data[BASE_CHANNEL_INDEX+1];
				}
				else{
					frame.data[BASE_CHANNEL_INDEX+1] = max_data[BASE_CHANNEL_INDEX+1] + c[BASE_CHANNEL_INDEX+1];
				}
			}
			else{
				frame.data[BASE_CHANNEL_INDEX+1] = min_data[BASE_CHANNEL_INDEX+1] + c[BASE_CHANNEL_INDEX+1];
			}

			if(c[BASE_CHANNEL_INDEX+2]<0){
				if(c[BASE_CHANNEL_INDEX+2] == -128){
					frame.data[BASE_CHANNEL_INDEX+2] = max_data[BASE_CHANNEL_INDEX+2];
				}
				else{
					frame.data[BASE_CHANNEL_INDEX+2] = max_data[BASE_CHANNEL_INDEX+2] + c[BASE_CHANNEL_INDEX+2];
				}
			}
			else{
				frame.data[BASE_CHANNEL_INDEX+2] = min_data[BASE_CHANNEL_INDEX+2] + c[BASE_CHANNEL_INDEX+2];
			}
		}
	}
}

