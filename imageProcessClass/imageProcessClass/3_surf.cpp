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
	cv::Mat imageR = this->imageR;//��û���ض��������£�Ӧ�û�Ĭ��Ϊ���Ա��������˲����¸�ֵ����Ҳ��
	/*system("color DF");
	cv::Mat imageL = cv::imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\t_outline.jpg");
	cv::Mat imageR = cv::imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\outline_image\\c_outline.jpg");*/




	//��ȡ�����㷽��
	//SIFT
	//cv::Ptr<cv::SIFT> sift = cv::SIFT::create();//openCV 4.5.2sift�㷨���ٷ���xfeature2d����nonfree��
	//ORB
	//cv::Ptr<cv::ORB> sift = cv::ORB::create();//orb��sift��ֱ��ʹ��
	//SURF
	cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(1000);//SURF�㷨����xfeature2d

	//sift������
	//std::vector<cv::KeyPoint> keyPointL, keyPointR;
	////������ȡ������
	//sift->detect(imageL, keyPointL);
	//sift->detect(imageR, keyPointR);

	//SURF��������
	int minHessian = 100;
	std::vector<cv::KeyPoint> keyPointL, keyPointR;
	surf->detect(imageL, keyPointL, Mat());//�ҳ��ؼ���
	surf->detect(imageR, keyPointR, Mat());


	//��������
	cv::Mat keyPointImageL;
	cv::Mat keyPointImageR;
	//sift
	drawKeypoints(imageL, keyPointL, keyPointImageL, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(imageR, keyPointR, keyPointImageR, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//surf
	/*drawKeypoints(imageL, keyPointL, keyPointImageL, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	drawKeypoints(imageR, keyPointR, keyPointImageR, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
*/
//��ʾ����
	//cv::namedWindow("KeyPoints of imageL");
	//cv::namedWindow("KeyPoints of imageR");//�����䲻��Ҳ�а�

	////��ʾ������
	//cv::imshow("KeyPoints of imageL", keyPointImageL);
	//cv::imshow("KeyPoints of imageR", keyPointImageR);

	//������ƥ��
	cv::Mat despL, despR;
	//��ȡ�����㲢��������������

	/*sift->detectAndCompute(imageL, cv::Mat(), keyPointL, despL);
	sift->detectAndCompute(imageR, cv::Mat(), keyPointR, despR);*/

	surf->detectAndCompute(imageL, cv::Mat(), keyPointL, despL);
	surf->detectAndCompute(imageR, cv::Mat(), keyPointR, despR);

	//Struct for DMatch: query descriptor index, train descriptor index, train image index and distance between descriptors.
	//int queryIdx �C>�ǲ���ͼ����������������� descriptor �����±꣬ͬʱҲ����������Ӧ�����㣨keypoint)���±ꡣ
	//int trainIdx �C> ������ͼ������������������±꣬ͬ��Ҳ����Ӧ����������±ꡣ
	//int imgIdx �C>�������Ƕ���ͼ��Ļ����á�
	//float distance �C>������һ��ƥ�������������������������������ŷ�Ͼ��룬��ֵԽСҲ��˵������������Խ����
	std::vector<cv::DMatch> matches;

	//������� flannBased ���� ��ô despͨ��orb�ĵ������Ͳ�ͬ��Ҫ��ת������
	if (despL.type() != CV_32F || despR.type() != CV_32F)
	{
		despL.convertTo(despL, CV_32F);
		despR.convertTo(despR, CV_32F);
	}

	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("FlannBased");
	matcher->match(despL, despR, matches);

	//�����������������ֵ 
	double maxDist = 0;
	for (int i = 0; i < despL.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist > maxDist)
			maxDist = dist;
	}

	//��ѡ�õ�ƥ���
	std::vector< cv::DMatch > good_matches;
	for (int i = 0; i < despL.rows; i++)
	{
		if (matches[i].distance < 0.5 * maxDist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//cv::Mat imageOutput;//������������imageOutput��û���ض��������£�����������Ա����imgeOutput����thisָ��������
	//����Ӧ�û�Ĭ��Ϊ���Ա��������ʱ����thisָ��Ҳ��
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
	cv::Mat H = findHomography(obj, scene, inliers, RANSAC);//��ʱӦ�þ������ŵ�ӳ��任���� 3��3��
	
	//-- Draw matches with RANSAC
	//cv::Mat img_matches_ransac;
	std::vector<DMatch> good_matches_ransac;
	for (size_t i = 0; i < inliers.size(); i++)
	{
		if (inliers[i])//inliers[i]=1,�����good_matchers_ransac
		{
			good_matches_ransac.push_back(good_matches[i]);
		}
	}
	cv::drawMatches(imageL, keyPointL, imageR, keyPointR, good_matches_ransac, this->img_matches_ransac, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);//��û���ض��������£�Ӧ�û�Ĭ��Ϊ���Ա��������˲���thisָ��Ҳ��
							

	//Mat perspectiveImg, affinedImgL;
	//Mat cImg = imread("D:\\Visual Studio\\Cpro\\image_process\\image_process\\Init_image\\FLIR0003 (2).jpg");
	Mat cImg = this->colorImg;
	//Mat T = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 0);
	//imshow("cImg", cImg);

	//1.�õ�ӳ�����H����͸�ӱ任
	warpPerspective(cImg, this->perspectiveImg, H, imageL.size());
	//imshow("perspectiveImg", perspectiveImg);
	//2.�÷������T������з���任
	//1����matlab����ͬ��2��Ŀ���͸�ӱ任Ч��(?)��
	//3������Project Properties > Configuration Properties > C/C++ > General > SDL checks�ص�
	//��֪���᲻��Ӱ��dll������
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
	//3.ÿ��ȡ�ĸ���Ӧ�����������Ȼ��ȡƽ��ֵ

	//imshow("affinedImgL", affinedImgL);
	//cv::namedWindow("img_matches_ransac");
	//cv::imshow("img_matches_ransac", img_matches_ransac);

	//cv::waitKey(0);
	return 0;
}

surfRansac::~surfRansac()
{

}