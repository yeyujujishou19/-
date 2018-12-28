// OpenCVcallTensorFlow.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include "windows.h"
#include <time.h>     
#include <stdio.h>  
#include <stdlib.h>
#pragma comment(lib, "winmm.lib ")

using namespace std;

using namespace cv;

//===========ȫ�ֱ���=========================================
vector<string> vector_charName;  //�ַ�����
clock_t  start, end12;  //ͳ��ʱ��
double timesum = 0;     //ͳ��ʱ��
//===========ȫ�ֱ���=========================================


//===========��غ���======================================================================================================
//===========��ȡ����=========================================

//ͳ�ƺڵ����
int bSums(Mat src)
{
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = src.begin<uchar>();
	Mat_<uchar>::iterator itend = src.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)==0) counter += 1;//��ֵ�������ص���0����255
	}
	return counter;
}

//��������ͼƬ��������Ǻڰ�ͼ��img1�ǵײ�ͼƬ��img2���ϲ�ͼƬ�����ص��Ӻ��ͼƬ
Mat ImageOverlay(Mat img1, Mat img2)
{
	int h = img1.rows;
	int w = img1.cols;
	int rows = img2.rows;
	int cols = img2.cols;
	Mat imageROI = img1(Range((h - rows) / 2, rows + (h - rows) / 2), Range((w - cols) / 2, cols + (w - cols) / 2));
	//��2������һ��Mat���Ͳ������趨ROI����
	//Mat imageROI = img1(Rect((h - rows) / 2, (w - cols) / 2, img2.cols, img2.rows));

	//��3��������Ĥ
	//������Ĥ
	//Mat mask;
	//img2.copyTo(mask);
	//imshow("665", mask);
	////waitKey(0);

	//��4������Ĥ������ROI
	img2.copyTo(imageROI);
	//imshow("imageROI", imageROI);

	//��5����ʾ���
	//namedWindow("1 ����ROIʵ��ͼ�����ʾ������");
	//imshow("1 ����ROIʵ��ͼ�����ʾ������", img1);
	//waitKey(0);
	imshow("6666", img1);
	waitKey(0);
	return img1;
}

//����ױ�
Mat HandWhiteEdges(Mat image)
{
	//��ֵ��
	Mat thresh1;
	threshold(image, thresh1, 249, 255, CV_THRESH_BINARY);

	//OpenCV����ĽṹԪ��
	Mat  kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
	//����ͼ��
	dilate(thresh1, thresh1, kernel);

	int row = image.rows;
	int col = image.cols;
	int tempr0 = 0;   //����
	int tempr1 = 0;   //����
	int tempc0 = 0;   //����
	int tempc1 = 0;   //����

	for (int r = 0; r < row;r++) //���ϵ���
	{
		int sum_col = 0;
		for (int c = 0; c < col; c++)
		{
			sum_col += thresh1.at<uchar>(r, c);
		}
		if (sum_col != 255 * col)
		{
			tempr0 = r;
			break;
		}
	}

	for (int r = row-1; r >= 0; r--) //���µ���
	{
		int sum_col = 0;
		for (int c = 0; c < col; c++)
		{
			sum_col += thresh1.at<uchar>(r, c);
		}
		if (sum_col != 255 * col)
		{
			tempr1 = r;
			break;
		}
	}

	for (int c= 0; c < col; c++) //������
	{
		int sum_row = 0;
		for (int r = 0; r < row; r++)
		{
			sum_row += thresh1.at<uchar>(r, c);
		}
		if (sum_row != 255 * row)
		{
			tempc0 = c;
			break;
		}
	}

	for (int c = col-1; c >=0; c--) //������
	{
		int sum_row = 0;
		for (int r = 0; r < row; r++)
		{
			sum_row += thresh1.at<uchar>(r, c);
		}
		if (sum_row != 255 * row)
		{
			tempc1 = c;
			break;
		}
	}

	//����ȫ��ͼƬ
	Mat imageTemp(64, 64, CV_8UC1, Scalar(255));
	//imshow("66", imageTemp);
	//waitKey(0);
	if (tempr1 - tempr0 == 0 || tempc1 - tempc0 == 0) //�հ�ͼ
	{
		return imageTemp;
	}

	Mat imageROI = image(Range(tempr0,tempr1), Range(tempc0,tempc1));
	//imshow("66", imageROI);
	Mat rstImg = ImageOverlay(imageTemp, imageROI); //����ͼ��
	//��ֵ��
	Mat binary2;
	threshold(rstImg, binary2, 0, 255, CV_THRESH_OTSU);
	
	return binary2;
}

