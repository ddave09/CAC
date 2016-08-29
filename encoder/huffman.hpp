/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include <stdio.h>

#ifndef __HUFFMAN_H
#define __HUFFMAN_H
enum DIRECTION{
	LEFT = 0,
	RIGHT,
	UNDEF
};

typedef struct ht{
	unsigned long long gram;
	signed short value; //Value applies to only leaf nodes
	DIRECTION dirctn;
	ht *parent;
	ht *left;
	ht *right;
	ht(){
		gram = 0;
		value = 0;
		dirctn = UNDEF;
		parent = NULL;
		left = NULL;
		right = NULL;
	}
	ht(ht *node){
		gram = node->gram;
		value = node->value;
		dirctn = UNDEF;
		parent = node->parent;
		left = node->left;
		right = node->right;
	}
}huffman_tree;

huffman_tree* u_code_huffman(unsigned char* img, unsigned long long n);
huffman_tree* s_code_huffman(signed char* img, unsigned long long n);
#endif

