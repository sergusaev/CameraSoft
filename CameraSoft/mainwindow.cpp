#include "mainwindow.h"
#include <QDebug>
#include <QLabel>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
//    QLabel *label = new QLabel("CounterStatus");
//    ui->statusbar->addWidget(label);


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

void MainWindow::onShowCurrentStatus(QString status)
{
    ui->CounterStatus->setText(status);
    //    ui->verticalLayout->hori->setText(status);
}

void MainWindow::onShowCurrentInferenceStatus(QString status)
{
    ui->InferenceStatus->setText(status);
}


