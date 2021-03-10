#include <iostream>
#include <opencv2/opencv.hpp>

/*
cv::Mat substract(cv::Mat bg, cv::Mat ob)
{
	cv::Mat res = cv::Mat(bg.rows, bg.cols, cv::IMREAD_GRAYSCALE);
	int seuil = 30;
	for (int j = 0; j < bg.rows; j++)
	{
		for (int i = 0; i < bg.cols; i++)
		{
			cv::Vec3b bg_p = bg.at<cv::Vec3b>(j, i);
			cv::Vec3b ob_p = ob.at<cv::Vec3b>(j, i);
			int diff = (ob_p[0] - bg_p[0]) + (ob_p[1] - bg_p[1]) + (ob_p[2] - bg_p[2]);
			res.at<uchar>(j, i) = (diff >= seuil ? 255 : 0);
		}
	}
	return res;
}
*/

/*
int main()
{
	cv::Mat bg = cv::imread("background.jpg", cv::IMREAD_COLOR);
	cv::Mat ob = cv::imread("object.jpg", cv::IMREAD_COLOR);
	cv::Mat res = substract(bg, ob);
	cv::namedWindow("img", cv::WINDOW_NORMAL);
	imshow("img", res);
	cv::waitKey(0);
}
*/