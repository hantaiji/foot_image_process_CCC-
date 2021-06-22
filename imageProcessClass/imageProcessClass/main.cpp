#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"
#include "7_rotate.h"
#include "8_zone.h"

//reduce_ther test(int num)
//{
//	reduce_ther my_reduce(num);
//	return my_reduce;
//}

int main()
{
	//1.降低红外图像分辨率
	reduce_ther my_reduce(89);
	//reduce_ther my_reduce = test(89);

	//imshow("原图像", my_reduce.originImg);
	//imshow("分辨率降低后的图像", my_reduce.reducedImg);

	//2.提取彩色图像/红外图像轮廓
	outLine my_outline(89, my_reduce.reducedImg);
	//imshow("原彩色图像", my_outline.colorImg);
	//imshow("彩色图像轮廓", my_outline.cannyCResult);
	//imshow("红外图像轮廓", my_outline.cannyTResult);

	//3.提取surf体征点，并用Ransac算法提纯，最后分别进行透视变换和仿射变换
	surfRansac my_surfRansac(89, my_outline.cannyCResult, my_outline.cannyTResult);
	//imshow("picture of matching", my_surfRansac.imageOutput);
	//imshow("img_matches_ransac", my_surfRansac.img_matches_ransac);
	//imshow("perspectiveImgL", my_surfRansac.perspectiveImg);
	//imshow("affinedImgL", my_surfRansac.affinedImgL);

	//4.将透视变换和仿射变换后的图像进行图像分割，观察哪个效果更好
	EMCluster my_EM1(my_surfRansac.perspectiveImg, my_reduce.reducedImg);
	EMCluster my_EM2(my_surfRansac.affinedImgL, my_reduce.reducedImg);
	//imshow("imgOrigin1", my_EM1.imgOrigin);//透视变换后彩色图像原图
	//imshow("imgSeged1", my_EM1.imgSeged);//透视变换彩色图像后 EM聚类分割获得的脚底图像
	//imshow("timgSeged1", my_EM1.timgSeged);//透视变换条件下：分割结果应用到红外图像后的脚底图像

	//imshow("imgOrigin2", my_EM2.imgOrigin);//仿射变换后彩色图像原图
	//imshow("imgSeged2", my_EM2.imgSeged);//仿射变换彩色图像后 EM聚类分割获得的脚底图像
	//imshow("timgSeged2", my_EM2.timgSeged);//仿射变换条件下：分割结果应用到红外图像后的脚底图像
	//waitKey(0); 

	readCSV my_readCSV(89, my_EM1.timgSeged);
	/*imshow("左足红外图像", my_readCSV.thermalMat[0]);
	imshow("右足红外图像", my_readCSV.thermalMat[1]);
	imshow("左足温度矩阵", my_readCSV.csvMat[0]);
	imshow("右足温度矩阵", my_readCSV.csvMat[1]);
	waitKey(0);*/

	tempCom my_tempComL(my_readCSV.csvMat[0], my_readCSV.thermalMat[0]);//右足温度特征计算
	//cout << my_tempComL.tempMax << " " << my_tempComL.tempMin << " " << my_tempComL.tempAver <<" "
	//	<< my_tempComL.tempVar << " " << my_tempComL.tempStd << endl;
	tempCom my_tempComR(my_readCSV.csvMat[1], my_readCSV.thermalMat[1]);//左足温度特征计算
	//cout << my_tempComR.tempMax << " " << my_tempComR.tempMin << " " << my_tempComR.tempAver << " "
	//	<< my_tempComR.tempVar << " " << my_tempComR.tempStd << endl;
	//system("pause");
	//rotateMat my_rotateR(my_readCSV.thermalMat[0], my_readCSV.csvMat[0]);
	//rotateMat my_rotateL(my_readCSV.thermalMat[1], my_readCSV.csvMat[1]);
	rotateMat my_rotateR(my_readCSV.thermalMat[0], my_readCSV.unRigisterMat[0]);
	rotateMat my_rotateL(my_readCSV.thermalMat[1], my_readCSV.unRigisterMat[1]);

	Mat flipLeft;
	flip(my_rotateL.rotatedTemp, flipLeft, 1);//左足镜像翻转
	//imshow("flipLeft", flipLeft);
	zoneAnalysis my_zoneR(my_rotateR.rotatedTemp);
	zoneAnalysis my_zoneL(flipLeft);
	
	//waitKey();
	return 0;
}