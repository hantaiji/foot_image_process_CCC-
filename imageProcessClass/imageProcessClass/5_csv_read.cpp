#include "1_reduce_thermal.h"
#include "2_outline.h"
#include "3_surf.h"
#include "4_EM.h"
#include "5_csv_read.h"

using namespace std;
using namespace cv;

readCSV::readCSV(int i, Mat & timgSeged)
{
	if (i < 10)
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\csv_file\\FLIR000";
		string q = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\cut_Csv\\FLIR000";
		this->path = p + to_string(i) + ".csv";
		this->cutCsvPathL = q + to_string(i) + "L.xls";
		this->cutCsvPathR = q + to_string(i) + "R.xls";
	}
	else
	{
		string p = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\csv_file\\FLIR00";
		string q = "D:\\Visual Studio\\Cpro\\image_process\\image_process\\cut_Csv\\FLIR00";
		this->path = p + to_string(i) + ".csv";
		this->cutCsvPathL = q + to_string(i) + "L.xls";
		this->cutCsvPathR = q + to_string(i) + "R.xls";
	}
	this->thermalSeged = timgSeged;
	csvRead();
	//���ﻹ���ټ�һ��my_Cut����ȡδ��׼ǰ���������¶Ⱦ���
	//Ϊ������ת��׾����������Ա�����ת������ص�ʱ�����¶ȹ�С�ĵ㣩��׼��
	Mat originMat;
	this->tempMat.copyTo(originMat);
	this->unRigisterMat = my_Cut(originMat);//��ʱtempMat��δ��׼
	csvRigister();
	this->thermalMat = my_Cut(this->thermalSeged);
	this->csvMat = my_Cut(this->tempMat);
	//xlsWrite(i, this->csvMat[0],this->cutCsvPathL);
	//xlsWrite(i, this->csvMat[1], this->cutCsvPathR);
}

int readCSV::csvRead()
{
	vector<vector<float>> csv_arr;
	vector<vector<float>>::iterator iter;
	ifstream fp(this->path); //��������һ��ifstream����ָ���ļ�·��
	string line;
	for (int i = 0; i < 250; i++)
	{
		if (i < 10)
		{
			getline(fp, line);
		}	
		else
		{
			getline(fp, line);
			vector<float> data_line;
			string number;
			istringstream readstr(line); //string��������
			//��һ�����ݰ�'��'�ָ�
			for (int j = 0; j < 321; j++) { //�ɸ������ݵ�ʵ�����ȡѭ����ȡ
				getline(readstr, number, ','); //ѭ����ȡ����
				data_line.push_back(atof(number.c_str()));////�ַ���תfloat
			}
			data_line.erase(data_line.begin());
			csv_arr.push_back(data_line); //���뵽vector��
		}
	}
	Mat originMat(0, csv_arr[0].size(), DataType<float>::type);
	//�������� cout���������λС�������λ��������at�����ʺ���û������
	for (int i = 0; i < csv_arr.size(); ++i)
	{
		stringstream ss;
		ss << csv_arr[i].data();
		// Make a temporary cv::Mat row and add to NewSamples _without_ data copy
		Mat Sample(1, csv_arr[0].size(), DataType<float>::type, csv_arr[i].data());
		originMat.push_back(Sample);
	}
	/*for (int i = 0; i < 240; i++)
	{
		for (int j = 0; j < 320; j++)
		{

			cout << originMat.at<float>(i, j) << " ";
		}
		cout << endl;
	}*/
	this->tempMat = originMat;
	return 0;
}

vector<Mat> readCSV::my_Cut(Mat & whole)
{
	vector<Mat> ceilImg;
	int height = whole.rows;
	int width = whole.cols;
	int ceil_height = (int) height;
	int ceil_width = (int)(width / 2);
	for (int j = 0; j < 2; j++)
	{
		if (j == 0)
		{
			Rect rect(0, 0, ceil_width, ceil_height);
			ceilImg.push_back(whole(rect));
		}
		else
		{
			Rect rect(ceil_width, 0, ceil_width, ceil_height);
			ceilImg.push_back(whole(rect));
		}
	}
	return ceilImg;
}

int readCSV::csvRigister()
{
	Mat grayThermal;
	//��׼��ĺ���ͼ��ת�ɻҶ�ͼ�� typeΪ0����CV_8U uchar
	cvtColor(this->thermalSeged, grayThermal, COLOR_BGR2GRAY);
	for (int i = 0; i < grayThermal.rows; i++)
	{
		for (int j = 0; j < grayThermal.cols; j++)
		{
			//grayThermal.at<uchar>,�����������ͱ������������һ�£���������at���������ط���
			if (grayThermal.at<uchar>(i, j) == 0)
			{
				//typeΪ5����CV_32F float
				this->tempMat.at<float>(i, j) = 0;
			}
		}
	}
	return  0;
}

void readCSV::xlsWrite(int i,Mat & t,string p)
{

	ofstream Fs(p);
	if (!Fs.is_open())
	{
		cout << "error!" << endl;
		return;
	}
	int height = t.rows;
	int width = t.cols;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Fs << t.at<float>(i, j) << '\t';
		}
		Fs << endl;
	}
	Fs.close();
}

readCSV::~readCSV()
{

}