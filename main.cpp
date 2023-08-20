#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const string videoPath = "roadcars.avi";
const string backgroundPath = "background.jpg";

int main(void)
{
	// ��ȡ��Ƶ��
	VideoCapture video(videoPath);
	if (video.isOpened() == false)
	{
		cout << "error: cannot load video..." << endl;
		exit(-1);
	}

	// ��ȡÿ֡�ĳߴ�
	int width = video.get(CAP_PROP_FRAME_WIDTH);// 320
	int height = video.get(CAP_PROP_FRAME_HEIGHT);// 176

	// ��ȡ����ͼ
	Mat background = imread(backgroundPath, IMREAD_GRAYSCALE);
	if (background.empty())
	{
		cout << "error: cannot load background..." << endl;
		exit(-2);
	}

	// ��֡����
	Mat frame, grayFrame;
	Mat diffImg, blurDiffImg, binaryImg, cleanBinary;
	Mat structElement = getStructuringElement(MORPH_RECT, Size(5, 5));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Rect outline;
	bool flag1[5] = { false, false, false, false, false };
	bool flag2[5] = { false, false, false, false, false };
	int lim = 250;
	int count = 0;
	vector<Mat> channelImgs;
	vector<Mat> resImgs;
	Mat resImg, dilated;
	char info[10] = { '0' };
	for (;;)
	{
		video >> frame;
		if (frame.empty())
			break;

		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);

		absdiff(grayFrame, background, diffImg);

		diffImg *= 2;

		blur(diffImg, blurDiffImg, Size(7, 7));

		threshold(blurDiffImg, binaryImg, 30, 255, THRESH_BINARY);

		morphologyEx(binaryImg, cleanBinary, MORPH_OPEN, structElement, Point(-1, -1), 1);

		findContours(cleanBinary, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		for (int i = 0; i < contours.size(); i++)
		{
			outline = boundingRect(contours[i]);
			if (outline.area() > 1000 || outline.br().x > 260)
			{
				rectangle(frame, outline, Scalar(0, 0, 255), 1);

				if (outline.br().x > lim)// �ղ���
				flag1[i] = true;
				
				if (flag1[i] == true && outline.br().x > lim && outline.tl().x < lim)// ��������
				{
					flag2[i] = true;
				}
				
				if (flag1[i] == true && flag2[i] == true && outline.tl().x > lim)// ��ȥ��
				{
					flag1[i] = false;
					flag2[i] = false;
					count++;
					//cout << count << endl;
				}
			}
		}

		sprintf_s(info, "car:%d", count);
		putText(frame, info, Point(200, 30), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 255, 0), 1);

		imshow("Ч��", frame);

		if (waitKey(100) == 27)
			break;
	}

	video.release();
	destroyAllWindows();
	return 0;
}

