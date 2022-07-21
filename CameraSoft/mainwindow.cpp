#include "mainwindow.h"
#include <QDebug>
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

void MainWindow::onShowCurrentFrame(QImage frame)
{
    if(frame.isNull()) {
        qDebug() << "Recieved image is empty";
        return;
    }
    setCurrFrame(frame);
    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0,0, currFrame().scaled(this->size()));
#ifdef TIMING_OUTPUT
    qDebug() << "Widget repainted with recieved image";
#endif
}

const QImage &MainWindow::currFrame() const
{
    return m_currFrame;
}

void MainWindow::setCurrFrame(const QImage &newCurrFrame)
{
    m_currFrame = newCurrFrame;
}

