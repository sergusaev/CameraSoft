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

void Transmitter::exec()
{
    cv::Mat currFrame;
    cv::VideoCapture cap;
    cap.open(0);

    cv::HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    std::vector<cv::Point> track;

    if (!cap.isOpened()) {
        qDebug() << "ERROR! Unable to open default camera";
        return;
    }
    qDebug() << "Start transmitting";
    for (;;)
    {
        cap >> currFrame;
        if (currFrame.empty()) {
            qDebug() << "ERROR! blank frame grabbed";
            break;
        }

        cv::Mat img = currFrame.clone();
        cv::resize(img,img,cv::Size(img.cols*2, img.rows*2));
        std::vector<cv::Rect> found;
        std::vector<double> weights;
        hog.detectMultiScale(img, found, weights);


        for( size_t i = 0; i < found.size(); i++ )
                {
                    cv::Rect r = found[i];
                    rectangle(img, found[i], cv::Scalar(0,0,255), 3);
                    std::stringstream temp;
                    temp << weights[i];
                    cv::putText(img, temp.str(), cv::Point(found[i].x,found[i].y+50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255));
                    track.push_back(cv::Point(found[i].x+found[i].width/2,found[i].y+found[i].height/2));
                }
//                for(size_t i = 1; i < track.size(); i++){
//                    line(img, track[i-1], track[i], cv::Scalar(255,255,0), 2);
//                }
//                cv::imshow("Input", img);
//                cv::waitKey(0);

        QImage qImgFrame = QImage((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_BGR888);
        emit showCurrentFrame(qImgFrame.copy());
    }
}
