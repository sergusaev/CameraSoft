#include "cvimagewidget.h"

#include <QPainter>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

CVImageWidget::CVImageWidget(QWidget *parent)
    : QWidget(parent)
{

}

QSize CVImageWidget::sizeHint() const {
    return m_qimage.size();
}
QSize CVImageWidget::minimumSizeHint() const {
    return m_qimage.size();
}
void CVImageWidget::showImage(const cv::Mat& image) {

    if (image.empty())
    {
        m_tmp = image;
        m_qimage = QImage("../CameraSoft/pics/hourglass.png");
    }
    else
    {
        // Convert the image to the RGB888 format
        switch (image.type()) {
        case CV_8UC1:
            cv::cvtColor(image, m_tmp, cv::COLOR_GRAY2RGB);
            break;
        case CV_16SC1:
            cv::cvtColor(m_tmp.clone(), m_tmp, cv::COLOR_GRAY2RGB);
            break;
        case CV_8UC3:
            cv::cvtColor(image, m_tmp, cv::COLOR_BGR2RGB);
            break;
        }

        // QImage needs the data to be stored continuously in memory
        if (!m_tmp.isContinuous()) return;;
        // Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
        // (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
        // has three bytes.
        m_qimage = QImage(m_tmp.data, m_tmp.cols, m_tmp.rows, m_tmp.cols*3, QImage::Format_RGB888);
    }

    //this->setFixedSize(image.cols, image.rows);

    repaint();
}

void CVImageWidget::paintEvent(QPaintEvent* /*event*/) {
    // Display the image
    QPainter painter(this);

    QRect imageArea = rect();
    double scaleX = (double)imageArea.width() / m_qimage.width();
    double scaleY = (double)imageArea.height() / m_qimage.height();

    if (scaleX > scaleY) {
        imageArea.setWidth(m_qimage.width() * scaleY);
    } else  {
        imageArea.setHeight(m_qimage.height() * scaleX);
    }
    painter.drawImage(imageArea, m_qimage);
    painter.end();
}
