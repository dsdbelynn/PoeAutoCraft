#include "mouseact.h"
#include "utility.h"

#define MOUSEACT_SPAN 20

void getMousePos(int*x,int*y)
{
    POINT pos;
    pos.x = 0;
    pos.y = 0;
    GetCursorPos(&pos);
    *x = pos.x;
    *y = pos.y;
}

void doPressAct(int x,int y,int type_id)
{
    if(!SetCursorPos(x,y))
    {
        return;
    }
    delayMs(MOUSEACT_SPAN);
    auto xx = static_cast<DWORD>(x);
    auto yy = static_cast<DWORD>(y);
    if(type_id == 1)
    {
        //鼠标左键单击
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN,xx, yy, 0, 0);//按下
        delayMs(MOUSEACT_SPAN);
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP,xx, yy, 0, 0);//松开
    }
    else if(type_id == 3)
    {
        //鼠标右键单击
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN,xx, yy, 0, 0);
        delayMs(MOUSEACT_SPAN);
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP,xx, yy, 0, 0);
    }
}
//    else if(type_id==2)
//    {
//        //鼠标左键双击
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP,x, y, 0, 0);

//    }
//    else if(type_id == 3)
//    {
//        //鼠标右键单击
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN,x, y, 0, 0);
//        delayMs(20);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP,x, y, 0, 0);
//    }
//    else if(type_id == 4)
//    {
//        //鼠标右键双击
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP,x, y, 0, 0);
//    }
//    else if(type_id == 5)
//    {
//        //鼠标移动
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE ,
//                    x * 65536 / desktop_width,y * 65536 / desktop_height,0,GetMessageExtraInfo());

//    }else if(type_id==6)
//    {
//        //鼠标拖拽
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN,x, y, 0, 0);
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE ,
//                    x1 * 65536 / desktop_width,y1 * 65536 / desktop_height,0,GetMessageExtraInfo());
//        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP,x1, y1, 0, 0);
//    }
