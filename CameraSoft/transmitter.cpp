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
        cv::Mat sobelImg;
        cv::Sobel(img, sobelImg, CV_8UC1, 1, 1, 3, 7, 1);

        //        cv::imshow("Sobel", sobelImg);
        //        cv::waitKey(0);
        cv::resize(img,img,cv::Size(img.cols * 1.2, img.rows * 1.2));
        std::vector<cv::Rect> found;
        std::vector<cv::Rect> found_filtered;
        std::vector<double> weights;

        hog.detectMultiScale(img, found, weights, 0.65, cv::Size(8, 8), cv::Size(32, 32), 1.059, 0.65);

        size_t i, j;
        for( i = 0; i < found.size(); i++ )
        {
            cv::Rect r = found[i];
            for( j = 0; j < found.size(); j++ ) {
                //filter out overlapping rectangles
                if ( j!=i ) {
                    cv::Rect iRect =  r;
                    cv::Rect jRect = found[j];
                    cv::Rect intersectRect = (iRect & jRect);
                    if (intersectRect.area()>=iRect.area()*0.9) break;
                }
            }
            if( j == found.size() )
                found_filtered.push_back(r);
        }

        for( size_t i = 0; i < found_filtered.size(); i++ )
        {
            cv::Rect r = found_filtered[i];
            // The HOG detector returns slightly larger rectangles than the real objects,
            // so we slightly shrink the rectangles to get a nicer output.
            r.x += cvRound(r.width*0.1);
            r.width = cvRound(r.width*0.8);
            r.y += cvRound(r.height*0.07);
            r.height = cvRound(r.height*0.8);
            cv::rectangle(img, found_filtered[i], cv::Scalar(0,0,255), 3);
            std::stringstream temp;
            temp << weights[i];
            cv::putText(img, temp.str(), cv::Point(found_filtered[i].x,found_filtered[i].y+50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255));
        }

        QImage qImgFrame = QImage((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_BGR888);
        emit showCurrentFrame(qImgFrame.copy());
    }
}
