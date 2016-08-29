/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#ifndef __HIGHGUI
#define __HIGHGUI
#include "highgui.h"
#include"../namespace/namespace.hpp"
#endif
#include<opencv2/core/core.hpp>

void recover_frame(Mat &max, Mat &min, Mat& frame, signed char *c, int rows, int cols, int channels);

