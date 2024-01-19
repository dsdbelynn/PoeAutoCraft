#ifndef KEYBOARDACT_H
#define KEYBOARDACT_H

#include <QObject>
#include "Windows.h"
#include "WinUser.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#pragma comment(lib, "User32.lib")
enum E_KEYBD_ACT_TYPE{
    E_KEYBD_ACT_TYPE_PRESS = 0,
    E_KEYBD_ACT_TYPE_RELEASE,
    E_KEYBD_ACT_TYPE_PRESS_AND_RELEASE
};
// press: press = 0, release = 1, press and release = 2;
void doKeyboardAct(BYTE key, int press);

#endif // KEYBOARDACT_H
