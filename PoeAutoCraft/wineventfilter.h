#ifndef WINEVENTFILTER_H
#define WINEVENTFILTER_H

#pragma once
#include <QAbstractNativeEventFilter>

class ShortCut;

class MyWinEventFilter : public QAbstractNativeEventFilter
{
public:
    MyWinEventFilter(ShortCut *shortcut);
    ~MyWinEventFilter();
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
private:
    ShortCut *m_shortcut;
};

#endif // WINEVENTFILTER_H
