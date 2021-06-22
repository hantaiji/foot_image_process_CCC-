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
@param _toComMat �����¶Ⱦ���
@param segedThermal �������ͼ�����ڱ�ע����¡������λ�ã��ɲ��ã���ע�͵�
@param tempMax, tempMin, tempAver, tempVar, tempStd������¡�����¡�ƽ���¶ȡ������׼��
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