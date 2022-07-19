#include "detector.h"

Detector::Detector()
{
    m_hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

Detector::~Detector()
{

}


void resizeRect(cv::Rect& rect) {
    rect.x += cvRound(rect.width*0.1);
    rect.width = cvRound(rect.width*0.8);
    rect.y += cvRound(rect.height*0.06);
    rect.height = cvRound(rect.height*0.8);
}

void filterOverlappingRects(const std::vector<cv::Rect> &detections, std::vector<cv::Rect> &filteredDetections) {
    size_t i, j;
    for( i = 0; i < detections.size(); i++ )
    {
        cv::Rect r = detections[i];
        for( j = 0; j < detections.size(); j++ ) {
            //filter out overlapping rectangles
            if ( j!=i  && (r & detections[j]).area() >= r.area() * 0.8){
                break;
            }
        }

        if( j == detections.size() )
            filteredDetections.push_back(r);
    }
}

void Detector::detect(cv::Mat &currFrame)
{
    m_hog.detectMultiScale(currFrame, m_found, m_weights, 0.45, cv::Size(8, 8), cv::Size(128, 128), 1.059, 2);
    filterOverlappingRects(m_found, m_found_filtered);

    for( size_t i = 0; i < m_found_filtered.size(); i++ )
    {
        cv::Rect r = m_found_filtered[i];
        resizeRect(r);
        cv::rectangle(currFrame, r, cv::Scalar(0,0,255), 3);
        int verticalCenter = r.tl().y + r.height/2;
        int horizontalCenter = r.tl().x + r.width/2;
        cv::circle(currFrame, cv::Point(horizontalCenter, verticalCenter),10,cv::Scalar(255,0,255), -1);
    }
    clear();
}

void Detector::clear()
{
    m_found.clear();
    m_found_filtered.clear();
    m_weights.clear();
}
