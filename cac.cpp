/*
 *  Copyright (c) 2016 The CAC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS 
 *  file in the root of the source tree.
 */

#include<iostream>
#include<algorithm>
#include<iterator>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<list>
#include"opencv/highgui.h"
#include"encoder/encode.hpp"
#include"common/common.hpp"
#include"decoder/decode.hpp"
#include"test/test.hpp"
#include"namespace/namespace.hpp"

list<Mat> frame_list;
list<Mat>:: iterator frame_it;

int main(int argc, char** argv){
	//test_delta();
	//huffman_u_test();
	printf("\n");
	cout<<"-----------------------------"<<endl;
	//huffman_s_test();*/
	CvCapture *capture = cvCaptureFromFile("Megamind.avi");
	if (!capture) {
    std::cout << "COULD NOT OPEN CAPTURE\n";
		exit(0);
	}
	
	IplImage * frame = NULL;
	while((frame = cvQueryFrame(capture))!=NULL){
		Mat frame_mat(frame, true);
		frame_list.push_back(frame_mat);
	}

	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);

	/*unsigned long long elements = 0;
	for(frame_it = frame_list.begin(); frame_it!= frame_list.end(); frame_it++){
		elements = (*frame_it).cols*(*frame_it).rows*(*frame_it).channels();
		//cout<<(*frame_it).cols<<" "<<(*frame_it).rows<<" "<<(*frame_it).channels()<<" "<<(*frame_it).type()<<endl;
	}

	cout<<"elements"<<elements<<endl;*/

	//Frist frame in the video is totally black. It is ignored for the latter reason.
	//frame_list.pop_front();

	cout<<"Total Frame Count "<<frame_list.size()<<endl;
	if(frame_list.empty()){
		cout<<"Frame list is empty"<<endl;
		return -1;
	}
  

	Mat max, min, rec_img;

	encode(frame_list, max, min);

	huffman_u_test(1140480);

	frame_it = frame_list.begin();

	int frame_rows = (*frame_it).rows;
	int frame_cols = (*frame_it).cols;
	int frame_channels = (*frame_it).channels();

	while(!video_codec.empty()){
			recover_frame(max, min, rec_img, video_codec.front(), frame_rows, frame_cols, frame_channels);
			validate_dif(rec_img, (*frame_it), frame_rows, frame_cols, frame_channels, max, min);
			frame_it++;
			video_codec.pop();
			cvNamedWindow("Video", WINDOW_AUTOSIZE);
			imshow("Video", rec_img);
			cvWaitKey(33);
	}

	cvDestroyWindow("Video");

	if(frame_it != frame_list.end()){
		cout<<"One or more frames are missing from Video Codec"<<endl;
	}

	frame_list.clear();
	
return 0;
}
