#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
class MouseAct;
class KeyboardAct;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication*, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buttonGetPos_clicked();
    void onF7Callback();
    void onF10Callback();
    void onGetPosTimeout();
    int startAutoAlt();
    void actAlt();
    void actAug();
    void onGetXYStopCallback();

    void on_altButton_clicked();

    void on_augButton_clicked();

    void on_equButton_clicked();

    void on_startButton_clicked();

    void on_pushButton_clicked();

private:
    enum E_XY_POS_INDEX
    {
        E_XY_POS_ALT = 0,
        E_XY_POS_AUG,
        E_XY_POS_EQU,
    };
    int posIndex = 0;
    QTimer getPosTimer;
    Ui::MainWindow *ui;
    MouseAct* mouseAct;
    KeyboardAct* keyboardAct;
    int flagStart = 0;
    int altUsed = 0;
    int augUsed = 0;
    int attemptedTimes = 0;
};
#endif // MAINWINDOW_H
