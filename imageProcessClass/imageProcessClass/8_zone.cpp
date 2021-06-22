#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"
#include "7_rotate.h"
#include "8_zone.h"

zoneAnalysis::zoneAnalysis(Mat & img)
{
	this->image = img;
	zone(this->image);	
	//类tempCom需要温度矩阵和红外图像实现初始化，虽然红外图像可以不用，但还是需要这个参数，在这里用this->**A代替
	tempCom my_tempComMPA(this->MPA, this->MPA);
	tempCom my_tempComLPA(this->LPA, this->LPA);
	tempCom my_tempComMCA(this->MCA, this->MCA);
	tempCom my_tempComLCA(this->LCA, this->LCA);
	featurePush(my_tempComMPA);
	featurePush(my_tempComLPA);
	featurePush(my_tempComMCA);
	featurePush(my_tempComLCA);
	//for (int i = 0; i < this->tempFeature.size(); i++)
	//{
	//	cout << this->tempFeature[i] << " ";
	//}
}

vector<float> zoneAnalysis::center(Mat & m)
{
	Mat csvSrc = m;//温度矩阵;
	Mat src;
	csvSrc.copyTo(src);//等于相当于引用，修改一个(如threshlold函数就会改变原始值)会影响另一个，如果不想互相影响应使用copyTo函数
	vector<float> centroid;
	threshold(src, src, 0, 255, THRESH_BINARY);//转二值图像 参数THRESH_BINARY，if(src>threshold) dst=255
	//src直接用threshold获得的是32F（5）类型，findContours需要8U(0)类型
	src.convertTo(src, CV_8U);
	//不能是THRESH_OTSU 因为他是自动选取最佳划分阈值，所以有可能将部分不为零像素点置零
	vector<vector<Point>>contours;	//每个轮廓中的点
	vector<Vec4i>hierarchy;			//轮廓的索引？？？	
	//int型的mode:RETR_EXTERNAL,只检测最外围轮廓
	//int型的mode:RETR_TREE,检测所有轮廓
	findContours(src, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
	int x, y;
	for (int i = 0; i < contours.size(); ++i)
	{
		Mat tmp(contours.at(i));
		Moments moment = moments(tmp, false);
		if (moment.m00 != 0)//除数不能为0
		{
			x = cvRound(moment.m10 / moment.m00);//计算重心横坐标，对应列
			y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标，对应行
			circle(src, Point(x, y), 5, Scalar(0));
		}
	}
	//imshow("src", src);
	centroid.push_back(x); centroid.push_back(y);
	return centroid;
}

void zoneAnalysis::zone(Mat & z)
{
	vector<float> cen = center(z);
	//cout << z.type() << endl;
	int M = z.rows;int N = z.cols;
	//寻找C点
	int row_c, row_min, row_max, col_c;
	for (int i = N - 1; i >= 0; i--)
	{
		float a = 0;
		for (int j = 0; j <= cen[1]; j++)
		{
			a = a + z.at<float>(j, i);
		}
		if (a != 0)
		{
			for (int j = 0; j <= cen[1]; j++)
			{
				if (z.at<float>(j, i) != 0)
				{
					row_min = j; break;
				}
			}
			for (int j = cen[1]; j >= 0; j--)
			{
				if (z.at<float>(j, i) != 0)
				{
					row_max = j; break;
				}
			}
			row_c = floor((row_min + row_max) / 2); col_c = i;
			break;
		}
	}
	//寻找D点
	int row_d, col_d;
	for (int i = 0; i <= N-1; i++)
	{
		float a = 0;
		for (int j = 0; j <= cen[1]; j++)
		{
			a = a + z.at<float>(j, i);
		}
		if (a != 0)
		{
			row_d = row_c; col_d = i;
			break;
		}
	}
	int col_m = col_d + floor((col_c - col_d)*0.65);
	//寻找A点
	int row_a, col_min, col_max, col_a;
	for (int i = 0; i <= M-1; i++)
	{
		float a = 0;
		for (int j = cen[0]; j <= N-1; j++)
		{
			a = a + z.at<float>(i, j);
		}
		if (a != 0)
		{
			for (int j = cen[0]; j <= N - 1; j++)
			{
				if (z.at<float>(i, j) != 0)
				{
					col_min = j; break;
				}
			}
			for (int j = N-1; j >= cen[0]; j--)
			{
				if (z.at<float>(i, j) != 0)
				{
					col_max = j; break;
				}
			}
			row_a = i; col_a = floor((col_min + col_max) / 2);
			break;
		}
	}
	//寻找B点
	int row_b, col_b;
	for (int i = cen[1]; i <= M - 1; i++)
	{
		if (z.at<float>(i, cen[0])==0)
		{
			row_b = i-1; col_b = cen[0];
			break;
		}
	}
	/*cout << "row_a:" << row_a << " " << "col_a:" << col_a << endl;
	cout << "row_b:" << row_b << " " << "col_b:" << col_b << endl;
	cout << "row_c:" << row_c << " " << "col_c:" << col_c << endl;
	cout << "row_d:" << row_d << " " << "col_d:" << col_d << endl;*/
	Mat testMPA(ceil((row_b - row_a + 1)*0.6), col_c - col_m + 1, CV_32F);
	for (int i = 0; i < ceil((row_b - row_a + 1)*0.6); i++)
	{
		for (int j = 0; j < col_c - col_m + 1; j++)
		{
			testMPA.at<float>(i, j) = z.at<float>(row_a + i, col_m + j);
		}
	}
	Mat testLPA(ceil((row_b - row_a + 1)*0.6), col_c - col_d + 1, CV_32F);
	for (int i = 0; i <ceil((row_b - row_a + 1)*0.6); i++)
	{
		for (int j = 0; j <col_c - col_d + 1; j++)
		{
			if (row_a + i > row_c&&col_d + j > col_m)
			{
				testLPA.at<float>(i, j) = 0;
			}
			else
			{
				testLPA.at<float>(i, j) = z.at<float>(row_a + i, col_d + j);
			}
		}
	}
	Mat testMCA(ceil((row_b - row_a + 1)*0.4), col_c - col_b + 1, CV_32F);
	for (int i = 0; i <ceil((row_b - row_a + 1)*0.4); i++)
	{
		for (int j = 0; j <col_c - col_b + 1; j++)
		{
			testMCA.at<float>(i, j) = z.at<float>(row_a + floor((row_b - row_a + 1)*0.6) + 1 + i, col_b+ j);
		}
	}
	Mat testLCA(ceil((row_b - row_a + 1)*0.4), col_b - col_d + 1, CV_32F);
	for (int i = 0; i <ceil((row_b - row_a + 1)*0.4); i++)
	{
		for (int j = 0; j <col_b - col_d + 1; j++)
		{
			testLCA.at<float>(i, j) = z.at<float>(row_a + floor((row_b - row_a + 1)*0.6) + 1 + i, col_d + j);
		}
	}
	this->MPA = testMPA; this->LPA = testLPA;  this->MCA = testMCA; this->LCA = testLCA;
	//imshow("MPA", testMPA);
	//imshow("LPA", testLPA);
	//imshow("MCA", testMCA);
	//imshow("LCA", testLCA);
	//waitKey();
}

void zoneAnalysis::featurePush(tempCom tempCom)
{
	this->tempFeature.push_back(tempCom.tempMax);
	this->tempFeature.push_back(tempCom.tempMin);
	this->tempFeature.push_back(tempCom.tempAver);
	this->tempFeature.push_back(tempCom.tempVar);
	this->tempFeature.push_back(tempCom.tempStd);
}

zoneAnalysis::~zoneAnalysis()
{

}