#include "detector.h"
#include <QDebug>
#include <fstream>

// Constants for detections
const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 480.0;
const float SCORE_THRESHOLD = 0.5;
const float NMS_THRESHOLD = 0.45;
const float CONFIDENCE_THRESHOLD = 0.45;
const double SAME_PERSON_OVERLAP_THRESHOLD = 0.8;   /*coefficient to detect whether the person is same by comparing areas of bounding
                                                    rects on different iterations*/

// Text parameters for detections
const double FONT_SCALE = 0.7;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

// Colors for detections
cv::Scalar BLACK = cvScalar(0,0,0);
cv::Scalar BLUE = cvScalar(255, 178, 50);
cv::Scalar YELLOW = cvScalar(0, 255, 255);
cv::Scalar RED = cvScalar(0,0,255);



class Detector::DetectedPerson final
{
public:
    DetectedPerson();
    DetectedPerson(const cv::Rect &initialRect);
    ~DetectedPerson();
    cv::Rect m_initialRect;
    cv::Rect m_currRect;
    int m_failedDetectionsCount;
};

Detector::DetectedPerson::DetectedPerson()
{
    m_failedDetectionsCount = 0;
}

Detector::DetectedPerson::DetectedPerson(const cv::Rect &initialRect)
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
    m_net = cv::dnn::readNet(/*"../CameraSoft/YOLOv5s.onnx"*/"C:/Projects/CameraSoft/CameraSoft/YOLOv5s.onnx");
    std::ifstream ifs("C:/Projects/CameraSoft/CameraSoft/coco.names");
    std::string line;
    while (getline(ifs, line))
    {
        m_classList.push_back(line);
    }
}

Detector::~Detector()
{

}

//the function to detect whether the person is same on different iterations
bool isSamePerson(const cv::Rect &first, const cv::Rect &second)
{
#ifdef DEBUG_OUTPUT
    qDebug() << "Area of union of detected bounding rects:" << (first & second).area();
    qDebug() << "Criteria of person identity: not less than" << qMin(first.area(), second.area()) * SAME_PERSON_OVERLAP_THRESHOLD;
#endif
    return (first & second).area() >= qMin(first.area(), second.area()) * SAME_PERSON_OVERLAP_THRESHOLD;
}