//��ȡ����
Mat SimpleGridFeature(Mat image)
{
	Mat new_img = HandWhiteEdges(image);  //�ױߴ���
	//imwrite("D:\\du_cpp2.jpg", new_img);
	//imshow("22", new_img);
	//waitKey(0);

	resize(new_img, image, Size(64, 64));
	int img_h = image.rows;
	int img_w = image.cols;

	//��ֵ��
	Mat binary;
	threshold(image, binary,0, 255.0, CV_THRESH_OTSU);
	//imshow("66", binary);
	//waitKey();

	//���������С
	int grid_size = 16;
	int grid_h = img_h / grid_size;
	int grid_w = img_w / grid_size;

	//������������
	Mat img_feature(1, grid_size*grid_size, CV_32FC1, Scalar(0));
	for (int h = 0; h < grid_size; h++)
	{
		for (int w = 0; w < grid_size; w++)
		{
			Mat imageROI = binary(Range(h*grid_h,(h+1)*grid_h), Range(w*grid_w, (w+1)*grid_w));
			//imshow("66", imageROI);
			//waitKey(0);
			int a = bSums(imageROI);
			img_feature.at<float>(0, h*grid_size + w)=bSums(imageROI);
		}
	}
	return img_feature;
}
//===========��ȡ����=========================================

//����          ���飬 ���鳤�� ���±�
void Mysort(Mat a, int length, int* b)
{
	float t;
	int i, j, t1;
	for (j = 0; j<length; j++)
		for (i = 0; i<length - 1 - j; i++)
			if (a.at<float>(0, i)<a.at<float>(0, i+1))
			{
				t = a.at<float>(0, i);
				a.at<float>(0, i) = a.at<float>(0, i + 1);
				a.at<float>(0, i + 1) = t;

				t1 = b[i];
				b[i] = b[i + 1];
				b[i + 1] = t1;
			}
}

//��ȡtxt�ĵ�
void readTxt(string file)
{
	ifstream myfile(file);

	string temp;
	if (!myfile.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
	}
	while (getline(myfile, temp))
	{
		vector_charName.push_back(temp);
		//cout << temp << endl;;
	}
	myfile.close();
}

// LPCWSTRתstring
std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

// ����winWIN32_FIND_DATA��ȡ�ļ��µ��ļ���
void readImgNamefromFile(char* fileName, vector <string> &imgNames)
{
	// vector���� ��������
	imgNames.clear();
	WIN32_FIND_DATA file;
	int i = 0;
	char tempFilePath[MAX_PATH + 1];
	char tempFileName[50];
	// ת�������ļ���
	sprintf_s(tempFilePath, "%s/*", fileName);
	// ���ֽ�ת��
	WCHAR   wstr[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_ACP, 0, tempFilePath, -1, wstr, sizeof(wstr));
	// ���Ҹ��ļ��������ļ���������Զ�ȡ��WIN32_FIND_DATA
	HANDLE handle = FindFirstFile(wstr, &file);
	if (handle != INVALID_HANDLE_VALUE)
	{
		FindNextFile(handle, &file);
		FindNextFile(handle, &file);
		// ѭ�������õ��ļ��е������ļ���    
		do
		{
			sprintf_s(tempFileName, "%s", fileName);
			imgNames.push_back(WChar2Ansi(file.cFileName));
			imgNames[i].insert(0, tempFileName);
			i++;
		} while (FindNextFile(handle, &file));
	}
	FindClose(handle);
}

