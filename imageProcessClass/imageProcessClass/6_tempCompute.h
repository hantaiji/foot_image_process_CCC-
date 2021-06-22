#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include <numeric>

using namespace std;
using namespace cv;

/*
input:_toComMat,segedThermal
output:tempMax, tempMin, tempAver, tempVar, tempStd
@param _toComMat 单足温度矩阵
@param segedThermal 单足红外图像，用于标注最高温、最低温位置，可不用，已注释掉
@param tempMax, tempMin, tempAver, tempVar, tempStd：最高温、最低温、平均温度、方差、标准差
*/

class tempCom
{
public:
	Mat _toComMat,segedThermal;
	float tempMax, tempMin, tempAver, tempVar, tempStd;
	tempCom(Mat & t,Mat & segedT);
	vector<float> comFunction(Mat & m);
	void position(Mat & t);
	void tempFeature(vector<float> & v);
	~tempCom();
};