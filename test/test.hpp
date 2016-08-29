/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include "highgui.h"
#include<opencv2/core/core.hpp>
#include"../namespace/namespace.hpp"
#include"../encoder/encode.hpp"
void validate_dif(Mat &first, Mat &second, int rows, int cols, int channels, Mat &max, Mat &min);
void huffman_u_test(unsigned long long elements);
void huffman_s_test();
void test_delta();
