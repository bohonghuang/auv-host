//
// Created by Qff on 2023/3/1.
//

#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <opencv2/opencv.hpp>

namespace auv::algorithm
{

class VideoCapture
{
public:
	VideoCapture(int index);
	cv::Mat get_frame();
private:
	cv::VideoCapture m_cap;
};

}

#endif //VIDEO_CAPTURE_H
