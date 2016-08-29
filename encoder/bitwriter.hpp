/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include "huffman.hpp"
#include <map>
#include <vector>

typedef struct __bit{
	unsigned char b:1;
	__bit(){
		b = 0;
	}
}__bit;

typedef std::map<short, std::vector<__bit> > __code_map;
__code_map* write_code(huffman_tree* hmt);

void u_write_file(unsigned char* img, __code_map* code_map, unsigned long long n);
void s_write_file(signed char* img, __code_map* code_map, unsigned long long n);
