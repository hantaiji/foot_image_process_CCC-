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
@param thermalSeged 配准后的红外足底图像
@param tempMat 配准后的温度矩阵（只有足底的温度数据，其他位置为0）
@param thermalMat 划分后的 足底红外子图--thermalMat[0] thermalMat[1]
@param csvMat 划分后的 足底温度左右子图(温度矩阵)--csvMat[0] csvMat[1]
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