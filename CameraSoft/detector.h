#ifndef DETECTOR_H
#define DETECTOR_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <list>

class Detector final : public QObject
{
    Q_OBJECT
public:
    Detector();
    ~Detector();
    QString detect(cv::Mat &currFrame);

signals:
    void cameIn();
    void wentOut();
private:

    std::vector<std::string> m_classList;               //vector of names of object classes for YOLO
    class DetectedPerson;                               //structure containing initial rect of detected person, current rect & detection failure count for each person
    cv::dnn::Net m_net;                                 // dnn (YOLO), performing object detection
    std::vector<cv::Rect> m_detections;                 //all detected rects
    std::vector<float> m_weights;                       //weights of all detected rects
    std::list<cv::Rect> m_detectionsFiltered;           //list of detected rects filtered via NMS
    std::vector<int> m_filteredRectsIndicies;           //indexes of filtered rects in vector of all rects
    std::list<DetectedPerson> m_detectedPeople;

    void clear();
};

#endif // DETECTOR_H
