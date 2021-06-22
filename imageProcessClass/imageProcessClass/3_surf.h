#pragma once

#include <math.h>
#include "1_reduce_thermal.h"
#include "2_outline.h"

using namespace std;
using namespace cv;

/*
input:colorImg, imageL, imageR
output:imageOutput, img_matches_ransac, perspectiveImg, affinedImgL
@param colorImg 原彩色图像
@param imageL 彩色图像轮廓
@param imageR 红外图像轮廓
@param imageOutput 特征点配准图
@param img_matches_ransac ransac算法提纯后的特征点配准图
@param perspectiveImg 透视变换后的彩色图像
@param afinedImgL 仿射变换后的彩色图像
*/

class surfRansac
{
public:
	Mat colorImg, imageL, imageR, imageOutput, img_matches_ransac, perspectiveImg, affinedImgL;
	string path;
	surfRansac(int num_i, Mat &Cimg, Mat &Timg);
	int my_ransac();
	~surfRansac();
};