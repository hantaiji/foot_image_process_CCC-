#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"
#include "6_tempCompute.h"

tempCom::tempCom(Mat & t,Mat & seged)
{
	this->_toComMat = t;
	this->segedThermal = seged;
	/*for (int i = 0; i < 240; i++)
	{
		for (int j = 0; j < 160; j++)
		{

			cout<<t.at<float>(i, j)<<endl;
		}
	}*/
	vector<float> v = comFunction(this->_toComMat);//将所有足底温度数据装入向量容器v
	tempFeature(v);//计算温度特征
	position(this->_toComMat);
}

vector<float> tempCom::comFunction(Mat & m)
{
	vector<float> que;
	int height = m.rows;
	int width = m.cols;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (m.at<float>(i, j) != 0)
			{
				que.push_back(m.at<float>(i, j));
			}
		}
	}
	/*for (int i = 0; i < que.size(); i++)
	{
		cout << que[i] << endl;
	}*/
	return que;
}

void tempCom::tempFeature(vector<float> & v)
{
	float max = *max_element(v.begin(), v.end());//最大值
	float min = *min_element(v.begin(), v.end());//最小值
	// 求均值
	float sum = accumulate(std::begin(v), std::end(v), 0.0);
	float mean =  sum / v.size();
    
    // 求方差与标准差
	float variance  = 0.0;
    for (int i = 0 ; i < v.size() ; i++)
    {
        variance = variance + pow(v[i]-mean,2);
    }
    variance = variance/v.size();
	float standard_deviation = sqrt(variance);
	//cout << max << " " << min << " " << mean << " " << variance << " " << standard_deviation << endl;
	this->tempMax = max; this->tempMin = min; this->tempAver = mean; 
	this->tempVar = variance; this->tempStd = standard_deviation;
}

void tempCom::position(Mat & t)
{
	vector<vector<int> > positionMax(10,vector<int>(2)), positionMin(10, vector<int>(2));
	int countMax = 0, countMin = 0;
	//cout << t.rows << endl;
	//cout << this->tempMax << endl;
	//寻找最大值最小值位置
	for (int i = 0; i < t.rows; i++)
	{
		for (int j = 0; j < t.cols; j++)
		{
			
			if (t.at<float>(i,j) == this->tempMax)
			{
				positionMax[countMax][0]=i;
				positionMax[countMax][1]=j;
				//cout << positionMax[countMax][0] << " " << positionMax[countMax][1] << endl;
				countMax++;
			}
			if (t.at<float>(i, j) == this->tempMin)
			{
				positionMin[countMin][0]=i;
				positionMin[countMin][1]=j;
				//cout << positionMin[countMin][0] << " " << positionMin[countMin][1] << endl;
				countMin++;
			}
		}		
	}
	//遍历最大值位置向量，可删除
	/*for (int i = 0; i < positionMax.size(); i++)
	{
		for (int j = 0; j < positionMax[0].size(); j++)
		{
			cout << positionMax[i][j] << " " ;
		}
		cout << endl;
	}*/
	//cout << positionMax.size() << endl;

	//在足底红外图像上用圆圈标记最大值、最小值
	//for (int i = 0; i < countMax; i++)
	//{
	//	int point_x = positionMax[i][1];//圆心坐标x值，对应列
	//	int point_y = positionMax[i][0];//圆心坐标y值，对应行
	//	//圆形绘画
	//	circle(this->segedThermal, Point(point_x, point_y), 4, Scalar(0, 0, 255), 1);
	//	imshow("src", this->segedThermal);
	//	//waitKey(0);
	//}
	//for (int i = 0; i < countMin; i++)
	//{
	//	int point_x = positionMin[i][1];//圆心坐标x值
	//	int point_y = positionMin[i][0];//圆心坐标y值
	//	//圆形绘画
	//	circle(this->segedThermal, Point(point_x, point_y), 4, Scalar(0, 255, 0), 1);
	//	imshow("src", this->segedThermal);
	//	waitKey(0);
	//}
}

tempCom::~tempCom()
{

}