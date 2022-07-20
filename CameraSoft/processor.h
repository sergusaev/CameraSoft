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

public slots:
    void onCameIn();
    void onWentOut();
signals:
    void showCurrentFrame(QImage frame);
    void incoming();
    void outgoing();

private:
   Counter m_counter;
   Detector m_detector;



};

#endif // PROCESSOR_H
