/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include<stdio.h>
#include<assert.h>
#include<iostream>
#include<iterator>
#include<pthread.h>
#include<omp.h>
#include"encode.hpp"
#include"../common/common.hpp"
#include"highgui.h"
#include"../namespace/namespace.hpp"

#define max(a, b) ((a>b)? a:b)
#define min(a, b) ((a>b)? b:a)
#define BASE_CHANNEL_INDEX i*cols*channels+j*channels
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t pred_rwlock = PTHREAD_RWLOCK_INITIALIZER;

queue<signed char*> video_codec;
queue<unsigned char*> predictive_video_codec;

unsigned short N = 256;

//TODO: Handle cases where wrlock will return an error message
void add_codec(signed char *c){
	pthread_rwlock_wrlock(&rwlock);
	video_codec.push(c);
	pthread_rwlock_unlock(&rwlock);
}

//TODO: Handle cases where wrlock will return an error message
void pred_add_codec(unsigned char *c){
	pthread_rwlock_wrlock(&pred_rwlock);
	predictive_video_codec.push(c);
	pthread_rwlock_unlock(&pred_rwlock);
}

unsigned char delta_code(unsigned char pred_val, unsigned char frame_val){
	unsigned char steps = 0, up = pred_val, down = pred_val;
	while((up != frame_val) || (down != frame_val)){
		++up;
		++steps;
		if(up == frame_val){
			break;
		}
		--down;
		++steps;
		if(down == frame_val){
			break;
		}
	}
	return steps;
}

void channel_helper(uint8_t level, pxl **c, Mat &max, Mat &min, Mat &frame,
		int rows, int cols, int channels, int i, int j){
	uint8_t pred_pxl_b = 0, pred_pxl_g = 0, pred_pxl_r = 0;
	//TODO: create logic to assign level at pixel granularity
	(*c)[BASE_CHANNEL_INDEX].level = level;

	uint8_t *max_data = (uint8_t*)max.data;
	uint8_t *min_data = (uint8_t*)min.data;
	uint8_t *frame_data = (uint8_t*)frame.data;

	pred_pxl_b = min_data[BASE_CHANNEL_INDEX] + ((level/N) *
			max_data[BASE_CHANNEL_INDEX] - min_data[BASE_CHANNEL_INDEX]);
	(*c)[BASE_CHANNEL_INDEX].value = delta_code(pred_pxl_b, frame_data[BASE_CHANNEL_INDEX]);

	(*c)[BASE_CHANNEL_INDEX+1].level = level;
	pred_pxl_g = min_data[BASE_CHANNEL_INDEX+1] + ((level/N) *
			max_data[BASE_CHANNEL_INDEX+1] - min_data[BASE_CHANNEL_INDEX+1]);
	(*c)[BASE_CHANNEL_INDEX+1].value = delta_code(pred_pxl_g, frame_data[BASE_CHANNEL_INDEX+1]);

	(*c)[BASE_CHANNEL_INDEX+2].level = level;
	pred_pxl_r = min_data[BASE_CHANNEL_INDEX+2] + ((level/N) *
			max_data[BASE_CHANNEL_INDEX+2] - min_data[BASE_CHANNEL_INDEX+2]);
	(*c)[BASE_CHANNEL_INDEX+2].value = delta_code(pred_pxl_r, frame_data[BASE_CHANNEL_INDEX+2]);
}

