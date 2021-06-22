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
	//���ڸ���ͬ��������ɫ
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
	src.copyTo(dst);//�����൱�����ã��޸�һ����Ӱ����һ����������뻥��Ӱ��Ӧʹ��copyTo����
	thermalImage.copyTo(segedTherImg);
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b point = src.at<Vec3b>(i, j);
			Mat tmp = (Mat_<float>(1, 3) << point[0], point[1], point[2]);
			data.push_back(tmp);
		}

	Ptr<EM> model = EM::create();
	model->setClustersNumber(2); //�����
	model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
	model->setTermCriteria(TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 0.1));
	model->trainEM(data, noArray(), labels, noArray());

	//int n = 0;
	////��ʾ�������ͬ������ò�ͬ����ɫ
	//for (int i = 0; i < dst.rows; i++)
	//{
	//	for (int j = 0; j < dst.cols; j++)
	//	{
	//		int index = labels.at<int>(n);
	//		dst.at<Vec3b>(i, j) = colors[index];
	//		n++;
	//	}
	//}

	int index = labels.at<int>(1), n = 0;//�����루0,0�������ر�ǩ��ͬ�ĵ㣨Ӧ��Ϊ����������Ϊ0
	for (int i = 0; i < dst.rows; i++)
		for (int j = 0; j < dst.cols; j++)
		{
			//int index = 1;
			if (labels.at<int>(n) == index)
			{
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0); //��ɫͼ��ָ��Ľŵ�ͼ��
				segedTherImg.at<Vec3b>(i, j) = Vec3b(0, 0, 0);//����ͼ��ָ��Ľŵ�ͼ��
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