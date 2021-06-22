#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"
#include "7_rotate.h"

rotateMat::rotateMat(Mat & ther,Mat & temp)
{
	this->thermalMat = ther;
	this->tempMat = temp;
	rotateFunc();
	rotatedRigister();
}

void rotateMat::rotateFunc()
{
	Mat thermalImage = this->thermalMat;//彩色红外图像
	Mat temp = this->tempMat;
	Mat src, graySrc;
	cvtColor(thermalImage, src, COLOR_BGR2GRAY);//转灰度
	src.copyTo(graySrc);//灰度图像保存在graysrc，以用于展示
	threshold(src, src, 0, 255, THRESH_BINARY);//转二值图像 参数THRESH_BINARY，if(src>threshold) dst=255
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
			//circle(src, Point(x, y), 5, Scalar(0));
		}
	}
	src.convertTo(src, CV_32F);//ruduce函数有格式要求，所以需要转换格式
	Mat sumRow(1, src.rows, CV_32F, Scalar(0));//各行之和
	reduce(src, sumRow, 1, REDUCE_SUM);

	int row_codi = y;//质心所在行
	vector<int> begin, terminate;//左边界、右边界
	while (1)
	{
		if (sumRow.at<float>(row_codi) == 0)
			break;
		if (src.at<float>(row_codi, 0) != 0)
		{
			break;
		}
		for (int i = 0; i < src.cols - 1; i++)//
		{

			if (src.at<float>(row_codi, i) == 0 && src.at<float>(row_codi, i + 1) != 0)
			{
				begin.push_back(i + 1); 
			}
			else
			{
				if (src.at<float>(row_codi, i) != 0 && src.at<float>(row_codi, i + 1) == 0)
				{
					terminate.push_back(i); 
				}
			}

		}
		row_codi++;
	}
	//image用于保存拟合图像
	Mat image = Mat::zeros(480, 640, CV_8UC3);
	//拟合所需点对
	vector<Point>points;
	for (int i = 0; i < begin.size(); i++)
	{
		points.push_back(Point(i, (terminate[i] - begin[i]) / 2 + begin[i]));
	}
	//画点
	for (int i = 0; i < points.size(); i++)

	{

		circle(image, points[i], 5, Scalar(0, 0, 255), 2, 8, 0);

	}
	//拟合，并画直线
	Vec4f line_para;
	fitLine(points, line_para, DIST_L2, 0, 1e-2, 1e-2);
	//计算斜率
	Point point0;
	point0.x = line_para[2];
	point0.y = line_para[3];
	double k = line_para[1] / line_para[0];
	double angle = atan(k);//弧度
	angle = angle * 180 / M_PI;//角度
	//dst用于保存旋转后的图像/矩阵
	Mat dst,tempData;
	//float scale = 200.0/ src.rows;//缩放因子    
	//cv::resize(src, src, cv::Size(), scale, scale, cv::INTER_LINEAR);    	    	
	//输出图像的尺寸与原图一样    
	Size dst_sz(src.cols, src.rows);
	//指定旋转中心      
	Point2f center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));
	//获取旋转矩阵（2x3矩阵）      
	Mat rot_mat = getRotationMatrix2D(center, -angle, 1.0);
	//设置选择背景边界颜色   
	/*cv::Scalar borderColor = Scalar(0, 238, 0);*/
	/*cv::warpAffine(src, dst, rot_mat, src.size(), INTER_LINEAR, BORDER_CONSTANT, borderColor);*/
	//复制边缘填充
	warpAffine(graySrc, dst, rot_mat, dst_sz, INTER_LINEAR, BORDER_REPLICATE);

	warpAffine(temp, tempData, rot_mat, dst_sz, INTER_LINEAR, BORDER_REPLICATE);//这样旋转会在边界产生较低的温度数据

	dst.copyTo(this->rotatedThermal);//保存旋转后的灰度图像
	tempData.copyTo(this->rotatedTemp);
	//cout << "rotatedTemp" << rotatedTemp << endl;
	/*imshow("graySrc", graySrc);
	imshow("dst", dst);
	imshow("rotatedTemp", rotatedTemp);
	waitKey(0);*/
}

void rotateMat::rotatedRigister()
{
	Mat grayThermal = this->rotatedThermal;
	//配准后的红外图像转成灰度图像 type为0，即CV_8U uchar
	//cvtColor(this->rotatedThermal, grayThermal, COLOR_BGR2GRAY);
	for (int i = 0; i < grayThermal.rows; i++)
	{
		for (int j = 0; j < grayThermal.cols; j++)
		{
			//grayThermal.at<uchar>,尖括号中类型必须与矩阵类型一致，才能利用at函数逐像素访问
			if (grayThermal.at<uchar>(i, j) == 0)
			{
				//type为5，即CV_32F float
				this->rotatedTemp.at<float>(i, j) = 0;
			}
		}
	}
	/*imshow("this->rotatedTemp", this->rotatedTemp);
	waitKey();*/
}

rotateMat::~rotateMat()
{

}