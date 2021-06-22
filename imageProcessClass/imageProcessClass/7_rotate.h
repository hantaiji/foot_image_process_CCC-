#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"
#define M_PI 3.14159265358979323846

using namespace std;
using namespace cv;

/*
input:thermalMat, tempMat
output:rotatedThermal, rotatedTemp
@param thermalMat ������д���������չʾ��ת���ͼ��
@param tempMat �����¶Ⱦ���
@param rotatedThermal ��ת��ĺ���ͼ��
@param rotatedTemp ��ת����¶Ⱦ���
*/
class rotateMat
{
public:
	Mat thermalMat, tempMat, rotatedThermal, rotatedTemp;
	rotateMat(Mat & ther_Mat,Mat & tem_Mat);
	void rotatedRigister();
	void rotateFunc();
	~rotateMat();
};