void make_predictive_codec(Mat &max, Mat &min, Mat &frame, int rows, int cols, int channels){
	int i=0, j=0;
	pxl *c = new pxl[rows*cols*channels];
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
			short diff = 0;
			uint8_t pred_pxl_b=0, pred_pxl_g=0, pred_pxl_r=0;
			uint8_t level = 0;

			if(!i && !j){
				level = 128;
				channel_helper(level, &c, max, min, frame, rows, cols, channels, i, j);
			}
			else if((i-1) == -1){
				level = c[BASE_CHANNEL_INDEX-1].level;
				channel_helper(level, &c, max, min, frame, rows, cols, channels, i, j);
			}
			else if((j-1) == -1){
				level = c[BASE_CHANNEL_INDEX - (cols*channels)].level;
				channel_helper(level, &c, max, min, frame, rows, cols, channels, i, j);
			}
			else{
				uint8_t l_upr_lft = c[(BASE_CHANNEL_INDEX - (cols*channels))-1].level;
				uint8_t l_upr = c[BASE_CHANNEL_INDEX - (cols*channels)].level;
				uint8_t l_lft = c[BASE_CHANNEL_INDEX - 1].level;
				if(l_upr_lft >=  max(l_upr, l_lft)){
					level = min(l_upr, l_lft);
				}
				else if(l_upr_lft <= min(l_upr, l_lft)){
					level = max(l_upr, l_lft);
				}
				else{
					level = l_upr + l_lft - l_upr_lft;
				}
				channel_helper(level, &c, max, min, frame, rows, cols, channels, i, j);
			}
		}
	}
	//FIXME: Modify the function so that it doesn't require below loop. Convert pix struct to unsigned char\
	get rid of level value per pixle.
	unsigned char *cx = new unsigned char[rows*cols*channels];
	#pragma parallel for
	for(i=0; i<rows; i++){
		#pragma parallel for
		for(j=0; j<cols; j++){
			cx[BASE_CHANNEL_INDEX] = c[BASE_CHANNEL_INDEX].value;
			cx[BASE_CHANNEL_INDEX+1] = c[BASE_CHANNEL_INDEX+1].value;
			cx[BASE_CHANNEL_INDEX+2] = c[BASE_CHANNEL_INDEX+2].value;
		}
	}
	delete c;
	pred_add_codec(cx);
}

void make_codec(Mat &max, Mat &min, Mat &frame, int rows, int cols, int channels){
	int i=0,j=0;
	signed char *c = new signed char[rows*cols*channels];

	uint8_t *max_data = (uint8_t*)max.data;
	uint8_t *min_data = (uint8_t*)min.data;
	uint8_t *frame_data = (uint8_t*)frame.data;

	#pragma parallel for
	for(i=0; i<rows; i++){
		#pragma parallel for
		for(j=0; j<cols; j++){
			unsigned char b = max_data[BASE_CHANNEL_INDEX] - frame_data[BASE_CHANNEL_INDEX];
			unsigned char g = max_data[BASE_CHANNEL_INDEX+1] - frame_data[BASE_CHANNEL_INDEX+1];
			unsigned char r =	max_data[BASE_CHANNEL_INDEX+2] - frame_data[BASE_CHANNEL_INDEX+2];

			unsigned char b1 = frame_data[BASE_CHANNEL_INDEX] - min_data[BASE_CHANNEL_INDEX];
			unsigned char g1 = frame_data[BASE_CHANNEL_INDEX+1] - min_data[BASE_CHANNEL_INDEX+1];
			unsigned char r1 = frame_data[BASE_CHANNEL_INDEX+2] - min_data[BASE_CHANNEL_INDEX+2];

			if(b>=b1){
				c[BASE_CHANNEL_INDEX] = b1;
			}
			else if(b1>b){
				if(b==0){
					c[BASE_CHANNEL_INDEX] = -128;
				}
				else{
					c[BASE_CHANNEL_INDEX] = -b;
				}
			}
			if(g>=g1){
				c[BASE_CHANNEL_INDEX+1] = g1;
			}
			else if(g1>g){
				if(g==0){
					c[BASE_CHANNEL_INDEX+1] = -128;
				}
				else{
					c[BASE_CHANNEL_INDEX+1] = -g;
				}
			}
			if(r>=r1){
				c[BASE_CHANNEL_INDEX+2] = r1;
			}
			else if(r1>r){
				if(r==0){
					c[BASE_CHANNEL_INDEX+2] = -128;
				}
				else{
					c[BASE_CHANNEL_INDEX+2] = -r;
				}
			}
		}
	}
	add_codec(c);
}