//��Matͼ�񵽶������ļ�
bool ImageToStreamFile(cv::Mat image, string filename)
{
	if (image.empty()) return false;
	const char *filenamechar = filename.c_str();
	FILE *fpw = fopen(filenamechar, "wb");//���û���򴴽�������������ͷ��ʼд
	if (fpw == NULL)
	{
		fclose(fpw);
		return false;
	}
	int channl = image.channels();//��һ���ֽ�  ͨ��
	int rows = image.rows;     //�ĸ��ֽڴ� ����
	int cols = image.cols;   //�ĸ��ֽڴ� ����

	fwrite(&channl, sizeof(char), 1, fpw);
	fwrite(&rows, sizeof(char), 4, fpw);
	fwrite(&cols, sizeof(char), 4, fpw);
	char* dp = (char*)image.data;
	if (channl == 3)
	{
		for (int i = 0; i < rows*cols; i++)
		{
			fwrite(&dp[i * 3], sizeof(char), 1, fpw);
			fwrite(&dp[i * 3 + 1], sizeof(char), 1, fpw);
			fwrite(&dp[i * 3 + 2], sizeof(char), 1, fpw);
		}
	}
	else if (channl == 1)
	{
		for (int i = 0; i < rows*cols; i++)
		{
			fwrite(&dp[i], sizeof(char), 1, fpw);
		}
	}
	fclose(fpw);
	return true;
}

//�Ӷ������ļ���Mat
bool StreamFileToImage(std::string filename, cv::Mat &image)
{
	const char *filenamechar = filename.c_str();
	FILE *fpr = fopen(filenamechar, "rb");
	if (fpr == NULL)
	{
		fclose(fpr);
		return false;
	}
	int channl(0);
	int imagerows(0);
	int imagecols(0);
	fread(&channl, sizeof(char), 1, fpr);//��һ���ֽ� ͨ��
	fread(&imagerows, sizeof(char), 4, fpr); //�ĸ��ֽڴ� ����
	fread(&imagecols, sizeof(char), 4, fpr); //�ĸ��ֽڴ� ����
	if (channl == 3)
	{
		image = Mat::zeros(imagerows, imagecols, CV_8UC3);
		char* pData = (char*)image.data;
		for (int i = 0; i < imagerows*imagecols; i++)
		{
			fread(&pData[i * 3], sizeof(char), 1, fpr);
			fread(&pData[i * 3 + 1], sizeof(char), 1, fpr);
			fread(&pData[i * 3 + 2], sizeof(char), 1, fpr);
		}
	}
	else if (channl == 1)
	{
		image = Mat::zeros(imagerows, imagecols, CV_8UC1);
		char* pData = (char*)image.data;
		for (int i = 0; i < imagerows*imagecols; i++)
		{
			fread(&pData[i], sizeof(char), 1, fpr);
		}
	}
	fclose(fpr);
	return true;
}
//===========��غ���======================================================================================================


//Ԥ��
void  MyPrediction(string filename, String pbPath, String txtpath)
{
	Mat img;
	StreamFileToImage(filename, img);  //ͨ���������ļ���ͼ

	dnn::Net net = cv::dnn::readNetFromTensorflow(pbPath); //��ȡTensorFlowģ��

	readTxt(txtpath);  //��ȡ�ַ���ǩ

	Mat Img_feature = SimpleGridFeature(img); //��ȡ����

	Mat inputBlob = dnn::blobFromImage(Img_feature, 1.0, Size(256, 1), Scalar(), true, false); //ת����ʽ

	net.setInput(inputBlob);  //��������

	cv::Mat pred = net.forward();//����ǰ�򴫲�

	int len = 653;
	int idstID[653];
	for (int i = 0; i < len; i++) //���³�ʼ���±�����
	{
		idstID[i] = i;
	}

	Mysort(pred, len, idstID); //����

	//===========���top5=========================
	for (int i = 0; i < 5; i++)
	{
		float ff = pred.at<float>(0, i);
		cout << "Ԥ����top" << i + 1 << ":\t" << vector_charName[idstID[i]] << "��\t ���ʣ�" << ff << endl;
		int a = 0;
	}
	cout << endl;
	//===========���top5=========================
}

