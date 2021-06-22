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
	//1.���ͺ���ͼ��ֱ���
	reduce_ther my_reduce(89);
	//reduce_ther my_reduce = test(89);

	//imshow("ԭͼ��", my_reduce.originImg);
	//imshow("�ֱ��ʽ��ͺ��ͼ��", my_reduce.reducedImg);

	//2.��ȡ��ɫͼ��/����ͼ������
	outLine my_outline(89, my_reduce.reducedImg);
	//imshow("ԭ��ɫͼ��", my_outline.colorImg);
	//imshow("��ɫͼ������", my_outline.cannyCResult);
	//imshow("����ͼ������", my_outline.cannyTResult);

	//3.��ȡsurf�����㣬����Ransac�㷨�ᴿ�����ֱ����͸�ӱ任�ͷ���任
	surfRansac my_surfRansac(89, my_outline.cannyCResult, my_outline.cannyTResult);
	//imshow("picture of matching", my_surfRansac.imageOutput);
	//imshow("img_matches_ransac", my_surfRansac.img_matches_ransac);
	//imshow("perspectiveImgL", my_surfRansac.perspectiveImg);
	//imshow("affinedImgL", my_surfRansac.affinedImgL);

	//4.��͸�ӱ任�ͷ���任���ͼ�����ͼ��ָ�۲��ĸ�Ч������
	EMCluster my_EM1(my_surfRansac.perspectiveImg, my_reduce.reducedImg);
	EMCluster my_EM2(my_surfRansac.affinedImgL, my_reduce.reducedImg);
	//imshow("imgOrigin1", my_EM1.imgOrigin);//͸�ӱ任���ɫͼ��ԭͼ
	//imshow("imgSeged1", my_EM1.imgSeged);//͸�ӱ任��ɫͼ��� EM����ָ��õĽŵ�ͼ��
	//imshow("timgSeged1", my_EM1.timgSeged);//͸�ӱ任�����£��ָ���Ӧ�õ�����ͼ���Ľŵ�ͼ��

	//imshow("imgOrigin2", my_EM2.imgOrigin);//����任���ɫͼ��ԭͼ
	//imshow("imgSeged2", my_EM2.imgSeged);//����任��ɫͼ��� EM����ָ��õĽŵ�ͼ��
	//imshow("timgSeged2", my_EM2.timgSeged);//����任�����£��ָ���Ӧ�õ�����ͼ���Ľŵ�ͼ��
	//waitKey(0); 

	readCSV my_readCSV(89, my_EM1.timgSeged);
	/*imshow("�������ͼ��", my_readCSV.thermalMat[0]);
	imshow("�������ͼ��", my_readCSV.thermalMat[1]);
	imshow("�����¶Ⱦ���", my_readCSV.csvMat[0]);
	imshow("�����¶Ⱦ���", my_readCSV.csvMat[1]);
	waitKey(0);*/

	tempCom my_tempComL(my_readCSV.csvMat[0], my_readCSV.thermalMat[0]);//�����¶���������
	//cout << my_tempComL.tempMax << " " << my_tempComL.tempMin << " " << my_tempComL.tempAver <<" "
	//	<< my_tempComL.tempVar << " " << my_tempComL.tempStd << endl;
	tempCom my_tempComR(my_readCSV.csvMat[1], my_readCSV.thermalMat[1]);//�����¶���������
	//cout << my_tempComR.tempMax << " " << my_tempComR.tempMin << " " << my_tempComR.tempAver << " "
	//	<< my_tempComR.tempVar << " " << my_tempComR.tempStd << endl;
	//system("pause");
	//rotateMat my_rotateR(my_readCSV.thermalMat[0], my_readCSV.csvMat[0]);
	//rotateMat my_rotateL(my_readCSV.thermalMat[1], my_readCSV.csvMat[1]);
	rotateMat my_rotateR(my_readCSV.thermalMat[0], my_readCSV.unRigisterMat[0]);
	rotateMat my_rotateL(my_readCSV.thermalMat[1], my_readCSV.unRigisterMat[1]);

	Mat flipLeft;
	flip(my_rotateL.rotatedTemp, flipLeft, 1);//���㾵��ת
	//imshow("flipLeft", flipLeft);
	zoneAnalysis my_zoneR(my_rotateR.rotatedTemp);
	zoneAnalysis my_zoneL(flipLeft);
	
	//waitKey();
	return 0;
}