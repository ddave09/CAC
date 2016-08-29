/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include "test.hpp"
#include "../encoder/huffman.hpp"
#include "../encoder/bitwriter.hpp"
#include "../common/common.hpp"

#define BASE_CHANNEL_INDEX i*cols*channels+j*channels

using namespace std;

void check_delta(unsigned char val1, unsigned char val2, unsigned char steps,
		unsigned long test_num){
	if(delta_code(val1, val2) == steps){
		printf("Test %lu passed\n", test_num);
		return;
	}
	printf("Test %lu failed\n", test_num);
}

void test_delta(){
	check_delta(18, 20, 3, 1);
	check_delta(20, 18, 4, 2);
	check_delta(0, 3, 5, 3);
	check_delta(255, 0, 1, 4);
	check_delta(255, 250, 10, 5);
}

void tree_traverse(huffman_tree *hft){
	huffman_tree *hmt = hft;
	if(hmt == NULL){
		return;
	}
	if(hmt->left == NULL && hmt->right == NULL){
		cout<<"F: "<<hmt->gram<<" V: "<<hmt->value<<endl;
	}
	else{
		cout<<"F: "<<hmt->gram<<" "<<endl;
	}
	tree_traverse(hmt->left);
	tree_traverse(hmt->right);
}


//For megamind.avi, elements: 1140480
void huffman_u_test(unsigned long long elements = 1140480){
	//unsigned char img[7] = {8, 8, 8, 25, 32, 77, 100};
//	unsigned char *img = new unsigned char[7];
	huffman_tree *hft = NULL;
	predictive_video_codec.pop();
	unsigned char *img = predictive_video_codec.front();
	hft = u_code_huffman(img, elements);
	assert(hft != NULL);
	//tree_traverse(hft);
	//__code_map* code_map = write_code(hft);
	//u_write_file(img, code_map, elements);
	printf("DONE!");
}

void huffman_s_test(){
	//signed char img[7] = {-128, 0, 127, -128, 127, -128, 0};
	//signed char img[7] = {-128, -128, -128, -128, -128, -128, -128};
	//unsigned long long sample = 7;
	/*unsigned long long int i = 0;
	for(i=0; i<sample; i++){
		img[i] = -128;
	}*/
	huffman_tree *hft = NULL;
	//video_codec.pop();
	signed char *img = video_codec.front();
	unsigned long long n = 1140480;
	hft = s_code_huffman(img, n);
	assert(hft != NULL);
	//tree_traverse(hft);
	__code_map* code_map = write_code(hft);
	//printf("File I/O Initiated\n");
	//s_write_file(img, code_map, n);
	printf("\nDONE!\n");
}

void validate_dif(Mat &first, Mat &second, int rows, int cols, int channels, Mat &max, Mat &min){
	int i=0,j=0;
	uint8_t *first_data = (uint8_t*)first.data;
	uint8_t *second_data = (uint8_t*)second.data;
	long long sum = 0;
	#pragma parallel for
	for(i=0; i<rows; i++){
		#pragma parallel for
		for(j=0; j<cols; j++){
			if(first_data[BASE_CHANNEL_INDEX] != second_data[BASE_CHANNEL_INDEX]){
				cout<<"Channel BLUE differs at ["<<i<<"]"<<"["<<j<<"] with values "<< first_data[BASE_CHANNEL_INDEX] <<" "<< second_data[BASE_CHANNEL_INDEX]<<endl;
			}
			if(first_data[BASE_CHANNEL_INDEX+1] != second_data[BASE_CHANNEL_INDEX+1]){
				printf(" Channel GREEN differs at point [%d][%d] with values %u %u, Max %u Min %u \n", i, j, first_data[BASE_CHANNEL_INDEX+1], second_data[BASE_CHANNEL_INDEX+1], max.data[BASE_CHANNEL_INDEX+1], min.data[BASE_CHANNEL_INDEX+1]);
			}
			if(first_data[BASE_CHANNEL_INDEX+2] != second_data[BASE_CHANNEL_INDEX+2]){
				cout<<"Channel RED differs at ["<<i<<"]"<<"["<<j<<"] with values "<< first_data[BASE_CHANNEL_INDEX+2] <<" "<< second_data[BASE_CHANNEL_INDEX+2]<<endl;
			}
			sum +=	
			(first_data[BASE_CHANNEL_INDEX] - second_data[BASE_CHANNEL_INDEX]) + 
			(first_data[BASE_CHANNEL_INDEX+1] - second_data[BASE_CHANNEL_INDEX+1]) + 
			(first_data[BASE_CHANNEL_INDEX+2] - second_data[BASE_CHANNEL_INDEX+2]);	
		}
	}
	assert(sum==0);
	//cout<<"Dif sum: "<<sum<<endl;
}

