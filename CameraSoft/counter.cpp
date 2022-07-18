#include "counter.h"
#include <QDebug>


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

void Counter::onIncoming(int count)
{
    setIn(in()+ count);
    setInside(inside() + count);
}

void Counter::onOutgoing(int count)
{
    if(inside() < count) {
        qDebug() << "Oops! Unexistant person outgoing...";
        return;
    }
    setIn (inside() - count);
    setInside(inside() - count);
}
