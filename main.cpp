#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;


cv::Mat substract(cv::Mat bg, cv::Mat ob, int s)
{
    cv::Mat res = cv::Mat(bg.rows, bg.cols, cv::IMREAD_GRAYSCALE);
    int seuil = s;
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

/*
int main(int argc, char** argv)
{
    

        // set input video
        std::string video = "backgroundSubstraction.mp4";
        // Open a video file:
        cv::VideoCapture capt(video);
        // Exit if unable to open a video
        if(!capt.isOpened()) {
            std::cout << "Unable to open video\n";
            std::exit(-1);
        }
        
        //set variable for frame
        cv::Mat frame;
        cv::Mat bg;
        cv::Mat sub;

        while(true)
        {
            //get a frame from video
            capt >> frame;
            
            if (capt.get(cv::CAP_PROP_POS_FRAMES) == 1) { bg = frame.clone(); }

            // stop the program if no more frames
            if(frame.empty())  break;
            sub = substract(bg, frame, 40);

            //show the frame
            cv::imshow("Video", sub);
            bg = frame.clone();

            // Stop the camera if the user presses the "ESC" key
            if(cv::waitKey(1) == 27) break;
         }
    
    return 0;
}
*/