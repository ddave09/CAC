/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include <iostream>
#include <queue>
#include <functional>
#include "huffman.hpp"
#define PADDED_OUT(x) (x+padding)
#define PADDED_IN(x) (x-padding)

using namespace std;

template <class T>
struct MinHeap
{
  bool operator() (const T *a, const T *b) const{
    return (a->gram > b->gram);
  }
};

typedef priority_queue<huffman_tree*, vector<huffman_tree*>, MinHeap<huffman_tree> > min_priority_queue;

bool is_leaf(huffman_tree *node){
	return ((node->left == NULL) && (node->right == NULL));
}

//TODO: Rather than popping and creating node again, mark node as used
huffman_tree* create_code(min_priority_queue &min_pq){
	huffman_tree *hmt = NULL;
	if(min_pq.empty()){
		return NULL;
	}
	else if(min_pq.size() == 1){
		hmt = new huffman_tree(min_pq.top());
		min_pq.pop();
		return hmt;
	}

	hmt = new huffman_tree();

	while(min_pq.size() > 1){
		huffman_tree *temp_parent = new huffman_tree();

		huffman_tree *node_min1 = min_pq.top();
		unsigned long long min1 = node_min1->gram;
		huffman_tree *temp_left = new huffman_tree();
		temp_left->gram = min1;
		temp_left->parent = temp_parent;
		temp_left->dirctn = LEFT;
		if(!is_leaf(node_min1)){
			temp_left->left = node_min1->left;
			temp_left->right = node_min1->right;
		}
		else{
			temp_left->value = node_min1->value;
		}

		min_pq.pop();

		huffman_tree* node_min2 = min_pq.top();
		unsigned long long min2 = node_min2->gram;
		huffman_tree *temp_right = new huffman_tree();
		temp_right->gram = min2;
		temp_right->parent = temp_parent;
		temp_right->dirctn = RIGHT;
		if(!is_leaf(node_min2)){
			temp_right->left = node_min2->left;
			temp_right->right = node_min2->right;
		}
		else{
			temp_right->value = node_min2->value;
		}
		min_pq.pop();

		unsigned long long temp_min = min1 + min2;
		temp_parent->gram = temp_min;
		temp_parent->left = temp_left;
		temp_parent->right = temp_right;

		min_pq.push(temp_parent);
	}

	hmt->gram = min_pq.top()->gram;
	hmt->left = min_pq.top()->left;
	hmt->right = min_pq.top()->right;
	min_pq.pop();

return hmt;
}

void prepare(unsigned long long histo[], min_priority_queue &min_pq, unsigned char padding){
	int i = 0;
	for(i=0;i<256;i++){
		if(histo[i]!=0){
			non_zero_count++;
			huffman_tree *leaf_node = new huffman_tree();
			cout<<"Key "<<i<<" Value "<<histo[i]<<endl;
			leaf_node->gram = histo[i];
			leaf_node->value = PADDED_IN(i);
			min_pq.push(leaf_node);
		}
	}
}

template<typename type>
huffman_tree* huffman_8(type* img, unsigned long long n, unsigned char padding = 0){
	int i=0;
	unsigned long long histo[256] = {0};
	min_priority_queue min_pq;

	for(i=0;i<n;i++){
		histo[PADDED_OUT(img[i])]++;
	}

	prepare(histo, min_pq, padding);
	return create_code(min_pq);
}

huffman_tree* u_code_huffman(unsigned char* img, unsigned long long n){
	return huffman_8(img, n);
}

huffman_tree* s_code_huffman(signed char* img, unsigned long long n){
	unsigned char padding = 128;
	return huffman_8(img, n, padding);
}

