#pragma once

#include "1_reduce_thermal.h"
#include <opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;

/*
input:colorImg, rdcImg
output:cannyCResult, cannyTResult
@param colorImg Ô­²ÊÉ«Í¼Ïñ
@param rdcImg ½µµÍ·Ö±æÂÊºóµÄºìÍâÍ¼Ïñ 320*240
@param cannyCResult ²ÊÉ«Í¼ÏñÂÖÀª
@param cannyTResult ºìÍâÍ¼ÏñÂÖÀª
*/

class outLine
{
public:
	Mat colorImg, rdcImg, cannyCResult, cannyTResult;
	string path;
	outLine(int num_i, Mat & reduced_img);
	void GrayImage(Mat & image, Mat & grayimage);
	int get_outline();
	~outLine();
};