void min_max_dif(Mat &max, Mat &min, list<Mat>&frame_list, int rows, int cols, int channels){
	unsigned int i=0,j=0;
	#pragma parallel for
	for(i=0; i<rows; i++){
		#pragma parallel for
		for(j=0; j<cols; j++){

			unsigned char min1=0, max1=0,min2=0, max2=0,min3=0, max3=0;

			list<Mat> :: iterator frame_it;
			frame_it = frame_list.begin();
			uint8_t *frame_data = (uint8_t*)(*frame_it).data;
			
			unsigned char b = frame_data[BASE_CHANNEL_INDEX];
			unsigned char g = frame_data[BASE_CHANNEL_INDEX+1];
			unsigned char r = frame_data[BASE_CHANNEL_INDEX+2];

			++frame_it;

			max1 = b; min1 = b; max2 = g; min2 = g; max3 = r; min3 = r;

			unsigned int frame_counter = 1;
			while((frame_counter < frame_list.size())){

				uint8_t tmin1 = 0, tmin2 = 0, tmin3 = 0, tmax1 = 0, tmax2 = 0, tmax3 = 0;
				uint8_t *frame_data = (uint8_t*)(*frame_it).data;
				unsigned char b = frame_data[BASE_CHANNEL_INDEX];
				unsigned char g = frame_data[BASE_CHANNEL_INDEX+1];
				unsigned char r = frame_data[BASE_CHANNEL_INDEX+2];
	
				frame_it++;			

				if(frame_it != frame_list.end()){
					
					uint8_t *frame_data1 = (uint8_t*)(*frame_it).data;
			
					unsigned char b1 = frame_data1[BASE_CHANNEL_INDEX];
					unsigned char g1 = frame_data1[BASE_CHANNEL_INDEX+1];
					unsigned char r1 = frame_data1[BASE_CHANNEL_INDEX+2];

					if(b>b1) { tmax1 = b; tmin1 = b1; } else { tmax1 = b1; tmin1 = b; }
					if(g>g1) { tmax2 = g; tmin2 = g1;} else { tmax2 = g1; tmin2 = g; }
					if(r>r1) { tmax3 = r; tmin3 = r1; } else { tmax3 = r1; tmin3 = r; }
				}
				else{
					tmax1 = b; tmin1 = b; tmax2 = g; tmin2 = g; tmax3 = r; tmin3 = r;
				}
			
				if(tmax1 > max1) { max1 = tmax1; }
				if(tmax2 > max2) { max2 = tmax2; }
				if(tmax3 > max3) { max3 = tmax3; }
				if(tmin1 < min1) { min1 = tmin1; }
				if(tmin2 < min2) { min2 = tmin2; }
				if(tmin3 < min3) { min3 = tmin3; }
				
			frame_it++;

			frame_counter +=2;
			}
			cout<<i<<" "<<j<<" "<<j+1<<" "<<j+2<<" "<<frame_counter<<endl;
			min.data[BASE_CHANNEL_INDEX] = min1;
			min.data[BASE_CHANNEL_INDEX+1] = min2;
			min.data[BASE_CHANNEL_INDEX+2] = min3;
			max.data[BASE_CHANNEL_INDEX] = max1;
			max.data[BASE_CHANNEL_INDEX+1] = max2;
			max.data[BASE_CHANNEL_INDEX+2] = max3;
		}
	}
}

void encode(list<Mat>&frame_list, Mat &max, Mat &min){
	list<Mat> :: iterator frame_list_itr;

	int frame_rows=-1, frame_cols=-1, frame_channels = -1;
	// Check has already been done; if it's not changed remove this condition in the release
	if(frame_list.empty()){
		return;
	}

	Mat first = frame_list.front();
	frame_rows = first.rows;
	frame_cols = first.cols;
	frame_channels = first.channels();

	max.create(frame_rows, frame_cols, CV_8UC3);
	min.create(frame_rows, frame_cols, CV_8UC3);

	min_max_dif(max, min, frame_list, frame_rows, frame_cols, frame_channels);


	unsigned long long frame_count = 0;
	#pragma parallel for
	for(frame_list_itr = frame_list.begin(); frame_list_itr != frame_list.end(); frame_list_itr++){
			//make_codec(max, min, (*frame_list_itr), frame_rows, frame_cols, frame_channels);
			make_predictive_codec(max, min, (*frame_list_itr), frame_rows, frame_cols, frame_channels);
			cout<<"Frame "<<++frame_count<<" processed"<<endl;
	}
}

