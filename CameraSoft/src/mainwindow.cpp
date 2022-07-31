#include "mainwindow.h"
#include <QDebug>
#include <QLabel>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onShowCurrentFrame(cv::Mat frame)
{
    if(frame.empty()) {
        qDebug() << "Recieved image is empty";
        return;
    }

    ui->image->showImage(frame);
}

void MainWindow::onShowCurrentCounterStatus(QString status)
{
    ui->CounterStatus->setText(status);
}

void MainWindow::onShowCurrentInferenceStatus(QString status)
{
    ui->InferenceStatus->setText(status);
}


