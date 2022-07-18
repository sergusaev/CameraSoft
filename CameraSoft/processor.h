#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QImage>
#include <memory>
#include "counter.h"
#include "detector.h"

class Processor final : public QObject
{
    Q_OBJECT

public:
    Processor();
    ~Processor();
    void exec();


signals:
    void showCurrentFrame(QImage frame);
    void incoming(int count);
    void outgoing(int count);

private:
   Counter m_counter;
   Detector m_detector;



};

#endif // PROCESSOR_H
