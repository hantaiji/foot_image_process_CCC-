#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include <iostream>
#include <fstream> 
#include <iomanip>

using namespace std;
using namespace cv;

/*
input:thermalSeged 
output:tempMat, thermalMat, csvMat
@param thermalSeged ��׼��ĺ������ͼ��
@param tempMat ��׼����¶Ⱦ���ֻ����׵��¶����ݣ�����λ��Ϊ0��
@param thermalMat ���ֺ�� ��׺�����ͼ--thermalMat[0] thermalMat[1]
@param csvMat ���ֺ�� ����¶�������ͼ(�¶Ⱦ���)--csvMat[0] csvMat[1]
*/

class readCSV
{
public:
	Mat thermalSeged, tempMat;
	//Mat leftThermal, rightThermal, leftMat, rightMat;
	vector<Mat> thermalMat, csvMat ,unRigisterMat;
	string path,cutCsvPathL, cutCsvPathR;
	readCSV(int num_i, Mat & timgSeged);
	vector<Mat> my_Cut(Mat & whole);
	int csvRead();
	int csvRigister();
	void xlsWrite(int i,Mat & m,string p);
	~readCSV();
}; 