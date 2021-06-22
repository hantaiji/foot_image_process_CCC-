#pragma once
#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"
#include "7_rotate.h"

using namespace std;
using namespace cv;

class zoneAnalysis
{
public:
	Mat image, MPA, LPA, MCA, LCA, analizedImg;
	vector<float> tempFeature;
	vector<float> center(Mat & m);//求质心
	zoneAnalysis(Mat & rotated);
	void zone(Mat & img);//分区域
	void featurePush(tempCom tempCom);
	~zoneAnalysis();
};