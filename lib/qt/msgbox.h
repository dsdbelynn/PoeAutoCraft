#ifndef MSGBOX_H
#define MSGBOX_H

#include <QObject>

class MsgBox : public QObject
{
    Q_OBJECT
public:
    enum EResult {
        CHECKED = 100
    };

    explicit MsgBox(QObject *parent = nullptr);

    static void info(QWidget* parent, const QString& text, const QString& title = QString(), const QString& btn = QString());
    static void warn(QWidget* parent, const QString& text, const QString& title = QString(), const QString& btn = QString());
    static void error(QWidget* parent, const QString& text, const QString& title = QString(), const QString& btn = QString());

    //问询对话框，btn0表示确定，btn1表示取消，返回确认选择，0表示btn0，1表示btn1，-1表示对话框关闭
    static int ask2(QWidget* parent, const QString& text, const QString& title = QString(),
                   const QString& btn0 = QString(), const QString& btn1 = QString());
    //三态问询对话框，btn0表示肯定，btn1表示否定，btn2表示取消，返回选择，0表示btn0，1表示btn1，2表示btn2，-1表示对话框关闭
    static int ask3(QWidget* parent, const QString& text, const QString& title = QString(),
                    const QString& btn0 = QString(), const QString& btn1 = QString(), const QString& btn2 = QString());
    //带复选框的问询对话框，btn0表示肯定，btn1表示否定，返回确认选择，100表示复选且btn0，0表示btn0，1表示btn1，-1表示对话框关闭
    static int ask2Checkable(QWidget* parent, const QString& check, const QString& text, const QString& title = QString(),
                          const QString& btn0 = QString(), const QString& btn1 = QString());
};

#endif // MSGBOX_H
