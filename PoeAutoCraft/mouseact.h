#ifndef MOUSEACT_H
#define MOUSEACT_H

#include "Windows.h"
#include "WinUser.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>

#pragma comment(lib, "User32.lib")

void getMousePos(int* x,int* y) ;

void doPressAct(int x,int y,int type);

enum E_MOUSE_ACT_TYPE_ID{
    E_MOUSE_ACT_TYPE_LEFT_CLICK = 0,
    E_MOUSE_ACT_TYPE_RIGHT_CLICK,
    E_MOUSE_ACT_TYPE_NUM
};

#endif // MOUSEACT_H
