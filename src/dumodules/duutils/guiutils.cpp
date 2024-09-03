#include "guiutils.h"

QMainWindow *GuiUtils::appMainWindow()
{
    foreach(QWidget *w, qApp->allWidgets())
    {
        if (w->inherits("MainWindow"))
        {
            QMainWindow *mw = qobject_cast<QMainWindow*>(w);
            if (mw) return mw;
        }
    }
    return nullptr;
}
