#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>
class MyLineEdit : public QLineEdit
{
public:
    MyLineEdit(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
private:
    int building = 0;

};

#endif // MYLINEEDIT_H
