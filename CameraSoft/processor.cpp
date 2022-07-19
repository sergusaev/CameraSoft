#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QDebug>
#include <vector>
#include <sstream>
#include "processor.h"


Processor::Processor()
{
    QObject::connect(this, SIGNAL(incoming(int)), &m_counter, SLOT(onIncoming(int)));
    QObject::connect(this, SIGNAL(outgoing(int)), &m_counter, SLOT(onOutgoing(int)));
}

Processor::~Processor()
{

}

void Processor::exec()
{
    cv::Mat currFrame;
    cv::VideoCapture cap;
    cap.open(/*"../CameraSoft/test_video_sample_1.mp4"*/0);

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

        m_detector.detect(currFrame);
        //        cv::imshow("Output", currFrame);
        //        cv::waitKey(0);
        QImage qImgFrame = QImage((uchar*)currFrame.data, currFrame.cols, currFrame.rows, currFrame.step, QImage::Format_BGR888);
        emit showCurrentFrame(qImgFrame);
    }
}
