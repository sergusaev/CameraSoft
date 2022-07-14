#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <vector>
#include <sstream>

#include "transmitter.h"

Transmitter::Transmitter()
{

}

Transmitter::~Transmitter()
{

}

void ResizeRect(cv::Rect& rect) {
    rect.x += cvRound(rect.width*0.1);
    rect.width = cvRound(rect.width*0.8);
    rect.y += cvRound(rect.height*0.06);
    rect.height = cvRound(rect.height*0.8);
}

void FilterOverlappingRects(const std::vector<cv::Rect> &detections, std::vector<cv::Rect> &filteredDetections) {
    size_t i, j;
        for( i = 0; i < detections.size(); i++ )
        {
            cv::Rect r = detections[i];
            for( j = 0; j < detections.size(); j++ ) {
                //filter out overlapping rectangles
                if ( j!=i  && ((r & detections[j]).area() >= r.area() * 0.7
                                || (r & detections[j]) == r)){
                    break;
                }
            }

            if( j == detections.size() )
                filteredDetections.push_back(r);
        }
}


void Transmitter::exec()
{
    cv::Mat currFrame;
    cv::VideoCapture cap;
    cap.open("C:/Projects/CameraSoft/CameraSoft/test_video_3.mp4");

    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

    if (!cap.isOpened()) {
        qDebug() << "ERROR! Unable to open default camera";
        return;
    }
    qDebug() << "Start transmitting";

    int frameWidth =  cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight =  cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    qDebug() << "Size:" << frameWidth << "x" << frameHeight;

    while (cap.read(currFrame))
    {
        if (currFrame.empty()) {
            qDebug() << "ERROR! blank frame grabbed";
            break;
        }
        std::vector<cv::Rect> found;
        std::vector<cv::Rect> found_filtered;
        std::vector<double> weights;

        hog.detectMultiScale(currFrame, found, weights, 0.3, cv::Size(8, 8), cv::Size(128, 128), 1.059, 2);
        FilterOverlappingRects(found, found_filtered);

        for( size_t i = 0; i < found_filtered.size(); i++ )
        {
            cv::Rect r = found_filtered[i];
            ResizeRect(r);
            cv::rectangle(currFrame, found_filtered[i], cv::Scalar(0,0,255), 3);
        }

        QImage qImgFrame = QImage((uchar*)currFrame.data, currFrame.cols, currFrame.rows, currFrame.step, QImage::Format_BGR888);
        emit showCurrentFrame(qImgFrame.copy());
    }
}
