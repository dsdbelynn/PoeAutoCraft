#include "mylineedit.h"


MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    // 设置焦点策略，确保LineEdit能够接收键盘事件
    //setFocusPolicy(Qt::StrongFocus);
}



void MyLineEdit::keyPressEvent(QKeyEvent *event)
{
    int k[4] = {0};
    int index = 0;
    if(building <= 0)
    {
        return;
    }
    QString ret;
    // 获取键盘按键的文本表示
    int key = event->key();
    int modifiers = event->modifiers();
    QString keyText = event->text();
    if((0 == key) && (Qt::NoModifier == modifiers))
    {
        return;
    }
    if((Qt::Key_Control == key) || (Qt::Key_Shift == key) || ((Qt::Key_Alt == key)))
    {
        this->clear();
        return;
    }
    if((Qt::Key_Backspace == key) && (Qt::NoModifier == modifiers))
    {
        this->clear();
        return;
    }
    if (modifiers & Qt::ControlModifier)
    {
        ret += "Ctrl + ";
        k[index++] = Qt::ControlModifier;
    }
    if (modifiers & Qt::AltModifier)
    {
        ret += "Alt + ";
        k[index++] = Qt::AltModifier;
    }
    if (modifiers & Qt::ShiftModifier)
    {
        ret += "Shift + ";
        k[index++] = Qt::ShiftModifier;
    }
    {
        k[index++] = key;
    }
    auto ks = QKeySequence(k[0],k[1],k[2],k[3]);
    auto str = ks.toString();
    int size = str.size();
    for(int i = 0; i < size; i++)
    {
        if(str.at(i) == ',')
        {
            str.remove(i,1);
            i--;
        }
        size = str.size();
    }
    int tail = str.size() - 1;
    if('~' == str.at(tail)) str.replace(tail, 1, '`');
    if('!' == str.at(tail)) str.replace(tail, 1, '1');
    if('@' == str.at(tail)) str.replace(tail, 1, '2');
    if('#' == str.at(tail)) str.replace(tail, 1, '3');
    if('$' == str.at(tail)) str.replace(tail, 1, '4');
    if('%' == str.at(tail)) str.replace(tail, 1, '5');
    if('^' == str.at(tail)) str.replace(tail, 1, '6');
    if('&' == str.at(tail)) str.replace(tail, 1, '7');
    if('*' == str.at(tail)) str.replace(tail, 1, '8');
    if('(' == str.at(tail)) str.replace(tail, 1, '9');
    if(')' == str.at(tail)) str.replace(tail, 1, '0');
    if('_' == str.at(tail)) str.replace(tail, 1, '-');
    if('+' == str.at(tail)) str.replace(tail, 1, '=');
    if('{' == str.at(tail)) str.replace(tail, 1, '[');
    if('}' == str.at(tail)) str.replace(tail, 1, ']');
    if('|' == str.at(tail)) str.replace(tail, 1, '\\');
    if(':' == str.at(tail)) str.replace(tail, 1, ';');
    if('"' == str.at(tail)) str.replace(tail, 1, '\'');
    if('<' == str.at(tail)) str.replace(tail, 1, ',');
    if('>' == str.at(tail)) str.replace(tail, 1, '.');
    if('?' == str.at(tail)) str.replace(tail, 1, '/');

    //ret += QString("[key=0x%1][keyText=%2]").arg(key, 0, 16).arg(keyText);
    ret = ks.toString(QKeySequence::NativeText);
    // 在LineEdit中显示按下的键盘组合键

    //禁止调用基类的keyPressEvent处理其他事件
    //QLineEdit::keyPressEvent(event);

    setText(str);
}
void MyLineEdit::focusInEvent(QFocusEvent *e)
{
    building = 1;
    qDebug("获得焦点");
    QLineEdit::focusInEvent(e);
}

void MyLineEdit::focusOutEvent(QFocusEvent *e)
{
    building = 0;
    qDebug("失去焦点");
    QLineEdit::focusOutEvent(e);
}
