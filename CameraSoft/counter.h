#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>

class Counter final : public QObject
{
    Q_OBJECT
public:
    Counter();
    ~Counter();

    int in() const;
    void setIn(int newIn);

    int out() const;
    void setOut(int newOut);

    int inside() const;
    void setInside(int newInside);


public slots:
    void onIncoming();
    void onOutgoing();


private:
    int m_in;
    int m_out;
    int m_inside;
};

#endif // COUNTER_H
