#include <QApplication>
#include <QThread>
#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "mainwindow.h"
#include "transmitter.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QThread *transmissionThread = QThread::create([&](){
        Transmitter transmitter;
        QObject::connect(&transmitter, SIGNAL(showCurrentFrame(QImage)), &w, SLOT(onShowCurrentFrame(QImage)));
        transmitter.exec();
    });
    QObject::connect(transmissionThread, SIGNAL(finished()), transmissionThread, SLOT(deleteLater()));
    transmissionThread->start();
    w.show();
    return a.exec();
}
