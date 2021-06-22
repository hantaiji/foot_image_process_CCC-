#pragma once

#include <opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"//IplImageͷ�ļ�
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
@param originImg ԭʼ����ͼ�� 640*480
@param reducedThermal ���ͷֱ��ʺ�ĺ���ͼ�� 320*240
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