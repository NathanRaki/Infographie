#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <bitset>

/* Decode a hidden message in an image */
void decode(cv::Mat tmp, int size)
{
	// Transform message in binary
	std::string message = "";
	int cpt = 0;
	for (int j = 0; j < tmp.rows; j++)
	{
		if (cpt >= size) { break; }
		for (int i = 0; i < tmp.cols; i++)
		{
			if (cpt >= size) { break; }
			cpt++;
			if (tmp.at<uchar>(j, i) % 2 == 0) { message += "0"; }
			else { message += "1"; }
		}
	}

	//Change bytes to ascii then translate it
	for (int pos = 0; pos < size; pos += 8)
	{
		std::string byte = message.substr(pos, 8);
		int dec = std::bitset<8>(byte).to_ulong();
		std::cout << char(dec);
	}
}

// Change gray intensity based on bit value
int convertIntensity(char bit, int intensity)
{
	if (((intensity % 2 == 0) && (bit == '0')) || ((intensity % 2 == 1) && (bit == '1'))) {  return intensity; }
	else if (intensity == 255) {  return intensity-1; }
	else { return intensity+1; }
}

// Hide a message inside an image
void encode()
{
	cv::Mat img = cv::imread("lena.jpg", cv::IMREAD_GRAYSCALE);
	std::string hidden = "peakaboo";
	std::string hiddenBytes = "";
	// Convert the message in binary
	for (int i = 0; i < hidden.length(); ++i)
	{
		int ascii = (uchar)hidden[i];
		std::string byte = std::bitset<8>(ascii).to_string();
		hiddenBytes += byte;
	}
	
	// Change color intensity based on bit values
	int n = 0;
	for (int j = 0; j < img.rows; j++) {
		if (n >= hiddenBytes.length()) { break; }
		for (int i = 0; i < img.cols; i++) {
			if (n >= hiddenBytes.length()) { break; }
			char bit = hiddenBytes[n];
			//std::cout << (int)img.at<uchar>(j, i) << " ";
			img.at<uchar>(j, i) = convertIntensity(bit, (int)img.at<uchar>(j, i));
			//std::cout << (int)img.at<uchar>(j, i) << std::endl;
			n++;
		}
	}
	decode(img, hidden.length()*8);
}

int main()
{
	/* Exercice 1 */
	cv::Mat stego = cv::imread("stego.png", cv::IMREAD_GRAYSCALE);
	decode(stego, 1176);
	std::cout << std::endl;
	/* Exercice 2 */
	encode();

	return 0;
}