/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include<list>
#include<opencv2/core/core.hpp>
#include"../namespace/namespace.hpp"

void encode(list<Mat> &frame_list, Mat &max, Mat &min);
void predictive_encode(list<Mat> &frame_list, Mat &max, Mat &min);
//Remove below functions from header file after tests are complete
unsigned char delta_code(unsigned char pred_val, unsigned char frame_val);

