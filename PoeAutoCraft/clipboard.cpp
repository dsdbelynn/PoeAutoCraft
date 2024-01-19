#include "clipboard.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>

QString getClipBoard()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    auto d = clipboard->mimeData();
    if(d->hasText())
    {
        return clipboard->text();
    }
    else
    {
        return QString();
    }
}