//������
int main()
{
	//String pbPath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\1.pb";  //ģ��·��
	String pbPath = "D:\\20180913_squ_1.pb";  //ģ��·��
	
	String txtpath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\ImgHanZiName653.txt";  //�����±��ҵ���Ӧ�ַ�����
	
	Mat src = imread("D:\\1.jpg",0);         //��ͼ

	string filename = "test";                 //���������ļ���

	ImageToStreamFile(src, filename);         //ͼ��ת���ɶ�����

	MyPrediction(filename, pbPath, txtpath);  //����������ļ�·���� pbģ��·�����ַ�����·������Ԥ��

	system("pause");

	return 0;

}

//int main()
//{
//	String weights = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\OCRsoftmax.pb";
//	dnn::Net net = cv::dnn::readNetFromTensorflow(weights);
//	String txtpath = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\ImgHanZiName653.txt";
//	readTxt(txtpath);  //�ַ���ǩ
//	int len = 653;
//	int idstID[653];
//	for (int i = 0; i < len; i++) //��ʼ���±�����
//	{
//		idstID[i] = i;
//	}
//	=======����ͼ��=============================
//	 ���ö���ͼ�������ļ��е�·��
//	char* fileName = "D:\\sxl\\����ͼƬ\\���ַ���\\train653\\��\\";
//	std::vector <string>  imgNames;
//	 ��ȡ��Ӧ�ļ����������ļ���
//	readImgNamefromFile(fileName, imgNames);
//	 ������Ӧ�ļ����������ļ���
//	cout << imgNames.size() <<endl;
//	for (int i = 0; i < imgNames.size(); i++)
//	{
//		cv::Mat img = cv::imread(imgNames[i],0);
//		if (!img.data)
//		{
//			cout << "��ͼƬ���ͣ�";
//			continue;
//		}
//		
//		===========�����ͼ�����㷨code========================
//		start = clock();
//		Mat img = imread("D:\\1.jpg", 0);
//		Mat Img_feature = SimpleGridFeature(img);
//
//		Mat inputBlob = dnn::blobFromImage(Img_feature, 1.0, Size(256, 1), Scalar(), true, false);
//		net.setInput(inputBlob);
//		cv::Mat pred = net.forward();//����ǰ�򴫲�
//
//		for (int i = 0; i < len; i++) //��ʼ���±�����
//		{
//			idstID[i] = i;
//		}
//		Mysort(pred, len, idstID); //����
//
//		===========���top5=========================
//		for (int i = 0; i < 5; i++)
//		{
//			float ff=pred.at<float>(0, i);
//			cout <<"Ԥ����top"<<i+1<<":\t"<< vector_charName[idstID[i]]<< "��\t ���ʣ�" << ff << endl;
//			int a = 0;
//		}
//		cout << endl;
//		===========���top5=========================
//
//
//		===========ͳ����ʱ=========================
//		end12 = clock();
//		double f = (end12-start);
//		timesum += f;
//		===========ͳ����ʱ=========================
//		double minVal; double maxVal; Point minLoc; Point maxLoc;
//		minMaxLoc(pred, &minVal, &maxVal, &minLoc, &maxLoc);
//		//cout << minVal << " " << maxVal << " " << minLoc.x << " " << maxLoc.x << endl;
//		if (i % 1 == 0)
//		{
//			cout << i << ":" << vector_charName[maxLoc.x] << "\t";
//			cout <<endl;
//		}
//		===========�����ͼ�����㷨code========================
//	}
//	=======����ͼ��=============================
//
//	t2 = GetTickCount();
//	printf("\n����ʱ:%f ����\n", timesum);
//	printf("\nƽ����ʱ:%f ����\n", timesum / imgNames.size());
//	system("pause");
//	return 0;
//}


//ͼ�������ת��
//int main()
//{
//	Mat src = imread("D:\\21.jpg");
//	string filename = "test";
//	ImageToStreamFile(src, filename);
//	Mat dst;
//	StreamFileToImage(filename, dst);
//	imshow("dst", dst);
//	waitKey(0);
//
//}

