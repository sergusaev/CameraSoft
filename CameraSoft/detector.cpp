#include "detector.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDateTime>

// Constants for detections
const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESHOLD = 0.45;
const float NMS_THRESHOLD = 0.45;
const float CONFIDENCE_THRESHOLD = 0.65;
const double SAME_PERSON_OVERLAP_THRESHOLD = 0.7;   //coefficient of union of two rects that might be one person


// Text parameters for detections
const double FONT_SCALE = 0.7;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

// Colors for detections
cv::Scalar BLACK = cv::Scalar(0,0,0);
cv::Scalar BLUE = cv::Scalar(255, 178, 50);
cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
cv::Scalar RED = cv::Scalar(0,0,255);



class Detector::DetectedPerson final
{
public:
    DetectedPerson();
    DetectedPerson(const cv::Rect &initialRect, float weight);
    ~DetectedPerson();
    cv::Rect m_initialRect;
    cv::Rect m_currRect;
    float m_weight;
    quint64 m_detectionTimePoint;

};

Detector::DetectedPerson::DetectedPerson()
{
    m_weight  = 0;
    m_detectionTimePoint = QDateTime::currentMSecsSinceEpoch();

}

Detector::DetectedPerson::DetectedPerson(const cv::Rect &initialRect, float weight)
{
    m_initialRect = std::move(initialRect);
    m_currRect = m_initialRect;
    m_detectionTimePoint = QDateTime::currentMSecsSinceEpoch();

    m_weight  = weight;
}



Detector::DetectedPerson::~DetectedPerson()
{

}

Detector::Detector()
{   
    m_classList = {"person", "bicycle", "car", "motorbike", "aeroplane","bus", "train", "truck",
                   "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench",
                   "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
                   "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard",
                   "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
                   "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
                   "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza",
                   "donut", "cake", "chair", "sofa", "pottedplant", "bed", "diningtable", "toilet",
                   "tvmonitor", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
                   "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors",
                   "teddy bear", "hair drier", "toothbrush" };

    m_net = cv::dnn::readNet("../CameraSoft/YOLOv5s.onnx");
    std::vector<std::pair<cv::dnn::Backend, cv::dnn::Target>> backends = cv::dnn::getAvailableBackends();
    for(const auto& backend : backends)
    {
        if (backend.second == cv::dnn::DNN_TARGET_CUDA)
        {
            m_net.setPreferableBackend(backend.first);
            m_net.setPreferableTarget(backend.second);
        }
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
    return (first & second).area() >= qMin(first.area(), second.area()) * SAME_PERSON_OVERLAP_THRESHOLD
            || qAbs((first.tl().x + first.width/2) - (second.tl().x + second.width/2)) <= qMin(first.width, second.width) / 2;
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
                      float weight)
{
    int left = box.x;
    int top = box.y;
    int width = box.width;
    int height = box.height;

    // Draw bounding box
    rectangle(inputFrame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, THICKNESS * 3);

    // Get the label for the class name and its confidence.
    std::string label = cv::format("%.2f", weight);
    label = className[0] + ":" + label;

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
    net.forward(outputs);
    return outputs;
}

//the function to detection rectangles. The purpose is to detect people so only rects of class "person" will be stored
void postProcess(cv::Mat &inputFrame,
                 std::vector<cv::Mat> &outputs,
                 const std::vector<std::string> &className,
                 std::vector<cv::Rect> &boxes,
                 std::vector<float> &confidences)
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
            // Continue if the class score is above the threshold and detected object is person (classID.x == 0, first index in m_classList).
            if (maxClassScore > SCORE_THRESHOLD && classID.x == 0)
            {
                // Store class ID and confidence in the pre-defined respective vectors.
                confidences.push_back(confidence);
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



QString Detector::detect(cv::Mat &currFrame)
{
    // Process the image
    std::vector<cv::Mat> detections;
    detections = preProcess(currFrame, m_net);
    postProcess(currFrame, detections, m_classList, m_detections, m_weights);
    filterOverlappingRects(m_detections, m_detectionsFiltered, m_weights, m_filteredRectsIndicies);
    for(auto it = m_detectedPeople.begin(); it !=  m_detectedPeople.end(); ) {
        bool personFound = false;
        //a variable to represent index of current element of m_detectionsFiltered in m_filteredRectIndicies (to get current element weight)
        int idx = 0;
        for(auto iter = m_detectionsFiltered.begin(); iter != m_detectionsFiltered.end();) {
            if(isSamePerson(*iter, it->m_currRect)) {
                personFound = true;
                it->m_currRect = *iter;
                it->m_weight = m_weights[m_filteredRectsIndicies[idx]];
                it->m_detectionTimePoint = QDateTime::currentMSecsSinceEpoch();
                iter = m_detectionsFiltered.erase(iter);
                m_filteredRectsIndicies[idx] = -1;
                m_filteredRectsIndicies.erase(std::remove(m_filteredRectsIndicies.begin(), m_filteredRectsIndicies.end(), - 1));
                break;
            } else {
                iter = std::next(iter);
                idx++;
            }
        }
        if(!personFound) {
            if((QDateTime::currentMSecsSinceEpoch() - it->m_detectionTimePoint) > 200) {
                if(locatedOnTheLeft(it->m_initialRect, it->m_currRect)) {
                    if (it->m_currRect.br().x > 635) { //person bounding rect's right border is on right border of frame
                        emit cameIn();
                    }
                } else {
                    if (it->m_currRect.tl().x  < 5) { //person bounding rect's left border is on left border of frame
                        emit wentOut();
                    }
                }
                it = m_detectedPeople.erase(it);
            }
        } else {
            drawBoundingRect(currFrame, m_classList, it->m_currRect, it->m_weight);
            it = std::next(it);
        }

    }
    int idx = 0;
    for(auto it = m_detectionsFiltered.begin(); it != m_detectionsFiltered.end();it = std::next(it)) {
        m_detectedPeople.push_back(DetectedPerson(*it, m_weights[idx]));
        drawBoundingRect(currFrame, m_classList, *it, m_weights[idx]);
        idx++;
    }

    // Put efficiency information.
    // The function getPerfProfile returns the overall time inference(t)
    // and the timings for each of the layers(in layersTimes).
    std::vector<double> layersTimes;
    double freq = cv::getTickFrequency() / 1000;
    double t = m_net.getPerfProfile(layersTimes) / freq;
    std::string label = cv::format("Inference time : %.2f ms", t);
    //    putText(currFrame, label, cv::Point(20, 40), FONT_FACE, FONT_SCALE, RED);
    clear();
    return QString::fromStdString(label);
}


void Detector::clear()
{
    m_detections.clear();
    m_detectionsFiltered.clear();
    m_weights.clear();
    m_filteredRectsIndicies.clear();
}


