#include "msgbox.h"
#include <QtWidgets>

MsgBox::MsgBox(QObject *parent) : QObject(parent)
{

}
int MsgBox::ask2(QWidget* parent, const QString& text, const QString& title, const QString& btn0, const QString& btn1)
{
    QMessageBox msg(QMessageBox::Question,
                    title.isEmpty() ? tr("请确认") : title,
                    text, QMessageBox::NoButton, parent);
    QAbstractButton* b0 = msg.addButton(btn0.isEmpty() ? tr("确定") : btn0, QMessageBox::AcceptRole);
    QAbstractButton* b1 = msg.addButton(btn1.isEmpty() ? tr("取消") : btn1, QMessageBox::RejectRole);
    msg.exec();

    int ret = -1;
    QAbstractButton* bc = msg.clickedButton();
    if(b0 == bc)
    {
        ret = 0;
    }
    else if(b1 == bc)
    {
        ret = 1;
    }
    return ret;
}
int MsgBox::ask3(QWidget* parent, const QString& text, const QString& title, const QString& btn0, const QString& btn1, const QString& btn2)
{
    QMessageBox msg(QMessageBox::Question,
                    title.isEmpty() ? tr("请确认") : title,
                    text, QMessageBox::NoButton, parent);
    QAbstractButton* b0 = msg.addButton(btn0.isEmpty() ? tr("是") : btn0, QMessageBox::YesRole);
    QAbstractButton* b1 = msg.addButton(btn1.isEmpty() ? tr("否") : btn1, QMessageBox::NoRole);
    QAbstractButton* b2 = msg.addButton(btn2.isEmpty() ? tr("取消") : btn2, QMessageBox::RejectRole);
    msg.exec();

    int ret = -1;
    QAbstractButton* bc = msg.clickedButton();
    if(b0 == bc)
    {
        ret = 0;
    }
    else if(b1 == bc)
    {
        ret = 1;
    }
    else if(b2 == bc)
    {
        ret = 2;
    }
    return ret;
}
void MsgBox::info(QWidget* parent, const QString& text, const QString& title, const QString& btn)
{
    QMessageBox msg(QMessageBox::Information,
                    title.isEmpty() ? tr("提示") : title,
                    text, QMessageBox::NoButton, parent);
    msg.addButton(btn.isEmpty() ? tr("确定") : btn, QMessageBox::AcceptRole);
    msg.exec();
}
void MsgBox::warn(QWidget* parent, const QString& text, const QString& title, const QString& btn)
{
    QMessageBox msg(QMessageBox::Warning,
                    title.isEmpty() ? tr("警告") : title,
                    text, QMessageBox::NoButton, parent);
    msg.addButton(btn.isEmpty() ? tr("确定") : btn, QMessageBox::AcceptRole);
    msg.exec();
}
void MsgBox::error(QWidget* parent, const QString& text, const QString& title, const QString& btn)
{
    QMessageBox msg(QMessageBox::Critical,
                    title.isEmpty() ? tr("错误") : title,
                    text, QMessageBox::NoButton, parent);
    msg.addButton(btn.isEmpty() ? tr("确定") : btn, QMessageBox::AcceptRole);
    msg.exec();
}

int MsgBox::ask2Checkable(QWidget* parent, const QString& check, const QString& text, const QString& title, const QString& btn0, const QString& btn1)
{
    QCheckBox* box = new QCheckBox(check, parent);
    QMessageBox msg(QMessageBox::Question,
                    title.isEmpty() ? tr("请确认") : title,
                    text,
                    QMessageBox::NoButton,
                    parent);
    QAbstractButton* b0 = msg.addButton(btn0.isEmpty() ? tr("确定") : btn0, QMessageBox::AcceptRole);
    QAbstractButton* b1 = msg.addButton(btn1.isEmpty() ? tr("取消") : btn1, QMessageBox::RejectRole);
    msg.setCheckBox(box);
    msg.exec();

    int ret = -1;
    QAbstractButton* bc = msg.clickedButton();
    if(b0 == bc)
    {
        if(box->isChecked())
        {
            ret = CHECKED;
        }
        else
        {
            ret = 0;
        }
    }
    else if(b1 == bc)
    {
        ret = 1;
    }
    box->deleteLater();
    return ret;
}
