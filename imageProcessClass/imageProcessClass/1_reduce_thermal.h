#pragma once

#include <opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"//IplImage头文件
#include <opencv2/core.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>  
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <cstdlib>
#include <sstream>
#include <cstring>

using namespace std;
using namespace cv;

/*
input:originImg 
output:reducedThermal
@param originImg 原始红外图像 640*480
@param reducedThermal 降低分辨率后的红外图像 320*240
*/

class reduce_ther
{
public:
	Mat originImg, reducedImg;
	string path;
	reduce_ther(int num_i);
	Mat reduceThermal();
	~reduce_ther();
};