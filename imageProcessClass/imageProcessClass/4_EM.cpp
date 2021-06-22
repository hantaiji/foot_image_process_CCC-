#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

EMCluster::EMCluster(int i)
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
	this->imgOrigin= imread(this->path);
	myEM();
}
//overload
EMCluster::EMCluster(Mat & img, Mat & therImg)
{
	this->imgOrigin = img;
	this->imgTher = therImg;
	myEM();
}

int EMCluster::myEM()
{
	//用于给不同分类结果上色
	Vec3b colors[] = { Vec3b(0, 0, 255), Vec3b(0, 255, 0), Vec3b(255, 100, 100), Vec3b(255, 0, 255) };
	Mat data, labels, src, dst, thermalImage, segedTherImg;
	//src = imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR0003 (2).jpg", 1);
	src = this->imgOrigin;
	thermalImage = this->imgTher;
	/*imshow("yuanshisrc", src);
	resize(src, src, Size(src.cols / 1.5, src.rows / 1.5));*/
	if (src.empty())
	{
		printf("can not load image \n");
		return -1;
	}
	src.copyTo(dst);//等于相当于引用，修改一个会影响另一个，如果不想互相影响应使用copyTo函数
	thermalImage.copyTo(segedTherImg);
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b point = src.at<Vec3b>(i, j);
			Mat tmp = (Mat_<float>(1, 3) << point[0], point[1], point[2]);
			data.push_back(tmp);
		}

	Ptr<EM> model = EM::create();
	model->setClustersNumber(2); //类个数
	model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	model->setTermCriteria(TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	model->trainEM(data, noArray(), labels, noArray());

	//int n = 0;
	////显示结果，不同的类别用不同的颜色
	//for (int i = 0; i < dst.rows; i++)
	//{
	//	for (int j = 0; j < dst.cols; j++)
	//	{
	//		int index = labels.at<int>(n);
	//		dst.at<Vec3b>(i, j) = colors[index];
	//		n++;
	//	}
	//}

	int index = labels.at<int>(1), n = 0;//所有与（0,0）点像素标签相同的点（应该为背景），置为0
	for (int i = 0; i < dst.rows; i++)
		for (int j = 0; j < dst.cols; j++)
		{
			//int index = 1;
			if (labels.at<int>(n) == index)
			{
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0); //彩色图像分割后的脚底图像
				segedTherImg.at<Vec3b>(i, j) = Vec3b(0, 0, 0);//红外图像分割后的脚底图像
			}
			n++;
		}
	this->imgSeged = dst;
	this->timgSeged = segedTherImg;

	//imshow("src", src);
	//imshow("dst", dst);
	//waitKey(0);

	return 0;
}

EMCluster::~EMCluster()
{

}