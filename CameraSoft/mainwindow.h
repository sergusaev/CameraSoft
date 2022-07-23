#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <opencv2/core/mat.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();
    const QImage &currFrame() const;
    void setCurrFrame(const QImage &newCurrFrame);



public slots:
//     void onShowCurrentFrame(QImage frame);
       void onShowCurrentFrame(cv::Mat frame);
//protected:
//    void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;
    QImage m_currFrame;


};
#endif // MAINWINDOW_H
