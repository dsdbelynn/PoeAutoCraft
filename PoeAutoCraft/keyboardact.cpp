#include "keyboardact.h"
#include "utility.h"
// press: press = 0, release = 1, press and release = 2;
void doKeyboardAct(BYTE key, int press)
{
    if(E_KEYBD_ACT_TYPE_PRESS == press)
    {
        keybd_event(key, 0, 0, 0);
    }
    else if(E_KEYBD_ACT_TYPE_RELEASE == press)
    {
        keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    }
    else if(E_KEYBD_ACT_TYPE_PRESS_AND_RELEASE == press)
    {
        keybd_event(key, 0, 0, 0);
        delayMs(20);
        keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
        delayMs(20);
    }
    else
    {

    }
    return;
}
