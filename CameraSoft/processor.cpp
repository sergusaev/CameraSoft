#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QDebug>
#include <vector>
#include <sstream>
#include <string>
#include "processor.h"

//Constants for text
//const cv::Point  BOTTOM_LEFT (50, 440);
//const cv::Scalar TEXT_COLOUR (255, 255, 0);

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
    cap.open("../CameraSoft/test_video_sample_4.mp4");

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

        QString inferenceStatus = m_detector.detect(currFrame);
        emit showCurrentFrame(currFrame);
        emit showCurrentInferenceStatus(inferenceStatus);
        emit showCurrentCounterStatus("In: " + QString("%1").arg(m_counter.in())+"  Out: " + QString("%1").arg(m_counter.out())+"  Inside:" + QString("%1").arg(m_counter.inside()));
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
