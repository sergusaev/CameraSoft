#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QDebug>
#include <vector>
#include <sstream>
#include <string>
#include "processor.h"

//Constants for text
const cv::Point  BOTTOM_LEFT (50, 440);
const cv::Scalar TEXT_COLOUR (255, 255, 0);
const int FONT_FACE = cv::FONT_HERSHEY_COMPLEX;
const double FONT_SCALE = 0.8;
const int THICKNESS = 1;

Processor::Processor()
    :m_counter()
    ,m_detector()
{
    QObject::connect(this, SIGNAL(incoming()), &m_counter, SLOT(onIncoming()));
    QObject::connect(this, SIGNAL(outgoing()), &m_counter, SLOT(onOutgoing()));
    QObject::connect(&m_detector, SIGNAL(cameIn()), this, SLOT(onCameIn()));
    QObject::connect(&m_detector, SIGNAL(wentOut()), this, SLOT(onWentOut()));
}

Processor::~Processor()
{

}



void Processor::exec()
{
    cv::Mat currFrame;
    cv::VideoCapture cap;
    cap.open(/*"../CameraSoft/video_2022-07-23_17-06-42.mp4"*/0);

    if (!cap.isOpened()) {
        qDebug() << "ERROR! Unable to open default camera";
        return;
    }

    qDebug() << "Start transmitting";

#ifdef DEBUG_OUTPUT
    int frameWidth =  cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frameHeight =  cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    qDebug() << "Size:" << frameWidth << "x" << frameHeight;
#endif


    while (cap.read(currFrame))
    {
        if (currFrame.empty()) {
            qDebug() << "ERROR! blank frame grabbed";
            break;
        }

        m_detector.detect(currFrame);
        cv::putText(currFrame,
                    "In: "+std::to_string(m_counter.in())+"  Out: "+std::to_string(m_counter.out())+"  Inside:"+std::to_string(m_counter.inside()),
                    BOTTOM_LEFT,
                    FONT_FACE,
                    FONT_SCALE,
                    TEXT_COLOUR,
                    THICKNESS
                    );
//                cv::imshow("Output", currFrame);
//                cv::waitKey(0);
//        QImage qImgFrame = QImage((uchar*)currFrame.data, currFrame.cols, currFrame.rows, currFrame.step, QImage::Format_BGR888);
        emit showCurrentFrame(currFrame);
    }
}

void Processor::onCameIn()
{
    emit incoming();
}

void Processor::onWentOut()
{
    emit outgoing();
}
