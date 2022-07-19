#ifndef DETECTOR_H
#define DETECTOR_H

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>

class Detector final
{
public:
    Detector();
    ~Detector();
    void detect(cv::Mat &currFrame);
private:
    void clear();
    cv::HOGDescriptor m_hog;
    std::vector<cv::Rect> m_found;
    std::vector<cv::Rect> m_found_filtered;
    std::vector<double> m_weights;

};

#endif // DETECTOR_H