////����
//int main()
//{
//	float a[] = { 5,7,3,6,4,1,2 };
//	int b[7] = { 0,1,2,3,4,5,6 };//��������ԭ�����Ӧ���±��Լ��������±�
//	Mysort(a, 7, b);
//	printf("����������Ϊ��\n");
//	for (int i = 0; i<7; i++)
//		printf("%f ", a[i]);
//	printf("\n");
//
//	printf("�����������Ӧ���±꣺\n");
//	for (int i = 0; i<7; i++)
//		printf("%d ", b[i]);
//	printf("\n");
//	system("pause");
//	return 0;
//}

//int main()
//{
//	String weights = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\expert-graph_t1.pb";
//	//String prototxt = "D:\\sxl\\VisualStudio\\vs2015programs\\OpenCVcallTensorFlow\\x64\\Debug\\OCR.pbtxt";
//	dnn::Net net = cv::dnn::readNetFromTensorflow(weights);
//	Mat img = imread("D:\\3.png", 0);
//	Mat inputBlob = dnn::blobFromImage(img, 1.0 / 255.0, Size(28, 28), Scalar(), false, false);
//	net.setInput(inputBlob);//set the network input, "data" is the name of the input layer     
//	cv::Mat pred = net.forward();//����ǰ�򴫲�
//	for (int row = 0; row < pred.rows; row++)
//	{
//		for (int col = 0; col < pred.cols; col++)
//		{
//			float fa = pred.at<float>(row, col);
//			int a = 0;
//		}
//	}
//	waitKey();
//	return 0;
//}
//
//bool ROI_AddImage()
//{
//
//	//��1������ͼ��
//	Mat srcImage1 = imread("D:\\661_2.jpg", 0);
//	Mat logoImage = imread("D:\\imageROI.jpg", 0);
//	////����ȫ��ͼƬ
//	//Mat imageTemp(logoImage.rows, logoImage.cols, CV_8UC1, Scalar(255));
//	//bitwise_and(logoImage, imageTemp, logoImage);
//	//imshow("22", logoImage);
//	//imwrite("D:\\661_1.jpg", srcImage1);
//	//imwrite("D:\\662_1.jpg", logoImage);
//	if (!srcImage1.data)
//	{
//		printf("fuck, read the picture is wrong!!! \n");
//		return false;
//	}
//
//	if (!logoImage.data)
//	{
//		printf("fuck, read the picture is wrong!!! \n");
//		return false;
//	}
//
//	//��2������һ��Mat���Ͳ������趨ROI����
//	Mat imageROI = srcImage1(Rect(0, 0, logoImage.cols, logoImage.rows));
//
//	//��3��������Ĥ
//	//Mat mask = imread("D:\\21_1.jpg", 0);
//	//������Ĥ
//	Mat mask;
//	logoImage.copyTo(mask);
//
//	//��4������Ĥ������ROI
//	logoImage.copyTo(imageROI, mask);
//	//��5����ʾ���
//	namedWindow("1 ����ROIʵ��ͼ�����ʾ������");
//	imshow("1 ����ROIʵ��ͼ�����ʾ������", srcImage1);
//	waitKey(0);
//	return true;
//}
//
//int main()
//{
//	bool b=ROI_AddImage();
//}

//==================��ȡpython��ȡ������====================
//   fstream infile;   //������
//int val=0;
//int icount = 0;
//char buffer[10];
//infile.open("E:\\sxl_Programs\\Python\\�洢npy\\��.txt", ios::in);
//if (!infile.is_open())
//	cout << "Open file failure" << endl;
//while (icount <= 255)            // ��δ���ļ�����һֱѭ��
//{
//	infile.getline(buffer, 10, '\n');//getline(char *,int,char) ��ʾ�����ַ��ﵽ256�����������оͽ���
//	cout << buffer << endl;
//	int ff= atof(buffer);
//	Img_feature.at<float>(0, icount)= atof(buffer);
//	icount++;
//}
//==================��ȡpython��ȡ������====================


