#include "wineventfilter.h"
#include "shortcut.h"

MyWinEventFilter::~MyWinEventFilter()
{

}

MyWinEventFilter::MyWinEventFilter(ShortCut *shortcut)
    : m_shortcut(shortcut)
{

}

bool MyWinEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY)
        {
            const quint32 keycode = HIWORD(msg->lParam);
            const quint32 modifiers = LOWORD(msg->lParam);
            bool res = m_shortcut->shortcuts.value(qMakePair(keycode, modifiers));
            if (res)
            {
                m_shortcut->activateShortcut();
                return true;
            }
        }

    }
    return false;
}
