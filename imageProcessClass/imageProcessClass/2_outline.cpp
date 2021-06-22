#include "1_reduce_thermal.h"
#include "2_outline.h"

using namespace std;
using namespace cv;

outLine::outLine(int i, Mat & reduced_img)
{
	if (i < 10)
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR000";
		this->path = p + to_string(i) + " (2).jpg";
	}
	else
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR00";
		this->path = p + to_string(i) + " (2).jpg";
	}
	this->colorImg = imread(this->path);
	this->rdcImg = reduced_img;
	get_outline();
}

void outLine::GrayImage(Mat & image, Mat & grayimage)
{
	cvtColor(image, grayimage, COLOR_BGR2GRAY);
}

int outLine::get_outline()
{
	Mat color_img = this->colorImg;
	//Mat color_img = imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR0003 (2).jpg");
	Mat img = this->rdcImg;
	Mat gray_cimg, gray_timg;
	/*Mat cannyTResult;
	Mat cannyCResult;*/
	//Mat *pointer_thermal;

	GrayImage(color_img, gray_cimg);
	GrayImage(img, gray_timg);
	Canny(color_img, this->cannyCResult, 200, 255);
	Canny(img, this->cannyTResult, 240, 255);
	//低于阈值1的像素点会被认为不是边缘；
	//高于阈值2的像素点会被认为是边缘；
	//在阈值1和阈值2之间的像素点, 若与第2步得到的边缘像素点相邻，则被认为是边缘，否则被认为不是边缘
	
	imwrite("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\c_outline.jpg", this->cannyCResult);
	imwrite("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\t_outline.jpg", this->cannyTResult);
	return 0;
}

outLine::~outLine()
{

}