#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QObject>
#include <QImage>
#include <memory>

class Transmitter final : public QObject
{
    Q_OBJECT

public:
    Transmitter();
    ~Transmitter();

    void exec();
signals:
    void showCurrentFrame(QImage frame);





};

#endif // TRANSMITTER_H
