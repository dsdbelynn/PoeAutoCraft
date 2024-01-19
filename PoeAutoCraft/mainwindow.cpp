#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shortcut.h"
#include "clipboard.h"
#include <QTime>
#include "mouseact.h"
#include "keyboardact.h"
#include "utility.h"
static volatile int flagStop = 0;

#define actSpan 100
MainWindow::MainWindow(QApplication* app, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto a = dynamic_cast<QApplication*>(app);
    QObject::connect(&getPosTimer, SIGNAL(timeout()), this, SLOT(onGetPosTimeout()));
//    ShortCut *shortcut2 = new ShortCut(ui->stop->text(), a);
//    QObject::connect(shortcut2, SIGNAL(activated()), this, SLOT(onGetXYStopCallback()));
    ShortCut *shortcut1 = new ShortCut("CTRL+F", a);
//    QObject::connect(shortcut1, SIGNAL(activated()), this, SLOT(onF10Callback()));

}

void MainWindow::on_altButton_clicked()
{
    posIndex = E_XY_POS_ALT;
    getPosTimer.start(10);
}

void MainWindow::on_augButton_clicked()
{
    posIndex = E_XY_POS_AUG;
    getPosTimer.start(10);
}

void MainWindow::on_equButton_clicked()
{
    posIndex = E_XY_POS_EQU;
    getPosTimer.start(10);
}

void MainWindow::onGetPosTimeout()
{
    int x;
    int y;
    getMousePos(&x, &y);
    switch(posIndex)
    {
        case E_XY_POS_ALT:
        {
            ui->alt_x->setText(QString("%1").arg(x));
            ui->alt_y->setText(QString("%1").arg(y));
            break;
        }
        case E_XY_POS_AUG:
        {
            ui->aug_x->setText(QString("%1").arg(x));
            ui->aug_y->setText(QString("%1").arg(y));
            break;
        }
        case E_XY_POS_EQU:
        {
            ui->equ_x->setText(QString("%1").arg(x));
            ui->equ_y->setText(QString("%1").arg(y));
            break;
        }
        default:
        {
            break;
        }
    }
    ui->editPosX->setText(QString("%1").arg(x));
    ui->editPosY->setText(QString("%1").arg(y));
}


void MainWindow::onGetXYStopCallback()
{
    flagStop = 1;
    getPosTimer.stop();
}

void MainWindow::actAlt()
{
    int alt_x = ui->alt_x->text().toInt();
    int alt_y = ui->alt_y->text().toInt();
    int equ_x = ui->equ_x->text().toInt();
    int equ_y = ui->equ_y->text().toInt();
    //右键改造
    doPressAct(alt_x, alt_y, E_MOUSE_ACT_TYPE_RIGHT_CLICK);
    delayMs(actSpan);
    //左键装备
    doPressAct(equ_x, equ_y, E_MOUSE_ACT_TYPE_LEFT_CLICK);
    delayMs(actSpan);

}
void MainWindow::actAug()
{
    int aug_x = ui->aug_x->text().toInt();
    int aug_y = ui->aug_y->text().toInt();
    int equ_x = ui->equ_x->text().toInt();
    int equ_y = ui->equ_y->text().toInt();
    //右键增幅
    doPressAct(aug_x, aug_y, E_MOUSE_ACT_TYPE_RIGHT_CLICK);
    delayMs(actSpan);
    //左键装备
    doPressAct(equ_x, equ_y, E_MOUSE_ACT_TYPE_LEFT_CLICK);
    delayMs(actSpan);
}
/*
 * 返回值：
 * 0:   成功
 * -1:  改造石不足
 * -2:  增幅石不足
 * -3:  玩家中止
 * -4:  未知错误
 *
 * 改造状态机
 * 状态0：点改造，改造计数+1，去状态2
 * 状态1：点增幅，增幅计数+1，去状态2
 * 状态2：判断词缀，有全+1就停，没全+1有前缀就去状态0，没前缀就去状态1
 */
int MainWindow::startAutoAlt()
{
    altUsed = 0;
    augUsed = 0;
    attemptedTimes = 0;
    ui->altUsed->setText(QString("改造石使用：%1").arg(altUsed));
    ui->augUsed->setText(QString("增幅石使用：%1").arg(augUsed));
    ui->attemptedTimes->setText(QString("%1").arg(attemptedTimes));
    ui->startButton->setDisabled(true);
    //获取参数信息
    QString target = ui->target->text();
    int altNum = ui->altNum->text().toInt();
    int augNum = ui->augNum->text().toInt();
    int state = 0;
    while(0 == flagStop)
    {
        if(0 == state)
        {
            if(altUsed >= altNum)
            {
                return -1;
            }
            altUsed++;
            attemptedTimes++;
            ui->altUsed->setText(QString("改造石使用：%1").arg(altUsed));
            ui->attemptedTimes->setText(QString("%1").arg(attemptedTimes));
            state = 2;
            //点改造
            actAlt();
        }
        else if(1 == state)
        {
            if(augUsed >= augNum)
            {
                return -2;
            }
            augUsed++;
            attemptedTimes++;
            ui->augUsed->setText(QString("增幅石使用：%1").arg(augUsed));
            ui->attemptedTimes->setText(QString("%1").arg(attemptedTimes));
            state = 2;
            actAug();
        }
        else if(2 == state)
        {
            //判断词缀
            onF10Callback();
            QString str = getClipBoard();
            if(str.contains(target))
            {
                return 0;
            }
            else
            {
                if(str.contains("前缀"))
                {
                    state = 0;
                }
                else
                {
                    state = 1;
                }
            }
        }
        else
        {
            break;
        }
    }
    if(1 == flagStop)
    {
        return -3;
    }
    return -3;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonGetPos_clicked()
{
    int x;
    int y;
    getMousePos(&x, &y);
    ui->editPosX->setText(QString("%1").arg(x));
    ui->editPosY->setText(QString("%1").arg(y));
}

void MainWindow::onF7Callback()
{
    qDebug("F7");
    on_buttonGetPos_clicked();
}

void MainWindow::onF10Callback()
{
    qDebug("F10");
    doKeyboardAct(VK_LCONTROL, 0);
    delayMs(20);
    doKeyboardAct(VK_LMENU, 0);
    delayMs(20);
    doKeyboardAct('C', 0);
    delayMs(20);
    doKeyboardAct('C', 1);
    delayMs(20);
    doKeyboardAct(VK_LMENU, 1);
    delayMs(20);
    doKeyboardAct(VK_LCONTROL, 1);
    delayMs(20);
    auto t = getClipBoard();
    ui->plainTextEdit->setPlainText(t);
}

void MainWindow::on_startButton_clicked()
{
    flagStop = 0;
    int ret = startAutoAlt();
    ui->startButton->setEnabled(true);
    if(0 == ret)
    {
        ui->startButton->setText("成功");
    }
    else if(-1 == ret)
    {
        ui->startButton->setText("改造石不足");
    }
    else if(-2 == ret)
    {
        ui->startButton->setText("增幅石不足");
    }
    else if(-3 == ret)
    {
        ui->startButton->setText("玩家中止");
    }
    else
    {
        ui->startButton->setText("未知错误");
    }

}

void MainWindow::on_pushButton_clicked()
{

}
