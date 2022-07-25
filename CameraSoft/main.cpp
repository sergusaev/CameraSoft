#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "mainwindow.h"
#include "processor.h"

Q_DECLARE_METATYPE(cv::Mat)

int main(int argc, char *argv[])
{

    //tweaking the debug output
    qSetMessagePattern("%{time h:mm:ss.zzz} %{if-category} %{category}: %{endif}%{if-debug} %{function} %{endif} %{message}");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    QApplication a(argc, argv);
    MainWindow w;
    QThread *transmissionThread = QThread::create([&](){
        Processor processor;
        QObject::connect(&processor, SIGNAL(showCurrentFrame(cv::Mat)), &w, SLOT(onShowCurrentFrame(cv::Mat)));
        QObject::connect(&processor, SIGNAL(showCurrentStatus(QString)), &w, SLOT(onShowCurrentStatus(QString)));
        QObject::connect(&processor, SIGNAL(showCurrentInferenceStatus(QString)), &w, SLOT(onShowCurrentInferenceStatus(QString)));
        processor.exec();
    });
    QObject::connect(transmissionThread, SIGNAL(finished()), transmissionThread, SLOT(deleteLater()));
    transmissionThread->start();
    w.show();
    return a.exec();
}
