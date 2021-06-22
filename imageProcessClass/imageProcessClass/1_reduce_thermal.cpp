#include "1_reduce_thermal.h"

reduce_ther::reduce_ther(int i)
{
	if (i < 10)
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR000";
		this->path = p + to_string(i) + ".jpg";
	}
	else
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR00";
		this->path = p + to_string(i) + ".jpg";
	}
	this->originImg = imread(this->path);
	this->reducedImg = reduceThermal();
}

Mat reduce_ther::reduceThermal()
{
	Mat img = this->originImg;
	//Mat img = imread(this->path);
	//Mat img = imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR0003.jpg"); //����һ��Mat
	//cout << img.type() << endl;
	Mat rdc_img(240, 320, CV_8UC3);//�ڶ���rdc_imgʱ������������CV_8UC3������������img<vec3b>��ֵʱ������Ϊ����δ֪������

	for (int i = 0; i < img.rows / 2; i++)
	{
		for (int j = 0; j < img.cols / 2; j++)
		{
			rdc_img.at<Vec3b>(i, j) = img.at<Vec3b>(2 * i, 2 * j);
		}
	}
	/*imshow("ԭͼ��", img);
	imshow("�ֱ��ʽ��ͺ��ͼ��", rdc_img);
	waitKey(0);*/
	return rdc_img;
}

reduce_ther::~reduce_ther()
{

}