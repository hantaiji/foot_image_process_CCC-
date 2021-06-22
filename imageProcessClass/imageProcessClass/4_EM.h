#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

/*
input:imgOrigin，imgTher
output:imgSeged, timgSeged
@param imgOrigin 变换后的彩色图像
@param imgTher 降低分辨率后的红外图像
@param imgSeged 变换后的彩色图像分割结果
@param timgSeged 变换后的红外图像配准结果
*/

class EMCluster
{
public:
	Mat imgOrigin, imgTher, imgSeged, timgSeged;
	string path;
	EMCluster(int num_i);
	//overload
	//对仿射变换后的图像进行分割，并将分割后的结果配准到红外图像
	EMCluster(Mat & tranImg, Mat & therImg);
	int myEM();
	~EMCluster();
};