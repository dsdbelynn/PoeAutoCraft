#ifndef SHOUTCUT_H
#define SHOUTCUT_H
#include <QObject>
#include <QApplication>
#include "Windows.h"
#include <QKeySequence>
#include <QHash>
class MyWinEventFilter;
class ShortCut : public QObject
{
    Q_OBJECT
public:
    ShortCut(QString key, QApplication* app);
    ~ShortCut();
    void activateShortcut();
    bool registerHotKey();
    bool unregisterHotKey();
    QHash<QPair<quint32, quint32>, ShortCut*> shortcuts;
private:
    QApplication *app;
    MyWinEventFilter *filter;
    QKeySequence m_key;
    Qt::Key key;
    Qt::KeyboardModifiers mods;
    static quint32 nativeKeycode(Qt::Key keycode);
    static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers);
signals:
    void activated();
public slots:
};

#endif // SHOUTCUT_H
