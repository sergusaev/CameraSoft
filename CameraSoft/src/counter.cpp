#include "counter.h"
#include <QDebug>


Counter::Counter()
{
    m_in = 0;
    m_out = 0;
    m_inside = 0;
}

Counter::~Counter()
{

}

int Counter::in() const
{
    return m_in;
}

void Counter::setIn(int newIn)
{
    m_in = newIn;
}

int Counter::out() const
{
    return m_out;
}

void Counter::setOut(int newOut)
{
    m_out = newOut;
}

int Counter::inside() const
{
    return m_inside;
}

void Counter::setInside(int newInside)
{
    m_inside = newInside;
}

void Counter::onIncoming()
{
    setIn(in() + 1);
    setInside(inside() + 1);
    qDebug() << "Incoming person!";

}

void Counter::onOutgoing()
{
    if(inside() < 1) {
        qDebug() << "Oops! Unexistant person outgoing...";
        return;
    }
    setOut (out() + 1);

    setInside(inside() - 1);
    qDebug() << "Outgoing person!";
}
