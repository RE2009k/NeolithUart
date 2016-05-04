/************************************************
**
************************************************/
#include <QApplication>
#include "mainwindow.h"
#include "RStartScreen/rstartscreen.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDelayTime pdelay;

    //显示启动画面
    RStartScreen rw;
    rw.setWindowFlags(Qt::FramelessWindowHint);
    rw.show();
    rw.startRun();
    rw.hide();    
    //pdelay.delay_ms(500);

    //显示程序主界面
    MainWindow w;
    w.setWindowTitle("NeolithUart V1.3");
    w.showMaximized();
    w.show();

    /*以下代码引起问题, 且仅在软件关闭时出现错误提示框*/
    //rw.~RStartScreen();
    //pdelay.~QDelayTime();

    /*if writed as:
     * delete pdelay;
     * display error: delete expected pointer */

    //QObject Tree Debug
    qApp->dumpObjectTree();
    qApp->dumpObjectInfo();

    return a.exec();
}
