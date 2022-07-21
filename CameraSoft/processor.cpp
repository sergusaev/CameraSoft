#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QDebug>
#include <vector>
#include <sstream>
#include <string>
#include "processor.h"


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

static cv::Point  bottomLeft (50, 40);         // a point for botom-left corner of counter text on a frame
static cv::Scalar textColor (255, 255, 0);      // a color for counter text
static int fontFace = 4;                        // is equal to FONT_HERSHEY_COMPLEX  (see cv::HersheyFonts)
static double fontScale = 0.8;                  // Font scale factor that is multiplied by the font-specific base size.
static int textThickness = 1;                   // thickness for text

void Processor::exec()
{
    cv::Mat currFrame;
    cv::VideoCapture cap;
    cap.open(/*"../CameraSoft/test_video_sample_4.mp4"*/0);

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
                    bottomLeft,
                    fontFace,
                    fontScale,
                    textColor,
                    textThickness
                    );
//                cv::imshow("Output", currFrame);
//                cv::waitKey(0);
        QImage qImgFrame = QImage((uchar*)currFrame.data, currFrame.cols, currFrame.rows, currFrame.step, QImage::Format_BGR888);
        emit showCurrentFrame(qImgFrame);
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
