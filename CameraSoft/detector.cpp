#include "detector.h"
#include <QDebug>

static double xOffset = 0.1;                        //coefficient to set x-coordinate  of top left point of rect offset
static double yOffset = 0.06;                       //coefficient to set y-coordinate  of top left point of rect offset
static double widthScale = 0.8;                     //coefficient to set rect width
static double heightScale = 0.8;                    //coefficient to set rect height
static double hitThreshold  = 0.3;                 // threshold of ratio of the detected rectangles
static double scaleFactor = 1.059;                  //coefficient of the detection window increase
static int thickness = 3;                           // thickness of bounding line
static double finalThreshold = 2;                   /*group_threshold – Coefficient to regulate the similarity threshold.
                                                    When detected, some objects can be covered by many rectangles.
                                                    0 means not to perform grouping. See groupRectangles()*/
static double filtrationOverlapThreshold = 0.8;     //coefficient to detect threshold for overlapping rectangles on one frame
static double samePersonOverlapThreshold = 0.8;     /*coefficient to detect whether the person is same by comparing areas of bounding
                                                    rects on different iterations*/

class Detector::DetectedPerson final
{
public:
    DetectedPerson();
    DetectedPerson(const cv::Mat &frame, const cv::Rect &initialRect);
    ~DetectedPerson();
    cv::Rect m_initialRect;
    cv::Rect m_currRect;
    int m_failedDetectionsCount;
};

Detector::DetectedPerson::DetectedPerson()
{
    m_failedDetectionsCount = 0;
}

Detector::DetectedPerson::DetectedPerson(const cv::Mat &frame, const cv::Rect &initialRect)
{
    m_initialRect = initialRect;
    m_currRect = initialRect;
    m_failedDetectionsCount = 0;
}



Detector::DetectedPerson::~DetectedPerson()
{

}

Detector::Detector()
{
    m_hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
}

Detector::~Detector()
{

}

//a function to detect whether the person is same on different iterations
bool isSamePerson(const cv::Rect &first, const cv::Rect &second)
{
    double firstArea = first.area();
    double secondArea = second.area();
    double unionArea = (first & second).area();
    qDebug() << "First area: " << firstArea;
    qDebug() << "Second area: " << secondArea;
    qDebug() << "Union area: " << unionArea;
    qDebug() << "Criteria of difference: " << firstArea * samePersonOverlapThreshold;
    return (first & second).area() >= qMin(first.area(), second.area()) * samePersonOverlapThreshold;
}

//a function to make bounding rects of detected persons more accurate
void resizeRect(cv::Rect& rect)
{
    rect.x += cvRound(rect.width * xOffset);
    rect.width = cvRound(rect.width * widthScale);
    rect.y += cvRound(rect.height * yOffset);
    rect.height = cvRound(rect.height * heightScale);
}

//a function to filter out multiple bounding rects over the same detected person
void filterOverlappingRects(const std::vector<cv::Rect> &detections, std::list<cv::Rect> &filteredDetections)
{
    size_t i, j;
    for( i = 0; i < detections.size(); i++ )
    {
        cv::Rect r = detections[i];
        for( j = 0; j < detections.size(); j++ ) {
            //filter out overlapping rectangles
            if ( j!=i  && (r & detections[j]).area() >= r.area() * filtrationOverlapThreshold){
                break;
            }
        }

        if(j == detections.size()) {
            resizeRect(r);
            filteredDetections.push_back(r);
        }
    }
}

//a function to detect whether the person went out or came in by comparing initial and current(last) bounding rect placement
bool locatedOnTheLeft(const cv::Rect &lhs, const cv::Rect &rhs)
{
    //comparing horizontal centers of rects
    return lhs.tl().x + lhs.width / 2 < rhs.tl().x + rhs.width / 2;
}


void Detector::detect(cv::Mat &currFrame)
{
    m_hog.detectMultiScale(currFrame,
                           m_found,
                           m_weights,
                           hitThreshold,
                           cv::Size(8, 8),
                           cv::Size(128, 128),
                           scaleFactor,
                           finalThreshold);
    filterOverlappingRects(m_found, m_found_filtered);

    for(auto it = m_detectedPeople.begin(); it !=  m_detectedPeople.end(); ) {
        bool personFound = false;
        for(auto iter = m_found_filtered.begin(); iter != m_found_filtered.end();) {
            if(isSamePerson(*iter, it->m_currRect)) {
                personFound = true;
                it->m_failedDetectionsCount = 0;
                it->m_currRect = *iter;
                iter = m_found_filtered.erase(iter);
                break;
            } else {
                iter = std::next(iter);
            }
        }
        if(!personFound) {
            it->m_failedDetectionsCount++;
            if(it->m_failedDetectionsCount > 15) {
                if(locatedOnTheLeft(it->m_initialRect, it->m_currRect)) {
                    emit cameIn();
                } else {
                    emit wentOut();
                }
                it = m_detectedPeople.erase(it);
            }
        } else {
            cv::rectangle(currFrame, it->m_currRect, cv::Scalar(0,0,255), thickness);
            it = std::next(it);
        }

    }


    for(auto it = m_found_filtered.begin(); it != m_found_filtered.end();it = std::next(it)) {
        cv::rectangle(currFrame, *it, cv::Scalar(0,0,255), thickness);
        //        cv::imshow("Current frame", currFrame);
        m_detectedPeople.push_back(DetectedPerson(currFrame, *it));

    }
    clear();
}


void Detector::clear()
{
    m_found.clear();
    m_found_filtered.clear();
    m_weights.clear();
}


