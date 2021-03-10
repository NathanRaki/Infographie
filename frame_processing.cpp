//video processing frame by frame
#include <iostream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

void negate_image(cv::Mat& image)
{
    for (int j = 0; j < image.rows; j++)
    {
        for (int i = 0; i < image.cols; i++)
        {
            image.at<cv::Vec3b>(j, i)[0] = 255 - image.at<cv::Vec3b>(j, i)[0];
            image.at<cv::Vec3b>(j, i)[1] = 255 - image.at<cv::Vec3b>(j, i)[1];
            image.at<cv::Vec3b>(j, i)[2] = 255 - image.at<cv::Vec3b>(j, i)[2];
        }
    }
}

int threshold_(cv::Mat img)
{
    // Initialisation des min/max
    int max = -1;
    int min = 256;
    // On parcourt chaque pixel
    for (int j = 0; j < img.rows; j++)
    {
        for (int i = 0; i < img.cols; i++)
        {
            // Si l'intensité est supérieure au max, elle devient le nouveau max
            if (img.at<uchar>(j, i) > max) { max = img.at<uchar>(j, i); }
            // Et inversement pour le min
            else if (img.at<uchar>(j, i) < min) { min = img.at<uchar>(j, i); }
        }
    }
    return (max - min) / 2; // On retourne le calcul du seuil
}

void binary_image(cv::Mat& image)
{
    int seuil = threshold_(image);
    for (int j = 0; j < image.rows; j++)
    {
        for (int i = 0; i < image.cols; i++)
        {
            image.at<uchar>(j, i) = (image.at<uchar>(j, i) >= seuil ? 0 : 255);
        }
    }
}

void only_green(cv::Mat& image)
{
    cv::Mat bgr[3];
    cv::Mat g, res;
    std::vector<cv::Mat> channels;

    split(image, bgr);
    cv::Mat img_green = bgr[1];
    g = cv::Mat::zeros(image.rows, image.cols, CV_8UC1);
    channels.push_back(g);
    channels.push_back(img_green);
    channels.push_back(g);
    cv::merge(channels, res);
    image = res;
}

void sepia(cv::Mat& image)
{
    cv::Mat kern = (cv::Mat_<float>(3, 3) << 0.272, 0.534, 0.131,
                                             0.349, 0.686, 0.168,
                                             0.393, 0.769, 0.189);
    cv::transform(image, image, kern);
}

void edit_contrast(cv::Mat& image)
{
    for (int j = 0; j < image.rows; j++)
    {
        for (int i = 0; i < image.cols; i++)
        {
            if (image.at<cv::Vec3b>(j, i)[0] < 30) { image.at<cv::Vec3b>(j, i)[0] = 0; }
            else if (image.at<cv::Vec3b>(j, i)[0] > 225) { image.at<cv::Vec3b>(j, i)[0] = 255; }
            else { image.at<cv::Vec3b>(j, i)[0] = int(round((255.0 / 195.0) * (image.at<cv::Vec3b>(j, i)[0] - 30) + 0.5));  }

            if (image.at<cv::Vec3b>(j, i)[1] < 30) { image.at<cv::Vec3b>(j, i)[1] = 0; }
            else if (image.at<cv::Vec3b>(j, i)[1] > 225) { image.at<cv::Vec3b>(j, i)[1] = 255; }
            else { image.at<cv::Vec3b>(j, i)[1] = int(round((255.0 / 195.0) * (image.at<cv::Vec3b>(j, i)[1] - 30) + 0.5)); }

            if (image.at<cv::Vec3b>(j, i)[2] < 30) { image.at<cv::Vec3b>(j, i)[2] = 0; }
            else if (image.at<cv::Vec3b>(j, i)[2] > 225) { image.at<cv::Vec3b>(j, i)[2] = 255; }
            else { image.at<cv::Vec3b>(j, i)[2] = int(round((255.0 / 195.0) * (image.at<cv::Vec3b>(j, i)[2] - 30) + 0.5)); }
        }
    }
}

int main(int argc, char** argv)
{
    // Open a video file:
    cv::VideoCapture cap("video2.avi");
    if(!cap.isOpened()) {
        std::cout << "Unable to open the camera\n";
        std::exit(-1);
    }

    // Get the width and the FPS(Frames Per Second) of the video
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double FPS = cap.get(cv::CAP_PROP_FPS);

    cv::Mat image;
    
    cv::Mat p2, p3, p4, p5, p6;
    int c = 3;
    int l = 2;
    int delta_w = width / c; //divide image in n parts
    int delta_h = height / l;

    while(true) {
        cap >> image;
        if(image.empty()) {
            std::cout << "Can't read frames from your camera\n";
            break;
        }
       //pass from RGB to graylevel
        p2 = image(cv::Rect(delta_w,    0,          image.cols / c, image.rows / l));
        p3 = image(cv::Rect(delta_w*2,  0,          image.cols / c, image.rows / l));
        p4 = image(cv::Rect(0,          delta_h,    image.cols / c, image.rows / l));
        p5 = image(cv::Rect(delta_w,    delta_h,    image.cols / c, image.rows / l));
        p6 = image(cv::Rect(delta_w*2,  delta_h,    image.cols / c, image.rows / l));

        negate_image(p2);
        cv::cvtColor(p3, p3, cv::COLOR_BGR2GRAY);
        binary_image(p3);
        cv::cvtColor(p3, p3, cv::COLOR_GRAY2BGR);
        only_green(p4);
        sepia(p5);
        edit_contrast(p6);

        p2.copyTo(image(cv::Rect(delta_w,       0,          image.cols / c, image.rows / l)));
        p3.copyTo(image(cv::Rect(delta_w * 2,   0,          image.cols / c, image.rows / l)));
        p4.copyTo(image(cv::Rect(0,             delta_h,    image.cols / c, image.rows / l)));
        p5.copyTo(image(cv::Rect(delta_w,       delta_h,    image.cols / c, image.rows / l)));
        p6.copyTo(image(cv::Rect(delta_w * 2,   delta_h,    image.cols / c, image.rows / l)));

        cv::imshow("Modified video", image);

        // Stop the camera if the user presses the "ESC" key
        if(cv::waitKey(1000.0/FPS) == 27) break;
     }

    return 0;
}
