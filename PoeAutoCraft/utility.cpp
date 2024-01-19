#include <QCoreApplication>
#include <QTime>
#include "utility.h"
void delayMs(int ms)
{
    QTime t = QTime::currentTime().addMSecs(ms);
    while(t > QTime::currentTime())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }
}
