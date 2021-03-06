/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
//TODO: Need to have constant map
#include <map>
#include "bitwriter.hpp"

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

using namespace std;

typedef struct __byte{
	unsigned char count;
	unsigned char byte;
	__byte(){
		count = 0;
		byte = 0;
	}
}__byte;

void write_byte(unsigned char byte, int file){
	if(write(file, &byte, 1) == -1){
		fprintf(stderr, "Unable to write file");
		//FIXME: delete code_map and img before program terminates
		exit(EXIT_FAILURE);
	}
	//printf("%x ", byte);
}

void shift_ctrl(__bit bit, __byte &byte, int file){
	if(byte.count == 7){
		write_byte(byte.byte, file);
		byte.count = 0;
		byte.byte = 0;
		return;
	}
	byte.byte |= bit.b;
	byte.byte <<= 1;
	byte.count += 1;
}

void stream_code(vector<__bit> &stream, __byte &byte, int file){
	vector<__bit>::iterator stream_it;
	for(stream_it = stream.begin(); stream_it != stream.end(); stream_it++){
		shift_ctrl((*stream_it), byte, file);
		//cout<<(unsigned int)(*stream_it).b;
	}
}


//TODO: If *img is not handled in previous functions after use. Mange resource at the end of this function.
template<typename type>
void write_file(type* img, __code_map* code_map, unsigned long long n){
	//FIXME: Temporary variable for test
	unsigned int count = 1;
	char *file_name = new char[10+sizeof(unsigned int)];
	sprintf(file_name, "Frames/FRM%u", count);
	int file = open(file_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	if(file == -1){
		fprintf(stderr, "Unable to open file");
		if(file_name != NULL)
			delete file_name;
		exit(EXIT_FAILURE);
	}

	int i = 0;
	__byte byte;
	__code_map :: iterator code_map_it;
	if(code_map == NULL || img == NULL){
		fprintf(stderr, "Invalid code or image");
		exit(EXIT_FAILURE);
	}
	for(i=0; i<n; i++){
		code_map_it = (*code_map).find(img[i]);
		if(code_map_it != (*code_map).end()){
			vector<__bit> code_stream = code_map_it->second;
			stream_code(code_stream, byte, file);
		}
		else{
			fprintf(stderr, "Huffman tree is corrupted");
			exit(EXIT_FAILURE);
		}
	}
	//FIXME: Last byte is written right aligned.\
	Decompression has to be smart about where to stop.
	if(byte.count){
		unsigned char shift_by = 7 - byte.count;
		byte.byte <<= shift_by;
		write_byte(byte.byte, file);
	}
	if(close(file) == -1){
		fprintf(stderr, "Unable to close file");
		if(file_name != NULL)
			delete file_name;
		exit(EXIT_FAILURE);
	}

	if(file_name != NULL){
		delete file_name;
	}
}

void u_write_file(unsigned char* img, __code_map* code_map, unsigned long long n){
	write_file(img, code_map, n);
}

void s_write_file(signed char* img, __code_map* code_map, unsigned long long n){
	write_file(img, code_map, n);
}

void visit(huffman_tree *node, vector<__bit>& bit_stream, __byte &byte,
		__code_map *code_map){
	vector<__bit> :: iterator bit_it;
	if(unlikely(node->left == NULL && node->right == NULL)){
		vector<__bit> code_value;
		for(bit_it = bit_stream.begin(); bit_it != bit_stream.end(); bit_it++){
			//shift_ctrl((*bit_it), byte);
			code_value.push_back(*bit_it);
			//cout<<(unsigned int)(*bit_it).b;
		}
		__bit bit;
		bit.b = node->dirctn;
		//shift_ctrl(bit, byte);
		code_value.push_back(bit);
		//cout<<(unsigned int)node->dirctn<<" ";
		code_map->operator[](node->value) = code_value;
		//cout<<" | "<<(int)node->value<<endl;
	}
	else{
		if(likely(node->dirctn != UNDEF)){
			__bit bit;
			bit.b = node->dirctn;
			bit_stream.push_back(bit);
		}
	}
}

/* Temporary test function remove in release */
void iterate_code_map(__code_map *code_map){
	__code_map :: iterator code_map_it;
	for(code_map_it = (*code_map).begin(); code_map_it != (*code_map).end(); code_map_it++){
		//cout<<"Key: "<< code_map_it->first<<" Value:";
		vector<__bit> :: iterator code_value_it;
		vector<__bit> code_value = code_map_it->second;
		for(code_value_it = code_value.begin(); code_value_it != code_value.end(); code_value_it++){
			//cout<<(short)(*code_value_it).b;
		}
		//cout<<""<<endl;
	}
}

__code_map* write_code(huffman_tree* hmt){
	if(hmt == NULL){
		return NULL;
	}
	huffman_tree *p_hmt = hmt;
	__code_map *code_map = new __code_map;
	stack<huffman_tree*> node_stack;
	__byte byte;
	vector<__bit> bit_stream;
	bit_stream.clear();
	if(!node_stack.empty()){
		fprintf(stderr, "Initial stack was not empty");
		if(code_map != NULL){
			delete code_map;
		}
		//TODO: Free all the resources. It is a must TO DO
		exit(EXIT_FAILURE);
	}
	while(!node_stack.empty() || p_hmt != NULL){
		if(p_hmt != NULL){
			visit(p_hmt, bit_stream, byte, code_map);
			if(p_hmt->right != NULL){
				node_stack.push(p_hmt->right);
			}
			p_hmt = p_hmt->left;
		}
		else{
			p_hmt = node_stack.top();
			node_stack.pop();
			if(!bit_stream.empty()){
				if(p_hmt->left != NULL && p_hmt->right != NULL){
					bit_stream.pop_back();
				}
			}
		}
	}
	//Remove in release
	//iterate_code_map(code_map);
	return code_map;
//	delete code_map;
}
