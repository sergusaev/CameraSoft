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

public slots:
    void onShowCurrentFrame(cv::Mat frame);
    void onShowCurrentStatus(QString status);
    void onShowCurrentInferenceStatus(QString status);
private:
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
