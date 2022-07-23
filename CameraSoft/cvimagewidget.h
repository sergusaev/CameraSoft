#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

#include <QWidget>
#include <opencv2/core/mat.hpp>

class CVImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CVImageWidget(QWidget *parent = 0);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
public slots:

    void showImage(const cv::Mat& image);

protected:
    void paintEvent(QPaintEvent* /*event*/);

    QImage m_qimage;
    cv::Mat m_tmp;
};

#endif // CVIMAGEWIDGET_H
