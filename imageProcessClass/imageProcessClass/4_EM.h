#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

/*
input:imgOrigin��imgTher
output:imgSeged, timgSeged
@param imgOrigin �任��Ĳ�ɫͼ��
@param imgTher ���ͷֱ��ʺ�ĺ���ͼ��
@param imgSeged �任��Ĳ�ɫͼ��ָ���
@param timgSeged �任��ĺ���ͼ����׼���
*/

class EMCluster
{
public:
	Mat imgOrigin, imgTher, imgSeged, timgSeged;
	string path;
	EMCluster(int num_i);
	//overload
	//�Է���任���ͼ����зָ�����ָ��Ľ����׼������ͼ��
	EMCluster(Mat & tranImg, Mat & therImg);
	int myEM();
	~EMCluster();
};