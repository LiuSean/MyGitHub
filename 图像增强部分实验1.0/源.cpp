#include<iostream>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/opencv.hpp"  
#include <opencv2/core/core.hpp>
#include<iostream>  
#include<vector>  
#include<fstream>
#include<set>
#include<cstdlib>
#include<cmath>
#include<ctime> 
using namespace std;
using namespace cv;
void LogEnhance(IplImage* img, IplImage* dst);
void ExporeOver(IplImage* img, IplImage* dst);
Mat HighPass(Mat img);
void ExporeOver(IplImage* img, IplImage* dst);
void Masic(IplImage* img, IplImage* dst, int nSize);
void ExpEnhance(IplImage* img, IplImage* dst);
uchar getPixel(IplImage* img, int row, int col, int k);
void setPixel(IplImage* img, int row, int col, int k, uchar val);
void ThreShold1(IplImage* img, IplImage* dst);
int main(int argc, char* argv[])
{
	const char* Path = "D://手势图片/4-25/1-1.png";
	IplImage *img = cvLoadImage(Path, CV_LOAD_IMAGE_ANYCOLOR);//获得指针
	IplImage *dst = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);

	cout << "输入你要选择的操作：" << endl;
	cout << "1、曝光过度" << endl;
	cout << "2、加马赛克" << endl;
	cout << "3、对数增强" << endl;
	cout << "4、指数增强" << endl;
	cout << "5、二值化" << endl;
	cout << "请输入你的选择:";

	int choice = 1;

	cin >> choice;
	switch (choice)
	{
	case 1:
		ExporeOver(img, dst);
		break;
	case 2:
		Masic(img, dst, 21);
		break;
	case 3:
		LogEnhance(img, dst);
		break;
	case 4:
		ExpEnhance(img, dst);
		break;
	case 5:
		ThreShold1(img,dst);
		break;
	default:
		cout << "输入错误" << endl;
		break;
	}
	cvSaveImage("E:\\dst.jpg", dst);
	cvNamedWindow("SRC", 1);
	cvNamedWindow("DST", 1);
	cvShowImage("SRC", img);
	cvShowImage("DST", dst);
	cvWaitKey();
	return 0;
}
Mat HighPass(Mat img)
{
	Mat temp;
	GaussianBlur(img, temp, Size(7, 7), 1.6, 1.6);

	int r = 3;
	Mat diff = img + r*(img - temp); //高反差保留算法
	return diff;
}
// 过度曝光原理：图像翻转，然后求原图与反图的最小值
void ExporeOver(IplImage* img, IplImage* dst)//曝光过度
{
	for (int row = 0; row <img->height; row++)
	{
		uchar *data = (uchar*)img->imageData + row* img->widthStep;
		uchar *dstData = (uchar*)dst->imageData + row* dst->widthStep;

		for (int col = 0; col<img->width; col++)
		{
			for (int k = 0; k<img->nChannels; k++)
			{
				uchar t1 = data[col*img->nChannels + k];
				uchar t2 = 255 - t1;
				dstData[col*img->nChannels + k] = min(t1, t2);
			}
		}
	}
}

uchar getPixel(IplImage* img, int row, int col, int k)
{
	return ((uchar*)img->imageData + row* img->widthStep)[col*img->nChannels + k];
}

void setPixel(IplImage* img, int row, int col, int k, uchar val)
{
	((uchar*)img->imageData + row* img->widthStep)[col*img->nChannels + k] = val;
}
// 将邻域的值用中心像素的值替换
// nSize:为尺寸大小，奇数
void Masic(IplImage* img, IplImage* dst, int nSize)//加马赛克
{
	int offset = (nSize - 1) / 2;
	for (int row = offset; row <img->height - offset; row = row + offset)
	{
		for (int col = offset; col<img->width - offset; col = col + offset)
		{
			int val0 = getPixel(img, row, col, 0);
			int val1 = getPixel(img, row, col, 1);
			int val2 = getPixel(img, row, col, 2);
			for (int m = -offset; m<offset; m++)
			{
				for (int n = -offset; n<offset; n++)
				{
					setPixel(dst, row + m, col + n, 0, val0);
					setPixel(dst, row + m, col + n, 1, val1);
					setPixel(dst, row + m, col + n, 2, val2);
				}
			}
		}
	}
}
void ExpEnhance(IplImage* img, IplImage* dst)//指数增强
{
	// 由于oldPixel:[1,256],则可以先保存一个查找表
	uchar lut[256] = { 0 };

	double temp = 1.0 / 255.0;

	for (int i = 0; i<255; i++)
	{
		lut[i] = (uchar)(temp*i*i + 0.5);
	}

	for (int row = 0; row <img->height; row++)
	{
		uchar *data = (uchar*)img->imageData + row* img->widthStep;
		uchar *dstData = (uchar*)dst->imageData + row* dst->widthStep;

		for (int col = 0; col<img->width; col++)
		{
			for (int k = 0; k<img->nChannels; k++)
			{
				uchar t1 = data[col*img->nChannels + k];
				dstData[col*img->nChannels + k] = lut[t1];
			}
		}
	}
}
void LogEnhance(IplImage* img, IplImage* dst)//对数增强
{
	// 由于oldPixel:[1,256],则可以先保存一个查找表
	uchar lut[256] = { 0 };

	double temp = 255 / log(256);

	for (int i = 0; i<255; i++)
	{
		lut[i] = (uchar)(temp* log(i + 1) + 0.5);
	}

	for (int row = 0; row <img->height; row++)
	{
		uchar *data = (uchar*)img->imageData + row* img->widthStep;
		uchar *dstData = (uchar*)dst->imageData + row* dst->widthStep;

		for (int col = 0; col<img->width; col++)
		{
			for (int k = 0; k<img->nChannels; k++)
			{
				uchar t1 = data[col*img->nChannels + k];
				dstData[col*img->nChannels + k] = lut[t1];
			}
		}
	}
}
void ThreShold1(IplImage* img, IplImage* dst)
{
	Mat imag, middle,result;//矩阵变量
	imag = img;	//将读入的彩色图像直接以灰度图像读入
	//namedWindow("原图", 1);
	//imshow("原图", imag);
	result = imag.clone();
	cvtColor(imag, middle, CV_RGB2GRAY);
	//进行二值化处理，选择30，200.0为阈值
	threshold(middle, result, 60, 200.0, CV_THRESH_BINARY);
	namedWindow("二值化图像");
	imshow("二值化图像", result);
	//waitKey();
}