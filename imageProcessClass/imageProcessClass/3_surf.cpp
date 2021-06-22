#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"

surfRansac::surfRansac(int i, Mat &Cimg, Mat &Timg)
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
	this->imageL = Cimg;
	this->imageR = Timg;
	my_ransac();
}

int surfRansac::my_ransac()
{
	cv::Mat imageL = this->imageL;
	cv::Mat imageR = this->imageR;//在没有重定义的情况下，应该会默认为类成员变量，因此不重新赋值好像也行
	/*system("color DF");
	cv::Mat imageL = cv::imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\t_outline.jpg");
	cv::Mat imageR = cv::imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\c_outline.jpg");*/




	//提取特征点方法
	//SIFT
	//cv::Ptr<cv::SIFT> sift = cv::SIFT::create();//openCV 4.5.2sift算法不再放在xfeature2d或者nonfree库
	//ORB
	//cv::Ptr<cv::ORB> sift = cv::ORB::create();//orb改sift可直接使用
	//SURF
	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(1000);//SURF算法放在xfeature2d

	//sift特征点
	//std::vector<cv::KeyPoint> keyPointL, keyPointR;
	////单独提取特征点
	//sift->detect(imageL, keyPointL);
	//sift->detect(imageR, keyPointR);

	//SURF特征点检测
	int minHessian = 100;
	std::vector<cv::KeyPoint> keyPointL, keyPointR;
	surf->detect(imageL, keyPointL, Mat());//找出关键点
	surf->detect(imageR, keyPointR, Mat());


	//画特征点
	cv::Mat keyPointImageL;
	cv::Mat keyPointImageR;
	//sift
	drawKeypoints(imageL, keyPointL, keyPointImageL, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(imageR, keyPointR, keyPointImageR, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//surf
	/*drawKeypoints(imageL, keyPointL, keyPointImageL, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	drawKeypoints(imageR, keyPointR, keyPointImageR, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
*/
//显示窗口
	//cv::namedWindow("KeyPoints of imageL");
	//cv::namedWindow("KeyPoints of imageR");//这两句不加也行吧

	////显示特征点
	//cv::imshow("KeyPoints of imageL", keyPointImageL);
	//cv::imshow("KeyPoints of imageR", keyPointImageR);

	//特征点匹配
	cv::Mat despL, despR;
	//提取特征点并计算特征描述子

	/*sift->detectAndCompute(imageL, cv::Mat(), keyPointL, despL);
	sift->detectAndCompute(imageR, cv::Mat(), keyPointR, despR);*/

	surf->detectAndCompute(imageL, cv::Mat(), keyPointL, despL);
	surf->detectAndCompute(imageR, cv::Mat(), keyPointR, despR);

	//Struct for DMatch: query descriptor index, train descriptor index, train image index and distance between descriptors.
	//int queryIdx –>是测试图像的特征点描述符（ descriptor ）的下标，同时也是描述符对应特征点（keypoint)的下标。
	//int trainIdx –> 是样本图像的特征点描述符的下标，同样也是相应的特征点的下标。
	//int imgIdx –>当样本是多张图像的话有用。
	//float distance –>代表这一对匹配的特征点描述符（本质是向量）的欧氏距离，数值越小也就说明两个特征点越相像。
	std::vector<cv::DMatch> matches;

	//如果采用 flannBased 方法 那么 desp通过orb的到的类型不同需要先转换类型
	if (despL.type() != CV_32F || despR.type() != CV_32F)
	{
		despL.convertTo(despL, CV_32F);
		despR.convertTo(despR, CV_32F);
	}

	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");
	matcher->match(despL, despR, matches);

	//计算特征点距离的最大值 
	double maxDist = 0;
	for (int i = 0; i < despL.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist > maxDist)
			maxDist = dist;
	}

	//挑选好的匹配点
	std::vector< cv::DMatch > good_matches;
	for (int i = 0; i < despL.rows; i++)
	{
		if (matches[i].distance < 0.5 * maxDist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//cv::Mat imageOutput;//假如重新声明imageOutput在没有重定义的情况下，则下面的类成员变量imgeOutput需用this指针来区分
	//否则应该会默认为类成员变量，那时不加this指针也行
	cv::drawMatches(imageL, keyPointL, imageR, keyPointR, good_matches, this->imageOutput);

	/*cv::namedWindow("picture of matching");
	cv::imshow("picture of matching", imageOutput);*/


	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keyPointL[good_matches[i].queryIdx].pt);
		scene.push_back(keyPointR[good_matches[i].trainIdx].pt);
	}

	std::vector<uchar>inliers;
	cv::Mat H = findHomography(obj, scene, inliers, RANSAC);//这时应该就是最优单映射变换矩阵 3行3列
	
	//-- Draw matches with RANSAC
	//cv::Mat img_matches_ransac;
	std::vector<DMatch> good_matches_ransac;
	for (size_t i = 0; i < inliers.size(); i++)
	{
		if (inliers[i])//inliers[i]=1,则加入good_matchers_ransac
		{
			good_matches_ransac.push_back(good_matches[i]);
		}
	}
	cv::drawMatches(imageL, keyPointL, imageR, keyPointR, good_matches_ransac, this->img_matches_ransac, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);//在没有重定义的情况下，应该会默认为类成员变量，因此不加this指针也行
							

	//Mat perspectiveImg, affinedImgL;
	//Mat cImg = imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR0003 (2).jpg");
	Mat cImg = this->colorImg;
	//Mat T = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 0);
	//imshow("cImg", cImg);

	//1.用单映射矩阵H进行透视变换
	warpPerspective(cImg, this->perspectiveImg, H, imageL.size());
	//imshow("perspectiveImg", perspectiveImg);
	//2.用仿射矩阵T矩阵进行仿射变换
	//1）与matlab版相同；2）目测比透视变换效果(?)；
	//3）需在Project Properties > Configuration Properties > C/C++ > General > SDL checks关掉
	//不知道会不会影响dll的生成
	std::vector<Point2f> affine_obj;
	std::vector<Point2f> affine_scene;
	for (size_t i = 0; i < good_matches_ransac.size(); i++)
	{
		//-- Get the keypoints from the good matches
		affine_obj.push_back(keyPointL[good_matches_ransac[i].queryIdx].pt);
		affine_scene.push_back(keyPointR[good_matches_ransac[i].trainIdx].pt);
	}

	Mat T = estimateRigidTransform(affine_obj, affine_scene, 1);
	warpAffine(cImg, this->affinedImgL, T, imageL.size());
	//std::vector<uchar>inliers;
	//cv::Mat H = findHomography(obj, scene, inliers, RANSAC);
	//3.每次取四个对应点计算仿射矩阵，然后取平均值

	//imshow("affinedImgL", affinedImgL);
	//cv::namedWindow("img_matches_ransac");
	//cv::imshow("img_matches_ransac", img_matches_ransac);

	//cv::waitKey(0);
	return 0;
}

surfRansac::~surfRansac()
{

}