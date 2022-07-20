#ifndef DETECTOR_H
#define DETECTOR_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <list>

class Detector final : public QObject
{
    Q_OBJECT
public:
    Detector();
    ~Detector();
    void detect(cv::Mat &currFrame);

signals:
    void cameIn();
    void wentOut();
private:
    class DetectedPerson;
    cv::HOGDescriptor m_hog;
    std::vector<cv::Rect> m_found;
    std::vector<double> m_weights;
    std::list<cv::Rect> m_found_filtered;
    std::list<DetectedPerson> m_detectedPeople;

    void clear();
};

#endif // DETECTOR_H
