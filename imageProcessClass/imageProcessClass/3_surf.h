#pragma once

#include <math.h>
#include "1_reduce_thermal.h"
#include "2_outline.h"

using namespace std;
using namespace cv;

/*
input:colorImg, imageL, imageR
output:imageOutput, img_matches_ransac, perspectiveImg, affinedImgL
@param colorImg ԭ��ɫͼ��
@param imageL ��ɫͼ������
@param imageR ����ͼ������
@param imageOutput ��������׼ͼ
@param img_matches_ransac ransac�㷨�ᴿ�����������׼ͼ
@param perspectiveImg ͸�ӱ任��Ĳ�ɫͼ��
@param afinedImgL ����任��Ĳ�ɫͼ��
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