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
	Mat thermalImage = this->thermalMat;//��ɫ����ͼ��
	Mat temp = this->tempMat;
	Mat src, graySrc;
	cvtColor(thermalImage, src, COLOR_BGR2GRAY);//ת�Ҷ�
	src.copyTo(graySrc);//�Ҷ�ͼ�񱣴���graysrc��������չʾ
	threshold(src, src, 0, 255, THRESH_BINARY);//ת��ֵͼ�� ����THRESH_BINARY��if(src>threshold) dst=255
	//������THRESH_OTSU ��Ϊ�����Զ�ѡȡ��ѻ�����ֵ�������п��ܽ����ֲ�Ϊ�����ص�����
	vector<vector<Point>>contours;	//ÿ�������еĵ�
	vector<Vec4i>hierarchy;			//����������������	
	//int�͵�mode:RETR_EXTERNAL,ֻ�������Χ����
	//int�͵�mode:RETR_TREE,�����������
	findContours(src, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
	int x, y;
	for (int i = 0; i < contours.size(); ++i)
	{
		Mat tmp(contours.at(i));
		Moments moment = moments(tmp, false);
		if (moment.m00 != 0)//��������Ϊ0
		{
			x = cvRound(moment.m10 / moment.m00);//�������ĺ����꣬��Ӧ��
			y = cvRound(moment.m01 / moment.m00);//�������������꣬��Ӧ��
			//circle(src, Point(x, y), 5, Scalar(0));
		}
	}
	src.convertTo(src, CV_32F);//ruduce�����и�ʽҪ��������Ҫת����ʽ
	Mat sumRow(1, src.rows, CV_32F, Scalar(0));//����֮��
	reduce(src, sumRow, 1, REDUCE_SUM);

	int row_codi = y;//����������
	vector<int> begin, terminate;//��߽硢�ұ߽�
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
	//image���ڱ������ͼ��
	Mat image = Mat::zeros(480, 640, CV_8UC3);
	//���������
	vector<Point>points;
	for (int i = 0; i < begin.size(); i++)
	{
		points.push_back(Point(i, (terminate[i] - begin[i]) / 2 + begin[i]));
	}
	//����
	for (int i = 0; i < points.size(); i++)

	{

		circle(image, points[i], 5, Scalar(0, 0, 255), 2, 8, 0);

	}
	//��ϣ�����ֱ��
	Vec4f line_para;
	fitLine(points, line_para, DIST_L2, 0, 1e-2, 1e-2);
	//����б��
	Point point0;
	point0.x = line_para[2];
	point0.y = line_para[3];
	double k = line_para[1] / line_para[0];
	double angle = atan(k);//����
	angle = angle * 180 / M_PI;//�Ƕ�
	//dst���ڱ�����ת���ͼ��/����
	Mat dst,tempData;
	//float scale = 200.0/ src.rows;//��������    
	//cv::resize(src, src, cv::Size(), scale, scale, cv::INTER_LINEAR);    	    	
	//���ͼ��ĳߴ���ԭͼһ��    
	Size dst_sz(src.cols, src.rows);
	//ָ����ת����      
	Point2f center(static_cast<float>(src.cols / 2.), static_cast<float>(src.rows / 2.));
	//��ȡ��ת����2x3����      
	Mat rot_mat = getRotationMatrix2D(center, -angle, 1.0);
	//����ѡ�񱳾��߽���ɫ   
	/*cv::Scalar borderColor = Scalar(0, 238, 0);*/
	/*cv::warpAffine(src, dst, rot_mat, src.size(), INTER_LINEAR, BORDER_CONSTANT, borderColor);*/
	//���Ʊ�Ե���
	warpAffine(graySrc, dst, rot_mat, dst_sz, INTER_LINEAR, BORDER_REPLICATE);

	warpAffine(temp, tempData, rot_mat, dst_sz, INTER_LINEAR, BORDER_REPLICATE);//������ת���ڱ߽�����ϵ͵��¶�����

	dst.copyTo(this->rotatedThermal);//������ת��ĻҶ�ͼ��
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
	//��׼��ĺ���ͼ��ת�ɻҶ�ͼ�� typeΪ0����CV_8U uchar
	//cvtColor(this->rotatedThermal, grayThermal, COLOR_BGR2GRAY);
	for (int i = 0; i < grayThermal.rows; i++)
	{
		for (int j = 0; j < grayThermal.cols; j++)
		{
			//grayThermal.at<uchar>,�����������ͱ������������һ�£���������at���������ط���
			if (grayThermal.at<uchar>(i, j) == 0)
			{
				//typeΪ5����CV_32F float
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