//the function to filter out multiple bounding rects over the same detected person using NMS
void filterOverlappingRects(const std::vector<cv::Rect> &detections, std::list<cv::Rect> &filteredDetections, std::vector<float> &confidences, std::vector<int> &indices)
{
    // Perform Non-Maximum Suppression.
    cv::dnn::NMSBoxes(detections, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
    for (int i = 0; i < indices.size(); i++)
    {
        int idx = indices[i];
        filteredDetections.push_back(detections[idx]);
    }
}

//the function to detect whether the person went out or came in by comparing initial and current(last) bounding rect placement
bool locatedOnTheLeft(const cv::Rect &lhs, const cv::Rect &rhs)
{
    //comparing horizontal centers of rects
    return lhs.tl().x + lhs.width / 2 < rhs.tl().x + rhs.width / 2;
}

//The function to annotate the class names anchored to the top left corner of the bounding box.
void drawLabel(cv::Mat& inputFrame, std::string label, int left, int top)
{
    // Display the label at the top of the bounding box.
    int baseLine;
    cv::Size labelSize = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
    top = qMax(top, labelSize.height);
    // Top left corner.
    cv::Point tlc = cv::Point(left, top);
    // Bottom right corner.
    cv::Point brc = cv::Point(left + labelSize.width, top + labelSize.height + baseLine);
    // Draw black rectangle.
    rectangle(inputFrame, tlc, brc, BLACK, cv::FILLED);
    // Put the label on the black rectangle.
    putText(inputFrame, label, cv::Point(left, top + labelSize.height), FONT_FACE, FONT_SCALE, YELLOW, THICKNESS);
}

//the function to draw bounding rect with short info of detected obj
void drawBoundingRect(cv::Mat& inputFrame,
                      const std::vector<std::string> &className,
                      cv::Rect &box,
                      int idx,
                      std::vector<float> &confidences,
                      std::vector<int> &classIDs)
{
    int left = box.x;
    int top = box.y;
    int width = box.width;
    int height = box.height;

    // Draw bounding box
    rectangle(inputFrame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, THICKNESS * 3);

    // Get the label for the class name and its confidence.
    std::string label = cv::format("%.2f", confidences[idx]);
    label = className[classIDs[idx]] + ":" + label;

    // Draw class labels
    drawLabel(inputFrame, label, left, top);
}


//The function takes the image and the network as arguments. At first, the image is converted to a blob. Then it is set as input to the network.
std::vector<cv::Mat> preProcess(cv::Mat &inputFrame, cv::dnn::Net &net)
{
    // Convert frame to blob.
    cv::Mat blob;
    cv::dnn::blobFromImage(inputFrame, blob, 1./255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    net.setInput(blob);

    // Forward propagate.
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    return outputs;
}

//the function to detection rectangles
void postProcess(cv::Mat &inputFrame,
                 std::vector<cv::Mat> &outputs,
                 const std::vector<std::string> &className,
                 std::vector<cv::Rect> &boxes,
                 std::vector<float> &confidences,
                 std::vector<int> &classIDs)
{

    // Resizing factor
    float xFactor = inputFrame.cols / INPUT_WIDTH;
    float yFactor = inputFrame.rows / INPUT_HEIGHT;
    float *data = (float *)outputs[0].data;
    const int dimensions = 85;

    // 25200 for default size 640.
    const int rows = 25200;

    // Iterate through 25200 detections.
    for (int i = 0; i < rows; ++i)
    {
        float confidence = data[4];
        // Discard bad detections and continue.

        if (confidence >= CONFIDENCE_THRESHOLD)
        {
            float * classesScores = data + 5;
            // Create a 1x85 Mat and store class scores of 80 classes.
            cv::Mat scores(1, className.size(), CV_32FC1, classesScores);
            // Perform minMaxLoc and acquire the index of best class  score.
            cv::Point classID;
            double maxClassScore;
            cv::minMaxLoc(scores, 0, &maxClassScore, 0, &classID);
            // Continue if the class score is above the threshold.
            if (maxClassScore > SCORE_THRESHOLD)
            {
                // Store class ID and confidence in the pre-defined respective vectors.
                confidences.push_back(confidence);
                classIDs.push_back(classID.x);
                // Center.
                float cx = data[0];
                float cy = data[1];
                // Box dimension.
                float w = data[2];
                float h = data[3];
                // Bounding box coordinates.
                int left = int((cx - 0.5 * w) * xFactor);
                int top = int((cy - 0.5 * h) * yFactor);
                int width = int(w * xFactor);
                int height = int(h * yFactor);
                // Store good detections in the boxes vector.
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        // Jump to the next row.
        data += dimensions;
    }

}



void Detector::detect(cv::Mat &currFrame)
{
    // Process the image
    std::vector<cv::Mat> detections;
    detections = preProcess(currFrame, m_net);
    postProcess(currFrame, detections, m_classList, m_detections, m_weights, m_classIDs);
    filterOverlappingRects(m_detections, m_detectionsFiltered, m_weights, m_filteredRectsIndicies);
    int idx = 0;        //a variable for idx of checked rect for further drawing of label
    for(auto it = m_detectedPeople.begin(); it !=  m_detectedPeople.end(); ) {
        bool personFound = false;
        for(auto iter = m_detectionsFiltered.begin(); iter != m_detectionsFiltered.end();) {

            if(isSamePerson(*iter, it->m_currRect)) {
                personFound = true;
                it->m_failedDetectionsCount = 0;
                it->m_currRect = *iter;
                iter = m_detectionsFiltered.erase(iter);
                idx++;  //increase idx in any case to provide valid object class name
                break;
            } else {
                iter = std::next(iter);
                idx++;
            }
        }
        if(!personFound) {
            it->m_failedDetectionsCount++;
            if(it->m_failedDetectionsCount > 5) {
                if(locatedOnTheLeft(it->m_initialRect, it->m_currRect)) {
                    emit cameIn();
                } else {
                    emit wentOut();
                }
                it = m_detectedPeople.erase(it);
            }
        } else {
            drawBoundingRect(currFrame, m_classList, it->m_currRect, idx, m_weights, m_classIDs);
            it = std::next(it);
        }

    }

    for(auto it = m_detectionsFiltered.begin(); it != m_detectionsFiltered.end();it = std::next(it)) {
        drawBoundingRect(currFrame, m_classList, *it, idx, m_weights, m_classIDs);
        m_detectedPeople.push_back(DetectedPerson(*it));

    }

    // Put efficiency information.
    // The function getPerfProfile returns the overall time inference(t)
    // and the timings for each of the layers(in layersTimes).
    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000;
    double t = m_net.getPerfProfile(layersTimes) / freq;
    std::string label = cv::format("Inference time : %.2f ms", t);
    putText(currFrame, label, cv::Point(20, 40), FONT_FACE, FONT_SCALE, RED);
    clear();
}


void Detector::clear()
{
    m_detections.clear();
    m_detectionsFiltered.clear();
    m_weights.clear();
    m_filteredRectsIndicies.clear();
    m_classIDs.clear();
}


