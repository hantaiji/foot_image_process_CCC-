#pragma once

#include "1_reduce_thermal.h"
#include <opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;

/*
input:colorImg, rdcImg
output:cannyCResult, cannyTResult
@param colorImg ԭ��ɫͼ��
@param rdcImg ���ͷֱ��ʺ�ĺ���ͼ�� 320*240
@param cannyCResult ��ɫͼ������
@param cannyTResult ����ͼ������
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